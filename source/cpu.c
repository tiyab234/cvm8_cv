/*
    Copyright (c) 2026 - Yann BOYER
*/
#include "cpu.h"
#include "audio.h"
#include "consts.h"
#include "render_engine.h"
#include <stddef.h>
#include <stdlib.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

void cpu_init(CPU* cpu) {
    // Since KEYS_COUNT has the same value as
    // REGS_COUNT, only one for loop !
    for (size_t i = 0; i < REGS_COUNT; i++) {
        cpu->v_regs[i] = 0;
        cpu->keys[i] = KEY_NOT_PRESSED;
    }

    cpu->stack = NULL;
    cpu->delay_tm = 0;
    cpu->sound_tm = 0;
    cpu->pc = CPU_INTERNAL_PROGRAM_COUNTER_START;
}

void cpu_deinit(CPU* cpu) {
    arrfree(cpu->stack);
}

void cpu_update_timers(CPU* cpu, AudioPlayer* audiopl) {
    if (cpu->delay_tm > 0) cpu->delay_tm--;
    if (cpu->sound_tm > 0) {
        cpu->sound_tm--;

        if (cpu->sound_tm == 1) audiopl_play_beep_sound(audiopl);
    }
}

uint16_t cpu_fetch_next_op(CPU* cpu, Memory* mem) {
    uint16_t msb = mem_read(mem, cpu->pc);
    uint16_t lsb = mem_read(mem, cpu->pc + 1);

    return msb << 8 | lsb;
}

