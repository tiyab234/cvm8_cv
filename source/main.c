/*
    Copyright (c) 2026 - Yann BOYER
*/
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "emu.h"
#include "consts.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "[FATAL ERROR] No ROM provided !\n");
        fprintf(stdout, "[INFO] Usage : ./cvm8_cv my_rom.rom/my_rom.ch8\n");
        return EXIT_FAILURE;
    }

    char* rom_path = argv[1];

    Emulator chip8_emu;
    emu_init(&chip8_emu);

    emu_load_rom_from_file(&chip8_emu, rom_path);

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("CVM8_CV by Yann BOYER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        emu_deinit(&chip8_emu);
        fprintf(stderr, "[FATAL ERROR] Unable to create the window !\n");
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL) {
        emu_deinit(&chip8_emu);
        SDL_DestroyWindow(window); // Destroy the created window.
        fprintf(stderr, "[FATAL ERROR] Unable to create the renderer !\n");
        SDL_Quit();
        return EXIT_FAILURE;
    }

    uint8_t div_cycles = 0;
    bool is_running = true;

    while (is_running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    break;
                default: break;
            }
        }

        emu_do_cpu_cycle(&chip8_emu);
        div_cycles++;

        // Drawing.
        for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
            for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
                SDL_Rect pixel = { x * PIXEL_SCALE_FACTOR, y * PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR };
                if (emu_re_is_pixel_on(&chip8_emu, x, y))
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer); // Prevent slowdowns...

        if (div_cycles == TIMER_CLOCK_DIVISION) {
            emu_update_cpu_timers(&chip8_emu);
            div_cycles = 0;
        }

        SDL_Delay(CPU_CLOCK_DELAY);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    emu_deinit(&chip8_emu);
    SDL_Quit();

    return EXIT_SUCCESS;
}
