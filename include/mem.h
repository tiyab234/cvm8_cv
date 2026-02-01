/*
    Copyright (c) 2026 - Yann BOYER
*/
#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#define TOTAL_MEMORY_SIZE 0x1000

typedef struct {
    uint8_t mem[TOTAL_MEMORY_SIZE];
} Memory;

// No deinit function needed, no dynamic alloc.
void mem_init(Memory* mem);
uint8_t mem_read(Memory* mem, uint16_t addr);
void mem_write(Memory* mem, uint16_t addr, uint8_t value);

#endif
