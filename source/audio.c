/*
    Copyright (c) 2026 - Yann BOYER
*/
#include "audio.h"
#include <stdio.h>

void audiopl_init(AudioPlayer* audiopl) {
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, MIX_DEFAULT_CHANNELS, 2048) < 0) {
        fprintf(stderr, "[FATAL ERROR] Unable to initialize the audio backend !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    audiopl->beep_sound = Mix_LoadWAV("beep_sound.wav");

    if (audiopl->beep_sound == NULL) {
        fprintf(stderr, "[FATAL ERROR] Unable to load beep_sound.wav !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }

    Mix_VolumeChunk(audiopl->beep_sound, MIX_MAX_VOLUME / 2);
}

void audiopl_deinit(AudioPlayer* audiopl) {
    Mix_FreeChunk(audiopl->beep_sound);
}

void audiopl_play_beep_sound(AudioPlayer* audiopl) {
    int check = Mix_PlayChannel(-1, audiopl->beep_sound, 0);
    if (check < 0) {
        fprintf(stderr, "[FATAL ERROR] Unable to play beep_sound.wav !\n");
        exit(EXIT_FAILURE); // Ugly, don't care.
    }
}
