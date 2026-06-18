#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

#define ITER 100000000UL
#define PATTERN_SIZE 1048576UL
#define PATTERN_MASK (PATTERN_SIZE - 1)

extern uint8_t pattern1[PATTERN_SIZE];
extern uint8_t pattern2[PATTERN_SIZE];

extern volatile uint64_t sink;
extern volatile uint8_t always_true;

void init_patterns(void);

#endif