#include "common.h"

uint8_t pattern1[PATTERN_SIZE];
uint8_t pattern2[PATTERN_SIZE];

volatile uint64_t sink = 0;
volatile uint8_t always_true = 1;

void init_patterns(void)
{
    uint32_t seed1 = 12345;
    uint32_t seed2 = 98765;

    for (size_t i = 0; i < PATTERN_SIZE; i++)
    {
        seed1 = 1664525u * seed1 + 1013904223u;
        seed2 = 22695477u * seed2 + 1u;

        pattern1[i] = (seed1 >> 31) & 1;
        pattern2[i] = (seed2 >> 31) & 1;
    }
}

