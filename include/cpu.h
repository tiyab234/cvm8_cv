/*
    Copyright (c) 2026 - Yann BOYER
*/
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "audio.h"
#include "render_engine.h"
#include "mem.h"

typedef enum {
    KEY_PRESSED = 1,
    KEY_NOT_PRESSED = 0,
} KeyState;

#define REGS_COUNT 16
#define KEYS_COUNT 16

typedef struct {
    uint8_t v_regs[REGS_COUNT]; // V Registers V0 -> VF.
    uint16_t* stack;
    KeyState keys[KEYS_COUNT];
    uint16_t index_reg;
    uint8_t delay_tm; // Delay Timer.
    uint8_t sound_tm; // Sound Timer.
    uint16_t pc; // Program Counter.
} CPU;

void cpu_init(CPU* cpu);
void cpu_deinit(CPU* cpu);
void cpu_update_timers(CPU* cpu, AudioPlayer* audiopl);
uint16_t cpu_fetch_next_op(CPU* cpu, Memory* mem);
void cpu_decode_and_execute(CPU* cpu, Memory* mem, RenderEngine* re);

#endif
