/*
    Copyright (c) 2026 - Yann BOYER
*/
#include "render_engine.h"
#include "consts.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void re_init(RenderEngine* re) {
    for (size_t i = 0; i < RENDER_TABLE_SIZE; i++) {
        re->render_table[i] = PIXEL_OFF;
    }
}

bool re_is_pixel_on(RenderEngine* re, uint8_t x, uint8_t y) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        fprintf(stderr, "[FATAL ERROR] Invalid coordinates !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    return re->render_table[y * CHIP8_SCREEN_WIDTH + x] == PIXEL_ON;
}

void re_change_pixel_state_to(RenderEngine* re, uint8_t x, uint8_t y, PixelState new_state) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        fprintf(stderr, "[FATAL ERROR] Invalid coordinates !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    re->render_table[y * CHIP8_SCREEN_WIDTH + x] = new_state;
}

void re_clear(RenderEngine* re) {
    for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            re_change_pixel_state_to(re, x, y, PIXEL_OFF);
        }
    }
}
