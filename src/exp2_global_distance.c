/*
    EXPERIMENTO 2 - DISTANCIA ENTRE BRANCHES CORRELACIONADAS

    OBjetivo: Verificar se, caso a Branch B dependa da Branch A, essa relação ainda ajuda
    quando existem N branches dummy entre A e B?

    A ideia é comparar dois modos: 
        - correlated_gap
        - uncorrelated_gap

    Em ambos temos:
        Branch A
         N dummy branches
        Branch B
    
    A diferença é: 
        correlated_gap:
            A = pattern1[i]
            B = pattern1[i]
        
        uncorrelated_gap:
            A = pattern1[i]
            B = pattern2[i]

    Se miss rate de correlated_gap < uncorrelated_gap, então 
    isso sugree que o histórico global ainda carrega info útil sobre A
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#define MAX_GAP 120 

// ajustamos conforme pattern_same en exp1
__attribute__((noinline))
uint64_t correlated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++)
    {
        size_t idx = i & PATTERN_MASK;

        uint8_t a = pattern1[idx];
        uint8_t b = pattern_same[idx];

        if(a)
            local++;

        for (size_t j = 0; j < gap; j++){
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
uint64_t uncorrelated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        uint8_t a = pattern1[i & PATTERN_MASK];
        uint8_t b = pattern2[i & PATTERN_MASK];

        if (a)
            local++;

        for (size_t j = 0; j < gap; j++) {
            // branch dummy
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if (d)
                local++;

            // if (always_true)
            //     local++;
        }

        if (b)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t anticorrelated_gap(size_t gap)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++) {
        size_t idx = i & PATTERN_MASK;

        uint8_t a = pattern1[idx];
        uint8_t inv_a = pattern_inv[idx];

        if (a)
            local++;

        for (size_t j = 0; j < gap; j++) {
            // branch dummy
            uint8_t d = dummy_pattern[j & DUMMY_MASK];
            if (d)
                local++;
        }

        if (inv_a)
            local++;
    }

    sink += local;
    return local;
}

/*

    (!) RESULTADOS ESPERADOS (APAGAR DEPOS):
    Se miss rate correlated_gap_n < uncorrelated_gap_n
    para valores pequenos de gap então há evidencias de correlação/hist. global

    se a diferença vai sumindo conforme gap aumenta então sugere um limite efetivo para a info de histórico
    usada nesse experiemnto - (não diz diratamente sobre tamanho do historico, só sobre alcance efetivo)
*/


static int parse_gap(int argc, char **argv, size_t *gap)
{
    if (argc < 3) {
        printf("Uso: %s <correlated|uncorrelated|anticorrelated> <gap>\n", argv[0]);
        return 0;
    }

    char *end = NULL;
    unsigned long value = strtoul(argv[2], &end, 10);

    if (end == argv[2] || *end != '\0') {
        printf("Gap inválido: %s\n", argv[2]);
        return 0;
    }

    if (value > MAX_GAP) {
        printf("Gap máximo permitido: %d\n", MAX_GAP);
        return 0;
    }

    *gap = (size_t)value;
    return 1;
}


int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Uso: %s <correlated|uncorrelated|anticorrelated> <gap>\n", argv[0]);
        return 1;
    }

    size_t gap = 0;

    if (!parse_gap(argc, argv, &gap))
        return 1;

    init_patterns();

    uint64_t result = 0;

    if (strcmp(argv[1], "correlated") == 0)
        result = correlated_gap(gap);
    else if (strcmp(argv[1], "uncorrelated") == 0)
        result = uncorrelated_gap(gap);
    else if (strcmp(argv[1], "anticorrelated") == 0)
        result = anticorrelated_gap(gap);
    else {
        printf("Modo inválido: %s\n", argv[1]);
        return 1;
    }

    printf("mode=%s gap=%zu result=%lu sink=%lu\n",
           argv[1], gap, result, sink);

    return 0;
}