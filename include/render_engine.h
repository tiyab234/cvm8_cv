/*
    Copyright (c) 2026 - Yann BOYER
*/
#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <stdbool.h>
#include <stdint.h>
#include "consts.h"

typedef enum {
    PIXEL_ON = 1,
    PIXEL_OFF = 0,
} PixelState;

#define RENDER_TABLE_SIZE (CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT)

typedef struct {
    PixelState render_table[RENDER_TABLE_SIZE];
} RenderEngine;

// No deinit needed, no dynamic alloc.
void re_init(RenderEngine* re);
bool re_is_pixel_on(RenderEngine* re, uint8_t x, uint8_t y);
void re_change_pixel_state_to(RenderEngine* re, uint8_t x, uint8_t y, PixelState new_state);
void re_clear(RenderEngine* re);

#endif
