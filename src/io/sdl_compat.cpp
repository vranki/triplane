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

#include <SDL3/SDL.h>
#include <mikmod.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../io/dksfile.h"
#include "../io/timing.h"
#include "sdl_compat.h"
#include "../util/wutil.h"

/* ---------------------------------------------------------------- input */

static void handle_common_event(const SDL_Event &e) {
  if (e.type == SDL_EVENT_QUIT) {
    exit(0);
  }
}

void pump_events() {
  SDL_PumpEvents();
  keys_refresh();
}

int kbhit() {
  SDL_Event e;

  io::timing::nopeuskontrolli();

  SDL_PumpEvents();
  int ret = SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST);
  if (ret > 0) {
    if (e.type == SDL_EVENT_KEY_UP) {
      keys_refresh();
      return 1;
    } else {
      SDL_PollEvent(&e);
      handle_common_event(e);
    }
  }
  keys_refresh();
  return 0;
}

int getch() {
  SDL_Event e;

  for (;;) {
    if (SDL_WaitEventTimeout(&e, 10)) {
      handle_common_event(e);
      if (e.type == SDL_EVENT_KEY_UP) {
        keys_refresh();
        int s = key_from_sdl(e.key.key);
        SDL_Keymod m = e.key.mod;
        if (s == TK_RSHIFT || s == TK_LSHIFT || s == TK_UNKNOWN) {
          continue;
        }
        if (m & SDL_KMOD_SHIFT) {
          if (s >= TK_a && s <= TK_z) {
            s = toupper(s);
          }
        }
        return s;
      }
    }
  }
}

void update_key_state() {
  SDL_Event e;
  SDL_PumpEvents();
  while (SDL_PollEvent(&e)) {
    handle_common_event(e);
  }
  keys_refresh();
}

/* ---------------------------------------------------------------- audio */

constexpr int MIXER_RATE = 44100;
constexpr int MIXER_CHANNELS = 16; /* max. simultaneous samples */

namespace {

struct mixer_channel {
  const sb_sample *sample;
  int pos;
  float left, right;
  bool looping;
};

struct mixer_state {
  SDL_AudioStream *stream = nullptr;
  mixer_channel channels[MIXER_CHANNELS] = {};
  bool music_playing = false;  /* a module is started in the MikMod player */
  float *mixbuf = nullptr;
  int16_t *musicbuf = nullptr;
  int bufframes = 0;
};

mixer_state mixer;

class audio_lock {
public:
  audio_lock() {
    if (mixer.stream)
      SDL_LockAudioStream(mixer.stream);
  }
  ~audio_lock() {
    if (mixer.stream)
      SDL_UnlockAudioStream(mixer.stream);
  }
};

/* Runs on the audio thread with the stream locked. */
void SDLCALL mixer_callback(void *, SDL_AudioStream *stream,
                            int additional_amount, int) {
  int frames_left = additional_amount / (int)(2 * sizeof(float));

  while (frames_left > 0) {
    int frames = std::min(frames_left, mixer.bufframes);
    float *out = mixer.mixbuf;
    memset(out, 0, frames * 2 * sizeof(float));

    if (mixer.music_playing) {
      if (Player_Active()) {
        ULONG bytes = frames * 2 * sizeof(int16_t);
        ULONG got = VC_WriteBytes((SBYTE *)mixer.musicbuf, bytes);
        int n = (int)(got / sizeof(int16_t));
        for (int i = 0; i < n; i++)
          out[i] = mixer.musicbuf[i] * (1.0f / 32768.0f);
      } else {
        /* the module has been played once, like Mix_PlayMusic(m, 1) */
        Player_Stop();
        mixer.music_playing = false;
      }
    }

    for (auto &ch : mixer.channels) {
      if (!ch.sample)
        continue;
      const float *pcm = ch.sample->pcm;
      for (int i = 0; i < frames; i++) {
        if (ch.pos >= ch.sample->frames) {
          if (!ch.looping || ch.sample->frames == 0) {
            ch.sample = nullptr;
            break;
          }
          ch.pos = 0;
        }
        float v = pcm[ch.pos++];
        out[2 * i] += v * ch.left;
        out[2 * i + 1] += v * ch.right;
      }
    }

    for (int i = 0; i < frames * 2; i++)
      out[i] = std::clamp(out[i], -1.0f, 1.0f);

    SDL_PutAudioStreamData(stream, out, frames * 2 * (int)sizeof(float));
    frames_left -= frames;
  }
}

bool mikmod_ready = false;

/* exit() handler. SDL_Quit is registered earlier and so runs later; stop the
 * audio stream (and its thread) before MikMod is torn down. */
void audio_atexit() {
  if (mixer.stream)
    sdl_uninit_sounds();
  MikMod_Exit();
}

/* MikMod renders the music into memory; it is set up once per process. */
bool ensure_mikmod() {
  if (mikmod_ready)
    return true;
  MikMod_RegisterDriver(&drv_nos);
  MikMod_RegisterLoader(&load_mod);
  md_mixfreq = MIXER_RATE;
  md_mode = DMODE_16BITS | DMODE_STEREO | DMODE_SOFT_MUSIC | DMODE_HQMIXER |
            DMODE_INTERP;
  md_musicvolume = 128;
  md_pansep = 128;
  md_reverb = 0;
  if (MikMod_Init("")) {
    fprintf(stderr, "MikMod_Init: %s\n", MikMod_strerror(MikMod_errno));
    return false;
  }
  atexit(audio_atexit);
  mikmod_ready = true;
  return true;
}

void stop_music_locked() {
  if (mixer.music_playing) {
    Player_Stop();
    mixer.music_playing = false;
  }
}

} // namespace

