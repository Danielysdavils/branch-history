/*
    EXPERIMENTO 4 - Estimar tamanho efetivo

    Objetivos: Usar variações sistemáticas de gap e period para estimar o "alcance efetivo" do
    histórico observado, sem afirmar tamanho fisíco exato do preditor

    Então vamos ter duas estimativas:
        1. Tamnaho efetivo de hist. global: usando o experimento 2, até qual gap
        o caso correlated continua melhor que uncorrelated?

        2. Tamanho efetivo de hist. local: Até qual period o spy branch periodico continua claramente
        melhor que o spy branch pseudoaleatório?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#define MAX_GAP 128
#define MAX_PERIOD 64

static uint8_t local_pattern[MAX_PERIOD];

static int is_power_of_two(size_t value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

static int parse_size_arg(const char *text, size_t *out)
{
    char *end = NULL;
    unsigned long value = strtoul(text, &end, 10);

    if (end == text || *end != '\0')
        return 0;

    *out = (size_t)value;
    return 1;
}

static void init_local_pattern(size_t period)
{
    for (size_t i = 0; i < period; i++)
        local_pattern[i] = 1;

    local_pattern[period - 1] = 0;
}

__attribute__((noinline))
uint64_t global_correlated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        size_t idx = i & PATTERN_MASK;

        uint8_t a = pattern1[idx];
        uint8_t b = pattern_same[idx];

        if (a)
            local++;

        for (size_t j = 0; j < gap; j++) {
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            // if (always_true)
            //     local++;
            if(d)
                local++;
        }

        if (b)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t global_uncorrelated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        uint8_t a = pattern1[i & PATTERN_MASK];
        uint8_t b = pattern2[i & PATTERN_MASK];

        if (a)
            local++;

        for (size_t j = 0; j < gap; j++) {
            // if (always_true)
            //     local++;
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if(d)
                local++;
        }

        if (b)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t global_anticorrelated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        size_t idx = i & PATTERN_MASK;

        uint8_t a = pattern1[idx];
        uint8_t inv_a = pattern_inv[idx]; 

        if (a)
            local++;

        for (size_t j = 0; j < gap; j++) {
            // if (always_true)
            //     local++;
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if(d)
                local++;
        }

        if (inv_a)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t local_periodic_pattern(size_t period, size_t gap)
{
    uint64_t local = 0;
    size_t period_mask = period - 1;
    size_t pattern_index = 0;

    for (size_t i = 0; i < ITER; i++) {
        uint8_t spy = local_pattern[pattern_index];

        if (spy)
            local++;

        pattern_index = (pattern_index + 1) & period_mask;

        for (size_t j = 0; j < gap; j++) {
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if (d)
                local++;
        }
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t local_random_pattern(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        uint8_t spy = pattern1[i & PATTERN_MASK];

        if (spy)
            local++;

        for (size_t j = 0; j < gap; j++) {
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if (d)
                local++;
        }
    }

    sink += local;
    return local;
}

static int run_global(int argc, char **argv, uint64_t *result)
{
    if (argc < 4) {
        printf("Uso: %s global <correlated|uncorrelated|anticorrelated> <gap>\n", argv[0]);
        return 0;
    }

    size_t gap = 0;

    if (!parse_size_arg(argv[3], &gap) || gap > MAX_GAP) {
        printf("Gap inválido. Use valor entre 0 e %d\n", MAX_GAP);
        return 0;
    }

    if (strcmp(argv[2], "correlated") == 0)
        *result = global_correlated_gap(gap);
    else if (strcmp(argv[2], "uncorrelated") == 0)
        *result = global_uncorrelated_gap(gap);
    else if (strcmp(argv[2], "anticorrelated") == 0)
        *result = global_anticorrelated_gap(gap);
    else {
        printf("Modo global inválido: %s\n", argv[2]);
        return 0;
    }

    printf("scope=global mode=%s gap=%zu result=%lu sink=%lu\n",
           argv[2], gap, *result, sink);

    return 1;
}

static int run_local(int argc, char **argv, uint64_t *result)
{
    if (argc < 4) {
        printf("Uso: %s local <period> <gap>\n", argv[0]);
        return 0;
    }

    size_t period = 0;
    size_t gap = 0;

    if (!parse_size_arg(argv[2], &period) ||
        period < 2 ||
        period > MAX_PERIOD ||
        !is_power_of_two(period)) {
        printf("Period inválido. Use potência de 2 entre 2 e %d\n", MAX_PERIOD);
        return 0;
    }

    if (!parse_size_arg(argv[3], &gap) || gap > MAX_GAP) {
        printf("Gap inválido. Use valor entre 0 e %d\n", MAX_GAP);
        return 0;
    }

    init_local_pattern(period);
    *result = local_periodic_pattern(period, gap);

    printf("scope=local mode=periodic period=%zu gap=%zu result=%lu sink=%lu\n",
           period, gap, *result, sink);

    return 1;
}

static int run_random(int argc, char **argv, uint64_t *result)
{
    if (argc < 3) {
        printf("Uso: %s random <gap>\n", argv[0]);
        return 0;
    }

    size_t gap = 0;

    if (!parse_size_arg(argv[2], &gap) || gap > MAX_GAP) {
        printf("Gap inválido. Use valor entre 0 e %d\n", MAX_GAP);
        return 0;
    }

    *result = local_random_pattern(gap);

    printf("scope=local mode=random gap=%zu result=%lu sink=%lu\n",
           gap, *result, sink);

    return 1;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Uso:\n");
        printf("  %s global <correlated|uncorrelated|anticorrelated> <gap>\n", argv[0]);
        printf("  %s local <period> <gap>\n", argv[0]);
        printf("  %s random <gap>\n", argv[0]);
        return 1;
    }

    init_patterns();

    uint64_t result = 0;
    int ok = 0;

    if (strcmp(argv[1], "global") == 0)
        ok = run_global(argc, argv, &result);
    else if (strcmp(argv[1], "local") == 0)
        ok = run_local(argc, argv, &result);
    else if (strcmp(argv[1], "random") == 0)
        ok = run_random(argc, argv, &result);
    else {
        printf("Escopo inválido: %s\n", argv[1]);
        return 1;
    }

    return ok ? 0 : 1;
}