// SDL2-based video and input for Emscripten/mobile port of DOOM
// Replaces the X11 i_video.c for web/mobile targets

#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "doomdef.h"
#include "d_event.h"

static SDL_Window*   sdl_window   = NULL;
static SDL_Renderer* sdl_renderer = NULL;
static SDL_Texture*  sdl_texture  = NULL;

static Uint32 pixel_palette[256];
static Uint32* framebuffer = NULL;


static int xlatekey(SDL_Keycode sym)
{
    switch (sym)
    {
      case SDLK_LEFT:      return KEY_LEFTARROW;
      case SDLK_RIGHT:     return KEY_RIGHTARROW;
      case SDLK_DOWN:      return KEY_DOWNARROW;
      case SDLK_UP:        return KEY_UPARROW;
      case SDLK_ESCAPE:    return KEY_ESCAPE;
      case SDLK_RETURN:    return KEY_ENTER;
      case SDLK_TAB:       return KEY_TAB;
      case SDLK_F1:        return KEY_F1;
      case SDLK_F2:        return KEY_F2;
      case SDLK_F3:        return KEY_F3;
      case SDLK_F4:        return KEY_F4;
      case SDLK_F5:        return KEY_F5;
      case SDLK_F6:        return KEY_F6;
      case SDLK_F7:        return KEY_F7;
      case SDLK_F8:        return KEY_F8;
      case SDLK_F9:        return KEY_F9;
      case SDLK_F10:       return KEY_F10;
      case SDLK_F11:       return KEY_F11;
      case SDLK_F12:       return KEY_F12;
      case SDLK_BACKSPACE: return KEY_BACKSPACE;
      case SDLK_DELETE:    return KEY_BACKSPACE;
      case SDLK_PAUSE:     return KEY_PAUSE;
      case SDLK_EQUALS:    return KEY_EQUALS;
      case SDLK_KP_EQUALS: return KEY_EQUALS;
      case SDLK_MINUS:     return KEY_MINUS;
      case SDLK_KP_MINUS:  return KEY_MINUS;
      case SDLK_RSHIFT:
      case SDLK_LSHIFT:    return KEY_RSHIFT;
      case SDLK_RCTRL:
      case SDLK_LCTRL:     return KEY_RCTRL;
      case SDLK_RALT:
      case SDLK_LALT:
      case SDLK_RGUI:
      case SDLK_LGUI:      return KEY_RALT;
      default:
        if (sym >= SDLK_SPACE && sym <= SDLK_z)
            return (int)sym;
        break;
    }
    return 0;
}


void I_ShutdownGraphics(void)
{
    if (sdl_texture)  SDL_DestroyTexture(sdl_texture);
    if (sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
    if (sdl_window)   SDL_DestroyWindow(sdl_window);
    if (framebuffer)  free(framebuffer);
    sdl_texture  = NULL;
    sdl_renderer = NULL;
    sdl_window   = NULL;
    framebuffer  = NULL;
}


void I_StartFrame(void)
{
}


void I_StartTic(void)
{
    event_t event;
    SDL_Event sdlevent;

    while (SDL_PollEvent(&sdlevent))
    {
        switch (sdlevent.type)
        {
          case SDL_KEYDOWN:
            event.type = ev_keydown;
            event.data1 = xlatekey(sdlevent.key.keysym.sym);
            event.data2 = event.data3 = 0;
            if (event.data1) D_PostEvent(&event);
            break;

          case SDL_KEYUP:
            event.type = ev_keyup;
            event.data1 = xlatekey(sdlevent.key.keysym.sym);
            event.data2 = event.data3 = 0;
            if (event.data1) D_PostEvent(&event);
            break;

          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
          {
            int state = SDL_GetMouseState(NULL, NULL);
            event.type = ev_mouse;
            event.data1 = 0;
            if (state & SDL_BUTTON(SDL_BUTTON_LEFT))   event.data1 |= 1;
            if (state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) event.data1 |= 2;
            if (state & SDL_BUTTON(SDL_BUTTON_RIGHT))  event.data1 |= 4;
            event.data2 = event.data3 = 0;
            D_PostEvent(&event);
            break;
          }

          case SDL_MOUSEMOTION:
            event.type = ev_mouse;
            event.data1 = 0;
            {
                int state = SDL_GetMouseState(NULL, NULL);
                if (state & SDL_BUTTON(SDL_BUTTON_LEFT))   event.data1 |= 1;
                if (state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) event.data1 |= 2;
                if (state & SDL_BUTTON(SDL_BUTTON_RIGHT))  event.data1 |= 4;
            }
            event.data2 = sdlevent.motion.xrel << 2;
            event.data3 = -sdlevent.motion.yrel << 2;
            D_PostEvent(&event);
            break;

          case SDL_QUIT:
            I_Quit();
            break;
        }
    }
}


void I_UpdateNoBlit(void)
{
}


void I_FinishUpdate(void)
{
    int i;
    byte* src = screens[0];

    for (i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++)
        framebuffer[i] = pixel_palette[src[i]];

    SDL_UpdateTexture(sdl_texture, NULL, framebuffer,
                      SCREENWIDTH * sizeof(Uint32));
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
}


void I_ReadScreen(byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}


void I_SetPalette(byte* palette)
{
    int i;
    for (i = 0; i < 256; i++)
    {
        unsigned char r, g, b;
        r = gammatable[usegamma][*palette++];
        g = gammatable[usegamma][*palette++];
        b = gammatable[usegamma][*palette++];
        pixel_palette[i] = (0xFFu << 24) | ((Uint32)r << 16)
                         | ((Uint32)g << 8) | (Uint32)b;
    }
}


void I_InitGraphics(void)
{
    static int firsttime = 1;
    if (!firsttime) return;
    firsttime = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        I_Error("Could not initialize SDL2: %s", SDL_GetError());

    sdl_window = SDL_CreateWindow(
        "DOOM",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREENWIDTH * 2, SCREENHEIGHT * 2,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!sdl_window)
        I_Error("Could not create SDL window: %s", SDL_GetError());

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer)
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
    if (!sdl_renderer)
        I_Error("Could not create SDL renderer: %s", SDL_GetError());

    SDL_RenderSetLogicalSize(sdl_renderer, SCREENWIDTH, SCREENHEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    sdl_texture = SDL_CreateTexture(sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREENWIDTH, SCREENHEIGHT);
    if (!sdl_texture)
        I_Error("Could not create SDL texture: %s", SDL_GetError());

    framebuffer = (Uint32*)malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(Uint32));
    // Note: screens[0] is already allocated by V_Init() — do NOT re-allocate here
}


//
// Touch input support for mobile — callable from JavaScript
//
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void doom_key_down(int key)
{
    event_t event;
    event.type = ev_keydown;
    event.data1 = key;
    event.data2 = 0;
    event.data3 = 0;
    D_PostEvent(&event);
}

EMSCRIPTEN_KEEPALIVE
void doom_key_up(int key)
{
    event_t event;
    event.type = ev_keyup;
    event.data1 = key;
    event.data2 = 0;
    event.data3 = 0;
    D_PostEvent(&event);
}
#endif
