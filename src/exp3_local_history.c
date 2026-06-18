/*
    EXPERIMENTO 3 - Evidencias de histórico local

    Objetivo: Determinar se uma branch princpical consegue manter seu
    própio padrão mesmo quando inserimos vários branches intermediários entre suas execuções

    Se sim, isso sugere algum mecanismo com histórico local, isto é, info
    associada ao própio branch.

    Para isso a ideia é criar um "spy branch" com um padrão periódico própio
    e entre cada execução ir inserindo N branches dummy previsíveis:

    spy
    dummy dummy dummy
    spy
    dummy dummy dummy
    .
    .
    .

    A hipótese é, se caso o spy branch continuar sendo bem previst mesmo com muitos dummies entre suas 
    execuções então isso sugere que o preditor mntem info especifica daquele branch.

    Com isso vamos comparar dois modos:
        - local_pattern: o spy branch segue um padrão periodico
        - random_pattern: o spy branch segue uma sequencia pseudoaleatória

    Se local_pattern < random_pattern mesmo com N dummies então o preditor 
    aprendeu comportamento repetitivo da branch principal
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#define MAX_PERIOD 64
#define MAX_GAP 128

static uint8_t local_pattern[MAX_PERIOD];


// foi usado potencia de 2 no periodo para permitir indexação
// circular por mascara de bists, evitando op de modulo /div dentro do loop principal da microbrenchmark
__attribute__((noinline))
uint64_t local_spy_pattern(size_t period, size_t gap)
{
    uint64_t local = 0;
    size_t pattern_index = 0;
    size_t period_mask = period - 1;

    for (size_t i = 0; i < ITER; i++)
    {
        uint8_t spy = local_pattern[pattern_index];

        if(spy)
            local++;

        // padrão periodico
        pattern_index = (pattern_index + 1) & period_mask;

        for (size_t j = 0; j < gap; j++){
            if(always_true)
                local++;
        }
    }

    sink += local;
    return local;
}


__attribute__((noinline))
uint64_t random_spy_pattern(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        uint8_t spy = pattern1[i & PATTERN_MASK];

        if (spy)
            local++;

        for (size_t j = 0; j < gap; j++) {
            if (always_true)
                local++;
        }
    }

    sink += local;
    return local;
}

/**
 * (!) Resultados esperados:
 * local period [n] gap 0 << random gap 0
 * indica que o padrão periodico do spy branch foi aprendido
 * 
 * se continuar baixo mesmo com muitos dummies
 * é evidencia compátivel com historico local ou preditor hibrido pq a info
 * do spy branch sobrevive mesmo com bracn interm. preenchendo o hist. global recente
*/


// ==== AUX FUNCTIONS ==== //

static void init_local_pattern(size_t period)
{
    for (size_t i = 0; i < period; i++)
        local_pattern[i] = 1;

    local_pattern[period - 1] = 0;
}

static int is_power_of_two(size_t value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

static int parse_args(int argc, char **argv, size_t *period, size_t *gap)
{
    if (argc < 3) {
        printf("Uso:\n");
        printf("  %s local <period> <gap>\n", argv[0]);
        printf("  %s random <gap>\n", argv[0]);
        return 0;
    }

    char *end = NULL;

    if (strcmp(argv[1], "local") == 0) {
        if (argc < 4) {
            printf("Uso: %s local <period> <gap>\n", argv[0]);
            return 0;
        }

        unsigned long p = strtoul(argv[2], &end, 10);

        if (end == argv[2] || *end != '\0') {
            printf("Period inválido: %s\n", argv[2]);
            return 0;
        }

        if (p < 2 || p > MAX_PERIOD || !is_power_of_two((size_t)p)) {
            printf("Period deve ser potência de 2 entre 2 e %d\n", MAX_PERIOD);
            return 0;
        }

        *period = (size_t)p;

        end = NULL;
        unsigned long g = strtoul(argv[3], &end, 10);

        if (end == argv[3] || *end != '\0') {
            printf("Gap inválido: %s\n", argv[3]);
            return 0;
        }

        if (g > MAX_GAP) {
            printf("Gap máximo permitido: %d\n", MAX_GAP);
            return 0;
        }

        *gap = (size_t)g;
        return 1;
    }

    if (strcmp(argv[1], "random") == 0) {
        unsigned long g = strtoul(argv[2], &end, 10);

        if (end == argv[2] || *end != '\0') {
            printf("Gap inválido: %s\n", argv[2]);
            return 0;
        }

        if (g > MAX_GAP) {
            printf("Gap máximo permitido: %d\n", MAX_GAP);
            return 0;
        }

        *period = 0;
        *gap = (size_t)g;
        return 1;
    }

    printf("Modo inválido: %s\n", argv[1]);
    return 0;
}


int main(int argc, char **argv)
{
    size_t period = 0;
    size_t gap = 0;

    if (!parse_args(argc, argv, &period, &gap))
        return 1;

    init_patterns();

    uint64_t result = 0;

    if (strcmp(argv[1], "local") == 0) {
        init_local_pattern(period);
        result = local_spy_pattern(period, gap);
    } else {
        result = random_spy_pattern(gap);
    }

    printf("mode=%s period=%zu gap=%zu result=%lu sink=%lu\n",
           argv[1], period, gap, result, sink);

    return 0;
}










