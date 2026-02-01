/*
    Copyright (c) 2026 - Yann BOYER
*/
#include "mem.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void mem_init(Memory* mem) {
    for (size_t i = 0; i < TOTAL_MEMORY_SIZE; i++) {
        mem->mem[i] = 0;
    }
}

uint8_t mem_read(Memory* mem, uint16_t addr) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        fprintf(stderr, "[FATAL ERROR] Memory READ command is out of range !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    return mem->mem[addr];
}

void mem_write(Memory* mem, uint16_t addr, uint8_t value) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        fprintf(stderr, "[FATAL ERROR] Memory WRITE command is out of range !\n");
        exit(EXIT_FAILURE);
    }

    mem->mem[addr] = value;
}
