// Stub sound implementation for Emscripten/mobile port of DOOM
// All sound functions are no-ops. Sound can be added later via Web Audio API.

#include <stdio.h>
#include <stdlib.h>

#include "z_zone.h"
#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "doomdef.h"

// SNDSERV globals required by i_sound.h when SNDSERV is defined
#ifdef SNDSERV
FILE*  sndserver = 0;
char*  sndserver_filename = "./sndserver ";
#endif


void I_InitSound(void)
{
    // No sound hardware to initialize on web
}

void I_UpdateSound(void)
{
}

void I_SubmitSound(void)
{
}

void I_ShutdownSound(void)
{
}

void I_SetChannels(void)
{
}

void I_SetSfxVolume(int volume)
{
    snd_SfxVolume = volume;
}

int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

int I_StartSound(int id, int vol, int sep, int pitch, int priority)
{
    return id;
}

void I_StopSound(int handle)
{
}

int I_SoundIsPlaying(int handle)
{
    return 0;
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
}


// Music API — all stubs
void I_InitMusic(void)          { }
void I_ShutdownMusic(void)      { }
void I_SetMusicVolume(int vol)  { snd_MusicVolume = vol; }
void I_PauseSong(int handle)    { }
void I_ResumeSong(int handle)   { }
int  I_RegisterSong(void* data) { return 1; }
void I_PlaySong(int handle, int looping) { }
void I_StopSong(int handle)     { }
void I_UnRegisterSong(int handle) { }
int  I_QrySongPlaying(int handle) { return 0; }
