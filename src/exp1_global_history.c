/*

    EXPERIMENTO 1 - EVIDENCIA DE HISTÓRICO GLOBAL 

    Objetivo: Testar se o resultado de um branch anterior ajuda o processador a prever um branch posterior   

    A ideia é comparar dois cenários:
        - A: branches correlacionadas
        - B: branches independentes
*/


#include <stdio.h>
#include <string.h>
#include "common.h"

/*  branch pseudoaleatório sozinho - serve como baseline de branch dificil;
    quanto o preditor erra quando existe apenas um branch dificil
    sem outro branch para ajudar ou interferir?
    
    - single_random miss alto -> o padrão é dificl
    - single_random miss baixo -> o teste nao está estressando o preditor como esperado

    (*) caso resulte miss baixo temos um problema em q tão dificil é essa branch
*/
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

/* 
    dois branch pseudoaleatórios independentes - serve como caso sem correlação
    o que acontece quando temos dois branch dificeis no mesmo loop,
    mas sem relação útil entre eles?
*/
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

// dois branch com mesmo resultado - serve como caso onde o histórico global
// pode ajudar, pois o resultado recente de branch A pode ajudar a prever branch B 
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

// testa de preditor aprende relação oposta (B = !A)
// ajuda a verificar se o preditor aprende apenas "repetição" ou se consegue capturar uma relação mais geral

// se o preditor fosse bom só quando dois branches tem o mesmo resultado correlated melhoraria mas anticorrelated talvez naõ
// se anticorrelated também melhora em relaçao a uncorrelated, a evidencia fica mais interessante:
// o preditor consegue explorar contexto histórico, não apenas repetir últimos resultados

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

/**
 * Resultados esperados:
 * (!) apagar depois. Deixar melhor no README
 * 
 * single_random: miss rate alto -> por ser aleatório preditor deve sofrer
 * 
 * double_uncorrelated: miss rate alto -> as duas aleatórias sem relação. PReditor não deve encontrar padrões evidentes entre uma e outra
 * 
 * double_correlated: miss rate menor que double_uncorrelated -> há formas de encontrar padrões e aprender deles 
 * 
 * double_anticorrelated: miss rate menor -> é provável que aprenda predição inversa (padrões especificos)
*/

int main (int argc, char **argv)
{
    if(argc < 2){
        printf("Uso: %s <single|uncorrelated|correlated|anticorrelated>\n", argv[0]);
        return 1;
    }

    init_patterns();

    uint64_t result = 0;

    if (strcmp(argv[1], "single") == 0)
        result = single_random();

    else if (strcmp(argv[1], "uncorrelated") == 0)
        result = double_uncorrelated();

    else if (strcmp(argv[1], "correlated") == 0)
        result = double_correlated();

    else if (strcmp(argv[1], "anticorrelated") == 0)
        result = double_anticorrelated();
    else {
        printf("Modo inválidp\n");
        return 1;
    }

    printf("result=%lu sink=%lu\n", result, sink);
    return 0;
}
