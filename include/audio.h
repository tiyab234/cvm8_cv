/*
    Copyright (c) 2026 - Yann BOYER
*/
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>

typedef struct {
    Mix_Chunk* beep_sound;
} AudioPlayer;

void audiopl_init(AudioPlayer* audiopl);
void audiopl_deinit(AudioPlayer* audiopl);
void audiopl_play_beep_sound(AudioPlayer* audiopl);

#endif
