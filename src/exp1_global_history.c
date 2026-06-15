/*

    EXPERIMENTO 1 - EVIDENCIA DE HISTÓRICO GLOBAL 

    Objetivo: Testar se o resultado de um branch anterior ajuda o processador a prever um branch posterior   

*/


#include <stdio.h>
#include <string.h>
#include "common.h"

__attribute__((noinline))
uint64_t single_random()
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++)
    {
        uint8_t a = pattern1[i & PATTERN_MASK];

        if(a)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t double_uncorrelated(void)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++)
    {
        uint8_t a = pattern1[i & PATTERN_MASK];
        uint8_t b = pattern2[i & PATTERN_MASK];

        if(a)
            local++;

        if(b)
            local++;
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t double_correlated(void)
{
    uint64_t local = 0;

    for (size_t i = 0; i < ITER; i++)
    {
        uint8_t a = pattern1[i & PATTERN_MASK];

        if(a) local++;

        if(a) local++; 
    }

    sink += local;
    return local;
}

__attribute__((noinline))
uint64_t double_anticorrelated(void)
{
    uint64_t local = 0;


    for (size_t i = 0; i < ITER; i++)
    {
        uint8_t a = pattern1[i & PATTERN_MASK];

        if(a) local++;

        if(!a) local++;
    }

    sink += local;
    return local;
}



