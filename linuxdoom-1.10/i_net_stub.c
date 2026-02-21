// Stub networking for Emscripten/mobile port of DOOM
// Sets up single-player mode only — no networking.

#include <stdlib.h>
#include <string.h>

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"
#include "doomstat.h"
#include "i_net.h"


void I_InitNetwork(void)
{
    doomcom = malloc(sizeof(*doomcom));
    memset(doomcom, 0, sizeof(*doomcom));

    // Single player setup
    netgame             = false;
    doomcom->id         = DOOMCOM_ID;
    doomcom->numplayers = 1;
    doomcom->numnodes   = 1;
    doomcom->deathmatch = false;
    doomcom->consoleplayer = 0;
    doomcom->ticdup      = 1;
    doomcom->extratics   = 0;
}

void I_NetCmd(void)
{
}