/**
 * Initialize SDL sounds so that sdl_play_sample can be called.
 * @return 0 on success, nonzero otherwise.
 */
int sdl_init_sounds() {
  if (!ensure_mikmod())
    return 1;
  if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    return 1;

  SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, MIXER_RATE};
  mixer.bufframes = 1024;
  mixer.mixbuf = (float *)util::wutil::walloc(mixer.bufframes * 2 * sizeof(float));
  mixer.musicbuf =
      (int16_t *)util::wutil::walloc(mixer.bufframes * 2 * sizeof(int16_t));

  mixer.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                           &spec, mixer_callback, nullptr);
  if (!mixer.stream) {
    fprintf(stderr, "SDL_OpenAudioDeviceStream: %s\n", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    return 1;
  }
  SDL_ResumeAudioStreamDevice(mixer.stream);
  return 0;
}

/** Deinitalize SDL sounds */
void sdl_uninit_sounds() {
  if (mixer.stream) {
    SDL_DestroyAudioStream(mixer.stream);
    mixer.stream = nullptr;
  }
  stop_music_locked();
  for (auto &ch : mixer.channels)
    ch.sample = nullptr;
  util::wutil::wfree(mixer.mixbuf);
  util::wutil::wfree(mixer.musicbuf);
  mixer.mixbuf = nullptr;
  mixer.musicbuf = nullptr;
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/**
 * Play sample.
 * @param sample sample loaded with sdl_load_sample.
 */
void sdl_play_sample(sb_sample *sample, int looping) {
  if (!mixer.stream || !sample)
    return;

  /* scale of left_volume and right_volume is [0..32] */
  int l = sample->left_volume * 8, r = sample->right_volume * 8;
  if (l > 0)
    l--;
  if (r > 0)
    r--;

  audio_lock lock;
  for (auto &ch : mixer.channels) {
    if (ch.sample)
      continue;
    ch.sample = sample;
    ch.pos = 0;
    ch.left = l / 255.0f;
    ch.right = r / 255.0f;
    ch.looping = looping != 0;
    return;
  }
  /* no free channels, this sound won't be played */
}

/** Stop playing all samples started with sdl_play_sample */
void sdl_stop_all_samples() {
  audio_lock lock;
  for (auto &ch : mixer.channels)
    ch.sample = nullptr;
}

/* Read a whole entry of fokker.dks into a malloc'd buffer. */
static uint8_t *read_dks_entry(const char *name, int *len) {
  if (dksopen(name) != 1)
    return nullptr;

  *len = dkssize();
  uint8_t *p = (uint8_t *)util::wutil::walloc(*len);
  if (dksread(p, *len) != 1) {
    fprintf(stderr, "Cannot read %s from the data file\n", name);
    exit(1);
  }
  dksclose();
  return p;
}

/**
 * Load sample with given name.
 * @param name audio name used inside fokker.dks
 * @return loaded sample or nullptr on error.
 */
sb_sample *sdl_sample_load(const char *name) {
  int len;
  uint8_t *p = read_dks_entry(name, &len);
  if (!p)
    return nullptr;

  SDL_AudioSpec wavspec;
  Uint8 *wav = nullptr;
  Uint32 wavlen = 0;
  if (!SDL_LoadWAV_IO(SDL_IOFromConstMem(p, len), true, &wavspec, &wav,
                      &wavlen)) {
    fprintf(stderr, "sdl_sample_load %s: %s\n", name, SDL_GetError());
    exit(1);
  }
  free(p);

  SDL_AudioSpec dst = {SDL_AUDIO_F32, 1, MIXER_RATE};
  Uint8 *pcm = nullptr;
  int pcmlen = 0;
  if (!SDL_ConvertAudioSamples(&wavspec, wav, (int)wavlen, &dst, &pcm,
                               &pcmlen)) {
    fprintf(stderr, "sdl_sample_load %s: %s\n", name, SDL_GetError());
    exit(1);
  }
  SDL_free(wav);

  sb_sample *sample = (sb_sample *)util::wutil::walloc(sizeof(sb_sample));
  sample->left_volume = sample->right_volume = SAMPLE_VOLUME;
  sample->pcm = (float *)pcm;
  sample->frames = pcmlen / (int)sizeof(float);
  return sample;
}

/**
 * Free sample loaded using sdl_sample_load.
 * @param sample to be free'd.
 */
void sdl_free_sample(sb_sample *sample) {
  if (!sample)
    return;
  {
    audio_lock lock;
    for (auto &ch : mixer.channels)
      if (ch.sample == sample)
        ch.sample = nullptr;
  }
  SDL_free(sample->pcm);
  free(sample);
}

sb_mod_file *sdl_load_mod_file(const char *name) {
  if (!ensure_mikmod())
    return nullptr;

  int len;
  uint8_t *p = read_dks_entry(name, &len);
  if (!p)
    return nullptr;

  MODULE *module;
  {
    audio_lock lock; /* MikMod is not thread safe against the player */
    module = Player_LoadMem((const char *)p, len, 64, 0);
  }
  if (!module) {
    fprintf(stderr, "sdl_load_mod_file %s: %s\n", name,
            MikMod_strerror(MikMod_errno));
    exit(1);
  }
  free(p);

  /* same player settings as SDL_mixer, play the song once */
  module->extspd = 1;
  module->panflag = 1;
  module->wrap = 0;
  module->loop = 0;

  sb_mod_file *mod = (sb_mod_file *)util::wutil::walloc(sizeof(sb_mod_file));
  mod->module = module;
  return mod;
}

void sdl_free_mod_file(sb_mod_file *mod) {
  if (!mod)
    return;
  {
    audio_lock lock;
    if (mixer.music_playing && Player_GetModule() == mod->module)
      stop_music_locked();
    Player_Free((MODULE *)mod->module);
  }
  free(mod);
}

void sdl_play_music(sb_mod_file *mod) {
  sdl_stop_all_samples();
  if (!mixer.stream || !mod)
    return;

  audio_lock lock;
  stop_music_locked();
  MODULE *module = (MODULE *)mod->module;
  Player_Start(module);
  Player_SetPosition(0); /* restart from the beginning if played before */
  mixer.music_playing = true;
}

void sdl_stop_music() {
  audio_lock lock;
  stop_music_locked();
}
