/* 
 * Triplane Classic - a side-scrolling dogfighting game.
 * Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * tjt@users.sourceforge.net
 */

#include "io/sound.h"
#include "triplane.h"
#include <stdio.h>
#include <stdlib.h>

int is_there_sound = 1;
int reverbed_channels = 0;
int soundcard_type = SOUNDCARD_NONE;

int play_sample(sb_sample * sample_ptr) {
    if (is_there_sound) {

        sample_ptr->right_volume = SAMPLE_VOLUME;
        sample_ptr->left_volume = SAMPLE_VOLUME;

        sdl_play_sample(sample_ptr);
    }
    return 0;
}

int play_2d_sample(sb_sample * sample_ptr, int player, int target) {
    int balance = 7;

    if (!playing_solo) {
        play_sample(sample_ptr);

        return 0;

    }

    if (is_there_sound) {
        balance = target - player;
        if (reverbed_channels)
            balance = -balance;

        balance = balance >> 4;

        if (balance > 0) {
            if (balance > SAMPLE_VOLUME)
                balance = SAMPLE_VOLUME;

            sample_ptr->right_volume = SAMPLE_VOLUME;
            sample_ptr->left_volume = SAMPLE_VOLUME - balance;

        } else {
            balance = -balance;
            if (balance > SAMPLE_VOLUME)
                balance = SAMPLE_VOLUME;

            sample_ptr->right_volume = SAMPLE_VOLUME - balance;
            sample_ptr->left_volume = SAMPLE_VOLUME;


        }
        sdl_play_sample(sample_ptr);
    }

    return 0;
}

void init_sounds(void) {
    soundcard_type = SOUNDCARD_NONE;

    if (sdl_init_sounds() != 0) {
        is_there_sound = 0;
        printf("SDL sound init failed. Switching sounds off.\n");

    } else {
        is_there_sound = 1;
        soundcard_type = SOUNDCARD_SDL;
    }

    return;
}

void uninit_sounds(void) {
    is_there_sound = 0;
    sdl_uninit_sounds();
}

sb_sample *sample_load(const char *samplename) {
    sb_sample *sample;

    sample = sdl_sample_load(samplename);

    if (sample == NULL) {
        printf("Cannon't locate data %s\n", samplename);
        exit(1);
    }
    return sample;
}
