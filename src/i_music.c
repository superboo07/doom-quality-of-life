/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 by id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2020 by Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is a part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries, and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#include <string.h>

#include "SDL_mixer.h"

#include "c_console.h"
#include "i_midirpc.h"
#include "m_config.h"
#include "mmus2mid.h"
#include "s_sound.h"

dboolean        midimusictype;
dboolean        musmusictype;

static dboolean music_initialized;

static int      current_music_volume;
static int      paused_midi_volume;

#if defined(_WIN32)
static dboolean midirpc;
dboolean        serverMidiPlaying;
#endif

// Shutdown music
void I_ShutdownMusic(void)
{
    if (!music_initialized)
        return;

    Mix_FadeOutMusic(500);
    while (Mix_PlayingMusic());
    music_initialized = false;

    if (mus_playing)
        I_UnRegisterSong(mus_playing->handle);

    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

#if defined(_WIN32)
    I_MidiRPCClientShutDown();
#endif
}

// Initialize music subsystem
dboolean I_InitMusic(void)
{
    int         freq;
    int         channels;
    uint16_t    format;

    // If SDL_mixer is not initialized, we have to initialize it
    // and have the responsibility to shut it down later on.
    if (!Mix_QuerySpec(&freq, &format, &channels))
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
            return false;

        if (Mix_OpenAudio(SAMPLERATE, MIX_DEFAULT_FORMAT, CHANNELS, CHUNKSIZE) < 0)
        {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            return false;
        }
    }

    SDL_PauseAudio(0);

    music_initialized = true;

#if defined(_WIN32)
    // Initialize RPC server
    if (I_MidiRPCInitServer())
        midirpc = I_MidiRPCInitClient();
#endif

    return music_initialized;
}

// Set music volume (0 - MAX_MUSIC_VOLUME)
void I_SetMusicVolume(int volume)
{
    // Internal state variable.
    current_music_volume = volume;

#if defined(_WIN32)
    // adjust server volume
    if (serverMidiPlaying)
    {
        I_MidiRPCSetVolume(current_music_volume);
        return;
    }
#endif

    Mix_VolumeMusic(current_music_volume);
}

// Start playing a mid
void I_PlaySong(void *handle, dboolean looping)
{
    if (!music_initialized)
        return;

#if defined(_WIN32)
    if (serverMidiPlaying)
    {
        I_MidiRPCPlaySong(looping);
        I_MidiRPCSetVolume(current_music_volume);
        return;
    }
#endif

    if (handle)
        Mix_PlayMusic(handle, (looping ? -1 : 1));
}

void I_PauseSong(void)
{
    if (!music_initialized)
        return;

#if defined(_WIN32)
    if (serverMidiPlaying)
    {
        I_MidiRPCPauseSong();
        return;
    }
#endif

    if (!midimusictype)
        Mix_PauseMusic();
    else
    {
        paused_midi_volume = Mix_VolumeMusic(-1);
        Mix_VolumeMusic(0);
    }
}

void I_ResumeSong(void)
{
    if (!music_initialized)
        return;

#if defined(_WIN32)
    if (serverMidiPlaying)
    {
        I_MidiRPCResumeSong();
        return;
    }
#endif

    if (!midimusictype)
        Mix_ResumeMusic();
    else
        Mix_VolumeMusic(paused_midi_volume);
}

void I_StopSong(void)
{
    if (!music_initialized)
        return;

#if defined(_WIN32)
    if (serverMidiPlaying)
    {
        I_MidiRPCStopSong();
        serverMidiPlaying = false;
        return;
    }
#endif

    Mix_HaltMusic();
}

void I_UnRegisterSong(void *handle)
{
    if (!music_initialized)
        return;

#if defined(_WIN32)
    if (serverMidiPlaying)
    {
        I_MidiRPCStopSong();
        serverMidiPlaying = false;
        return;
    }
#endif

    if (handle)
        Mix_FreeMusic(handle);
}

void *I_RegisterSong(void *data, int size)
{
    if (!music_initialized)
        return NULL;
    else
    {
        Mix_Music   *music = NULL;
        SDL_RWops   *rwops = NULL;

        midimusictype = false;
        musmusictype = false;

        // Check for MIDI or MUS format first:
        if (size >= 14)
        {
            if (!memcmp(data, "MThd", 4))                       // is it a MIDI?
                midimusictype = true;
            else if (mmuscheckformat((uint8_t *)data, size))    // is it a MUS?
            {
                MIDI    mididata;
                uint8_t *mid;
                int     midlen;

                musmusictype = true;

                memset(&mididata, 0, sizeof(MIDI));

                if (!mmus2mid((uint8_t *)data, (size_t)size, &mididata))
                    return NULL;

                // Hurrah! Let's make it a mid and give it to SDL_mixer
                MIDIToMidi(&mididata, &mid, &midlen);

                FreeMIDIData(&mididata);

                data = mid;
                size = midlen;
                midimusictype = true;                           // now it's a MIDI
            }
        }

#if defined(_WIN32)
        // Check for option to invoke RPC server if is MIDI
        if (midimusictype && midirpc)
            if (I_MidiRPCRegisterSong(data, size))
            {
                serverMidiPlaying = true;
                return NULL;        // server will play this song
            }
#endif

        if ((rwops = SDL_RWFromMem(data, size)))
            music = Mix_LoadMUS_RW(rwops, SDL_FALSE);

        return music;
    }
}
