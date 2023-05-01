/*
Copyright (C) 2023 Ian Burgmyer

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// snd_sdl2.c -- SDL2 audio handler

#include <SDL.h>

#include "../quakedef.h"

qboolean initialized = false;
SDL_AudioDeviceID audio_device = 0;

int audio_buffer_len;

void S_Callback(void *userdata, Uint8 *stream, int len) {
    int pos, remaining;

    pos = shm->samplepos * (shm->samplebits / 8);
    if(pos >= audio_buffer_len) {
        shm->samplepos = pos = 0;
    }

    remaining = audio_buffer_len - pos;

    if(len > remaining) {
        // Copy the end of the buffer.
        memcpy(stream, shm->buffer + pos, remaining);

        // Copy the remainder of the data from the start of the buffer.
        int new_remaining = len - remaining;
        memcpy(stream + remaining, shm->buffer, new_remaining);

        shm->samplepos = new_remaining / (shm->samplebits / 8);
    } else {
        memcpy(stream, shm->buffer + pos, len);
        shm->samplepos = (pos + len) / (shm->samplebits / 8);
    }
}

qboolean SNDDMA_Init(void) {
    SDL_AudioSpec want, have;

    if(SDL_Init(SDL_INIT_AUDIO)) {
        Con_Printf("Error initializing SDL2 audio.\n");
        return false;
    }

    initialized = true;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.channels = 2;
    want.samples = 2048;
    want.callback = S_Callback;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 1);

    // Configure Quake's sound system.
    memset((void *)&sn, 0, sizeof(sn));
    shm = &sn;

    shm->channels = have.channels;
    shm->samplebits = have.format & 0xFF;
    shm->speed = have.freq;
    shm->soundalive = true;
    shm->samples = have.samples * have.channels * 8;
    shm->samplepos = 0;
    shm->submission_chunk = 1;

    audio_buffer_len = shm->samples * (shm->samplebits / 8);
    shm->buffer = (unsigned char *)calloc(1, audio_buffer_len);
    if(shm->buffer == NULL) {
        Con_Printf("Error allocating audio buffer.\n");
        initialized = false;
        shm = NULL;
        SDL_CloseAudioDevice(audio_device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    Con_Printf("SDL2 audio initialized (%i channels, %i-bit, %i hz)\n", have.channels, shm->samplebits, have.freq);
    SDL_PauseAudioDevice(audio_device, 0);

    return true;
}

void SNDDMA_Shutdown(void) {
    if(!initialized) {
        return;
    }

    Con_Printf("Shutting down SDL2 audio.\n");

    if(audio_device > 0) {
        SDL_CloseAudioDevice(audio_device);
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    if(shm != NULL && shm->buffer != NULL) {
        free(shm->buffer);
    }
}

int SNDDMA_GetDMAPos(void) {
    return shm->samplepos;
}

void SNDDMA_Submit(void) {
}
