/*
    Copyright (c) 2026 - Yann BOYER
*/
#include "emu.h"
#include "consts.h"
#include "cpu.h"
#include "render_engine.h"
#include <stdio.h>
#include <stdlib.h>

void emu_init(Emulator* emu) {
    audiopl_init(&emu->audiopl);
    mem_init(&emu->mem);
    cpu_init(&emu->cpu);
    re_init(&emu->re);
}

void emu_deinit(Emulator* emu) {
    audiopl_deinit(&emu->audiopl);
    cpu_deinit(&emu->cpu);
}

void emu_load_rom_from_file(Emulator* emu, char* rom_path) {
    FILE* rom_file = fopen(rom_path, "rb");

    if (rom_file == NULL) {
        fprintf(stderr, "[FATAL ERROR] Unable to open the ROM file !\n");
        emu_deinit(emu);
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    fseek(rom_file, 0, SEEK_END);
    unsigned long rom_buf_size = ftell(rom_file);
    rewind(rom_file);

    if (rom_buf_size > MAX_ROM_SIZE) {
        fclose(rom_file);
        fprintf(stderr, "[FATAL ERROR] Your ROM exceeds the max rom size !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    // Load FONTSET before anything else.
    for (uint8_t i = 0; i < FONTSET_SIZE; i++) {
        mem_write(&emu->mem, i, FONTSET[i]);
    }

    uint8_t* rom_buf = (uint8_t*) malloc(rom_buf_size * sizeof(uint8_t));
    if (rom_buf == NULL) {
        fclose(rom_file);
        fprintf(stderr, "[FATAL ERROR] Memory Allocation Error !\n");
        emu_deinit(emu);
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    fread(rom_buf, rom_buf_size, 1, rom_file);
    fclose(rom_file);

    for (size_t idx = 0; idx < rom_buf_size; idx++) {
        mem_write(&emu->mem, idx + CPU_INTERNAL_PROGRAM_COUNTER_START, rom_buf[idx]);
    }

    free(rom_buf);
}

bool emu_re_is_pixel_on(Emulator* emu, uint8_t x, uint8_t y) {
    return re_is_pixel_on(&emu->re, x, y);
}

void emu_update_cpu_timers(Emulator* emu) {
    cpu_update_timers(&emu->cpu, &emu->audiopl);
}

void emu_do_cpu_cycle(Emulator* emu) {
    cpu_decode_and_execute(&emu->cpu, &emu->mem, &emu->re);
}