void cpu_decode_and_execute(CPU* cpu, Memory* mem, RenderEngine* re) {
    uint16_t f_op = cpu_fetch_next_op(cpu, mem);

    // NOTE : Performance penalty of recomputing these
    // every cycle doesn't seem to be high, but might still
    // be worth looking at it in the future if I feel the need
    // to.
    uint16_t nnn = f_op & 0x0FFF;
    uint8_t nn = f_op & 0x00FF;
    uint8_t n = f_op & 0x000F;
    uint8_t x = (f_op & 0x0F00) >> 8;
    uint8_t y = (f_op & 0x00F0) >> 4;

    switch (f_op & 0xF000) {
        case 0x0000:
            switch (f_op & 0x00FF) {
                case 0x00E0:
                    // CLS
                    re_clear(re);
                    cpu->pc += 2;
                    break;
                case 0x00EE:
                    // RET
                    cpu->pc = arrpop(cpu->stack);
                    cpu->pc += 2;
                    break;
                default:
                    fprintf(stderr, "[FATAL ERROR] Unknown opcode -> 0x%04x\n", f_op);
                    exit(EXIT_FAILURE); // Ugly, don't care.
                    break;
            }
            break;
        case 0x1000:
            // JP addr
            cpu->pc = nnn;
            break;
        case 0x2000:
            arrpush(cpu->stack, cpu->pc);
            cpu->pc = nnn;
            break;
        case 0x3000:
            // SE Vx, byte
            cpu->pc += cpu->v_regs[x] == nn ? 4 : 2;
            break;
        case 0x4000:
            // SNE Vx, byte
            cpu->pc += cpu->v_regs[x] != nn ? 4 : 2;
            break;
        case 0x5000:
            // SE Vx, Vy
            cpu->pc += cpu->v_regs[x] == cpu->v_regs[y] ? 4 : 2;
            break;
        case 0x6000:
            // LD Vx, byte
            cpu->v_regs[x] = nn;
            cpu->pc += 2;
            break;
        case 0x7000:
            // ADD Vx, byte
            cpu->v_regs[x] += nn;
            cpu->pc += 2;
            break;
        case 0x8000:
            switch (f_op & 0x000F) {
                case 0x0000:
                    // LD Vx, Vy
                    cpu->v_regs[x] = cpu->v_regs[y];
                    cpu->pc += 2;
                    break;
                case 0x0001:
                    // OR Vx, Vy
                    cpu->v_regs[x] |= cpu->v_regs[y];
                    cpu->pc += 2;
                    break;
                case 0x0002:
                    // AND Vx, Vy
                    cpu->v_regs[x] &= cpu->v_regs[y];
                    cpu->pc += 2;
                    break;
                case 0x0003:
                    // XOR Vx, Vy
                    cpu->v_regs[x] ^= cpu->v_regs[y];
                    cpu->pc += 2;
                    break;
                case 0x0004:
                    // ADD Vx, Vy
                    {
                        uint16_t r = cpu->v_regs[x] + cpu->v_regs[y];

                        cpu->v_regs[0xF] = r > 0xFF ? 1 : 0;

                        cpu->v_regs[x] = (uint8_t)(r & 0xFF);

                        cpu->pc += 2;
                    }
                    break;
                case 0x0005:
                    // SUB Vx, Vy
                    cpu->v_regs[0xF] = cpu->v_regs[x] > cpu->v_regs[y] ? 1 : 0;
                    cpu->v_regs[x] -= cpu->v_regs[y];
                    cpu->pc += 2;
                    break;
                case 0x0006:
                    // SHR Vx {, Vy}
                    cpu->v_regs[0xF] = cpu->v_regs[x] & 0x1;
                    cpu->v_regs[x] >>= 1;
                    cpu->pc += 2;
                    break;
                case 0x0007:
                    // SUBN Vx, Vy
                    cpu->v_regs[0xF] = cpu->v_regs[y] > cpu->v_regs[x] ? 1 : 0;
                    cpu->v_regs[x] = cpu->v_regs[y] - cpu->v_regs[x];
                    cpu->pc += 2;
                    break;
                case 0x000E:
                    // SHL Vx [, Vy]
                    cpu->v_regs[0xF] = (cpu->v_regs[x] & 128) >> 7;
                    cpu->v_regs[x] <<= 1;
                    cpu->pc += 2;
                    break;
                default:
                    fprintf(stderr, "[FATAL ERROR] Unknown opcode -> 0x%04x\n", f_op);
                    exit(EXIT_FAILURE); // Ugly, don't care.
                    break;
            }
            break;
        case 0x9000:
            // SNE Vx, Vy
            cpu->pc += cpu->v_regs[x] != cpu->v_regs[y] ? 4 : 2;
            break;
        case 0xA000:
            // LD I, addr
            cpu->index_reg = nnn;
            cpu->pc += 2;
            break;
        case 0xB000:
            // JP V0, addr
            cpu->pc = nnn + cpu->v_regs[0x0];
            break;
        case 0xC000:
            // RND Vx, byte
            {
                srand(time(NULL));
                uint8_t min = 0;
                uint8_t max = 255;
                uint8_t rand_num = rand() % (max - min + 1);

                cpu->v_regs[x] = rand_num & nn;
                cpu->pc += 2;
            }
            break;
        case 0xD000:
            // DRW Vx, Vy, nibble
            {
                uint8_t x_orig = cpu->v_regs[x];
                uint8_t y_orig = cpu->v_regs[y];

                cpu->v_regs[0xF] = 0;
                for (uint8_t y_coord = 0; y_coord < n; y_coord++) {
                    uint8_t pixel = mem_read(mem, y_coord + cpu->index_reg);
                    for (uint8_t x_coord = 0; x_coord < 8; x_coord++) {
                        if ((pixel & (0x80 >> x_coord)) != 0) {
                            uint8_t x_pixel = (x_orig + x_coord) % CHIP8_SCREEN_WIDTH;
                            uint8_t y_pixel = (y_orig + y_coord) % CHIP8_SCREEN_HEIGHT;


                            // XOR technique, ON -> OFF & Collision, OFF -> ON.
                            if (re_is_pixel_on(re, x_pixel, y_pixel)) {
                                re_change_pixel_state_to(re, x_pixel, y_pixel, PIXEL_OFF);
                                cpu->v_regs[0xF] = 1; // Collision.
                            } else {
                                re_change_pixel_state_to(re, x_pixel, y_pixel, PIXEL_ON);
                            }
                        }
                    }
                }

                cpu->pc += 2;
            }
            break;
        case 0xE000:
            switch (f_op & 0x00FF) {
                case 0x009E:
                    // SKP Vx
                    cpu->pc += cpu->keys[cpu->v_regs[x]] == KEY_PRESSED ? 4 : 2;
                    break;
                case 0x00A1:
                    // SKNP Vx
                    cpu->pc += cpu->keys[cpu->v_regs[x]] == KEY_NOT_PRESSED ? 4 : 2;
                    break;
                default:
                    fprintf(stderr, "[FATAL ERROR] Unknown opcode -> 0x%04x\n", f_op);
                    exit(EXIT_FAILURE); // Ugly, don't care.
                    break;
            }
            break;
        case 0xF000:
            switch (f_op & 0x00FF) {
                case 0x0007:
                    // LD Vx, DT
                    cpu->v_regs[x] = cpu->delay_tm;
                    cpu->pc += 2;
                    break;
                case 0x000A:
                    // LD Vx, K
                    fprintf(stderr, "[FATAL ERROR] LD Vx, K not implemented !\n");
                    exit(EXIT_FAILURE);
                    break;
                case 0x0015:
                    // LD DT, Vx
                    cpu->delay_tm = cpu->v_regs[x];
                    cpu->pc += 2;
                    break;
                case 0x0018:
                    // LD ST, Vx
                    cpu->sound_tm = cpu->v_regs[x];
                    cpu->pc += 2;
                    break;
                case 0x001E:
                    // ADD I, Vx
                    cpu->index_reg += cpu->v_regs[x];
                    cpu->pc += 2;
                    break;
                case 0x0029:
                    // LD F, Vx
                    cpu->index_reg = cpu->v_regs[x] * 5;
                    cpu->pc += 2;
                    break;
                case 0x0033:
                    // LD B, Vx
                    {
                        uint8_t reg_val = cpu->v_regs[x];

                        mem_write(mem, cpu->index_reg, reg_val / 100); // Hundreds.
                        mem_write(mem, cpu->index_reg + 1, (reg_val % 100) / 10); // Tens.
                        mem_write(mem, cpu->index_reg + 2, reg_val % 10); // Units.

                        cpu->pc += 2;
                    }
                    break;
                case 0x0055:
                    // LD [I], Vx
                    for (uint8_t i = 0; i < x + 1; i++) {
                        mem_write(mem, cpu->index_reg + i, cpu->v_regs[i]);
                    }

                    cpu->pc += 2;
                    break;
                case 0x0065:
                    // LD Vx, [I]
                    for (uint8_t i = 0; i < x + 1; i++) {
                        cpu->v_regs[i] = mem_read(mem, cpu->index_reg + i);
                    }

                    cpu->pc += 2;
                    break;
                default:
                    fprintf(stderr, "[FATAL ERROR] Unknown opcode -> 0x%04x\n", f_op);
                    exit(EXIT_FAILURE); // Ugly, don't care.
                    break;
            }
            break;
        default:
            fprintf(stderr, "[FATAL ERROR] Unknown opcode -> 0x%04x\n", f_op);
            exit(EXIT_FAILURE); // Ugly, don't care.
            break;

    }
}
