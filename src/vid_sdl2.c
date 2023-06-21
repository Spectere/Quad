/*
 * Copyright (C) 2023 Ian Burgmyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// vid_sdl2.c -- SDL video component

#include <SDL.h>

#ifdef RENDER_GL
#include <glad/glad.h>
#endif // RENDER_GL

#include "quakedef.h"

#include "cl_input.h"
#include "host_cmd.h"

// Input
qboolean ignore_mouse_input = false;
qboolean mouse_captured = false;
cvar_t in_mouse = { "in_mouse", "1", true };
cvar_t in_mlook_lock = { "in_mlook_lock", "1", true };

// A jumble of globals, locals, defines, and cvars, copied from gl_vidnt.c.
// TODO: A lot has changed in the past few decades. Let's see how many of these we can axe.
#define WARP_WIDTH		320
#define WARP_HEIGHT		200
const char *gl_vendor;
const char *gl_renderer;
const char *gl_version;
unsigned short d_8to16table[256];
unsigned int d_8to24table[256];
unsigned char d_15to8table[65536];
qboolean scr_skipupdate;
float gldepthmin, gldepthmax;
int texture_extension_number = 1;
cvar_t gl_ztrick = {"gl_ztrick", "1"};
cvar_t _windowed_mouse = {"_windowed_mouse","1", true};

#ifdef RENDER_GL
int texture_mode;
#else
#include "render_soft/soft_d_local.h"

byte *vid_buffer = NULL;
SDL_Renderer *sdl_renderer = NULL;
SDL_Texture *sdl_texture = NULL;

static byte *vid_surfcache;
static int vid_surfcachesize;
static int VID_highhunkmark;

qboolean VID_AllocBuffers(int width, int height);
#endif // RENDER_GL


// TODO: Temporary stuff that should eventually be deleted.
const int VID_WIDTH = 1280;
const int VID_HEIGHT = 960;

// Newer stuff that probably shouldn't be deleted. :)
SDL_Window* sdl_window = NULL;
SDL_GLContext sdl_gl_context = NULL;
float mouse_x = 0;
float mouse_y = 0;

/*
 * General video functions.
 */
void VID_SetPalette(unsigned char *palette) {
    byte *pal;
    unsigned int r, g, b;
    unsigned int v;
    int r1, g1, b1;
    int j, k, l;
    unsigned short i;
    unsigned int *table;

//
// 8 8 8 encoding
//
    pal = palette;
    table = d_8to24table;
    for (i = 0; i < 256; i++) {
        r = pal[0];
        g = pal[1];
        b = pal[2];
        pal += 3;

//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
        v = (255 << 24) + (r << 0) + (g << 8) + (b << 16);
        *table++ = v;
    }
    d_8to24table[255] &= 0xffffff;    // 255 is transparent

    // JACK: 3D distance calcs - k is last closest, l is the distance.
    // FIXME: Precalculate this and cache to disk.
    for (i = 0; i < (1 << 15); i++) {
        /* Maps
            000000000000000
            000000000011111 = Red  = 0x1F
            000001111100000 = Blue = 0x03E0
            111110000000000 = Grn  = 0x7C00
        */
        r = ((i & 0x1F) << 3) + 4;
        g = ((i & 0x03E0) >> 2) + 4;
        b = ((i & 0x7C00) >> 7) + 4;
        pal = (unsigned char *) d_8to24table;
        for (v = 0, k = 0, l = 10000 * 10000; v < 256; v++, pal += 4) {
            r1 = r - pal[0];
            g1 = g - pal[1];
            b1 = b - pal[2];
            j = (r1 * r1) + (g1 * g1) + (b1 * b1);
            if (j < l) {
                k = v;
                l = j;
            }
        }
        d_15to8table[i] = k;
    }
}

void VID_ShiftPalette(unsigned char *palette) {
    VID_SetPalette(palette);
}

void VID_Init(unsigned char *palette) {
    Cvar_RegisterVariable(&in_mouse);
    Cvar_RegisterVariable(&in_mlook_lock);

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Sys_Error("Unable to initialize SDL2.\n");
    }

#ifdef RENDER_GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    sdl_window = SDL_CreateWindow(
            "QuadGL",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            VID_WIDTH, VID_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if(sdl_window == NULL) {
        Sys_Error("Unable to initialize SDL2 window.\n");
    }

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if(sdl_gl_context == NULL) {
        Sys_Error("Unable to initialize GL context.\n");
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        Sys_Error("Unable to initialize GLAD.\n");
    }

    VID_SetPalette(palette);

    gl_vendor = (char*)glGetString(GL_VENDOR);
    Con_Printf("GL_VENDOR: %s\n", gl_vendor);
    gl_renderer = (char*)glGetString(GL_RENDERER);
    Con_Printf("GL_RENDERER: %s\n", gl_renderer);
    gl_version = (char*)glGetString(GL_VERSION);
    Con_Printf("GL_VERSION: %s\n", gl_version);

    GLint gl_mtex_units;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &gl_mtex_units);
    Con_Printf("GL_MAX_TEXTURE_UNITS: %i\n", gl_mtex_units);

    if(gl_mtex_units < 2) {
        VID_Shutdown();
        Sys_Error("QuadGL requires a graphics card with multitexturing support.\n");
    }

    glClearColor(1,0,0,0);
    glCullFace(GL_FRONT);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.666f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_FLAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#else
    sdl_window = SDL_CreateWindow(
            "Quad",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            VID_WIDTH, VID_HEIGHT,
            SDL_WINDOW_SHOWN
    );
    if(sdl_window == NULL) {
        Sys_Error("Unable to initialize SDL2 window.\n");
    }

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if(sdl_renderer == NULL) {
        Sys_Printf("Unable to initialize SDL2 hardware renderer. Attempting software fallback...");

        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);
        if(sdl_renderer == NULL) {
            Sys_Error("Unable to initialize SDL2 software renderer.\n");
        }
    }

    sdl_texture = SDL_CreateTexture(
        sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
        VID_WIDTH, VID_HEIGHT
    );
    if(sdl_texture == NULL) {
        Sys_Error("Unable to create SDL2 texture.\n");
    }
#endif // RENDER_GL

    IN_MouseCapture(in_mouse.value > 0.0f);

    // Populate the vid object.
    vid.width = VID_WIDTH;
    vid.height = VID_HEIGHT;
    vid.numpages = 2;
    vid.conwidth = VID_WIDTH;
    vid.conheight = VID_HEIGHT;
    vid.conwidth &= 0xfff8; // make it a multiple of eight

    // pick a conheight that matches with correct aspect
    vid.conheight = vid.conwidth*3 / 4;

    vid.maxwarpwidth = WARP_WIDTH;
    vid.maxwarpheight = WARP_HEIGHT;
    vid.colormap = host_colormap;
    vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));

#ifdef RENDER_SOFT
    // TODO: Make this a bit less hardcoded. :)
    vid_buffer = malloc(VID_WIDTH * VID_HEIGHT);
    vid.buffer = vid.conbuffer = vid.direct = vid_buffer;
    vid.rowbytes = vid.conrowbytes = VID_WIDTH;
    vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);

    VID_AllocBuffers(VID_WIDTH, VID_HEIGHT);
    D_InitCaches(vid_surfcache, vid_surfcachesize);
#endif // RENDER_SOFT
}

void VID_Shutdown(void) {
#ifdef RENDER_SOFT
    if(sdl_texture) {
        SDL_DestroyTexture(sdl_texture);
    }

    if(sdl_renderer) {
        SDL_DestroyRenderer(sdl_renderer);
    }

    if(vid_buffer) {
        free(vid_buffer);
    }
#endif // RENDER_SOFT

    IN_MouseCapture(false);

    if(sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


/*
 * Input-related functions.
 */
void Sys_SendKeyEvents(void) {}

void IN_Init(void) {
    SDL_Init(SDL_INIT_EVENTS);
}

void IN_Shutdown(void) {
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

void IN_Commands(void) {}

void IN_Move(usercmd_t *cmd) {
    float mouse_var;

    mouse_var = mouse_captured ? 1.0f : 0.0f;
    if(mouse_var != in_mouse.value) {
        IN_MouseCapture(in_mouse.value > 0.0f);
    }

    if(!mouse_captured || ignore_mouse_input) {
        return;
    }

    mouse_x *= sensitivity.value;
    mouse_y *= sensitivity.value;

    if((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1))) {
        cmd->sidemove += m_side.value * mouse_x;
    } else {
        cl.viewangles[YAW] -= m_yaw.value * mouse_x;
    }

    if(in_mlook.state & 1 || in_mlook_lock.value) {
        V_StopPitchDrift();
    }

    if((in_mlook.state & 1 || in_mlook_lock.value) && !(in_strafe.state & 1)) {
        cl.viewangles[PITCH] += m_pitch.value * mouse_y;
        if(cl.viewangles[PITCH] > 80) {
            cl.viewangles[PITCH] = 80;
        }
        if(cl.viewangles[PITCH] < -70) {
            cl.viewangles[PITCH] = -70;
        }
    } else {
        if((in_strafe.state & 1) && noclip_anglehack) {
            cmd->upmove -= m_forward.value * mouse_y;
        } else {
            cmd->forwardmove -= m_forward.value * mouse_y;
        }
    }

    mouse_x = mouse_y = 0.0f;
}

void IN_MouseCapture(qboolean captured) {
    mouse_captured = captured;
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

int IN_TranslateKeycode(SDL_Keycode keycode) {
    int candidate;

    switch(keycode) {
        // Navigational keys.
        case SDLK_UP: candidate = K_UPARROW; break;
        case SDLK_DOWN: candidate = K_DOWNARROW; break;
        case SDLK_LEFT: candidate = K_LEFTARROW; break;
        case SDLK_RIGHT: candidate = K_RIGHTARROW; break;
        case SDLK_PAGEUP: candidate = K_PGUP; break;
        case SDLK_PAGEDOWN: candidate = K_PGDN; break;
        case SDLK_HOME: candidate = K_HOME; break;
        case SDLK_END: candidate = K_END; break;

        // Function keys.
        case SDLK_F1: candidate = K_F1; break;
        case SDLK_F2: candidate = K_F2; break;
        case SDLK_F3: candidate = K_F3; break;
        case SDLK_F4: candidate = K_F4; break;
        case SDLK_F5: candidate = K_F5; break;
        case SDLK_F6: candidate = K_F6; break;
        case SDLK_F7: candidate = K_F7; break;
        case SDLK_F8: candidate = K_F8; break;
        case SDLK_F9: candidate = K_F9; break;
        case SDLK_F10: candidate = K_F10; break;
        case SDLK_F11: candidate = K_F11; break;
        case SDLK_F12: candidate = K_F12; break;

        // Modifier keys.
        case SDLK_LCTRL: case SDLK_RCTRL: candidate = K_CTRL; break;
        case SDLK_LALT: case SDLK_RALT: candidate = K_ALT; break;
        case SDLK_LSHIFT: case SDLK_RSHIFT: candidate = K_SHIFT; break;

        // Miscellaneous keys.
        case SDLK_INSERT: candidate = K_INS; break;
        case SDLK_DELETE: candidate = K_DEL; break;
        case SDLK_BACKSPACE: candidate = K_BACKSPACE; break;
        case SDLK_PAUSE: candidate = K_PAUSE; break;

        // Everything else!
        default: candidate = (int)keycode; break;
    }

    if(candidate > 255) {
        return 0;
    }

    return candidate;
}

int IN_TranslateMouseButton(SDL_MouseButtonEvent button) {
    int candidate;

    switch(button.button) {
        case SDL_BUTTON_LEFT: candidate = K_MOUSE1; break;
        case SDL_BUTTON_RIGHT: candidate = K_MOUSE2; break;
        case SDL_BUTTON_MIDDLE: candidate = K_MOUSE3; break;

        default: candidate = 0; break;
    }

    return candidate;
}


/*
 * Common windowing system functions.
 */
void WND_ProcessEvents(void) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                Key_Event(IN_TranslateKeycode(event.key.keysym.sym), true);
                break;

            case SDL_KEYUP:
                Key_Event(IN_TranslateKeycode(event.key.keysym.sym), false);
                break;

            case SDL_MOUSEMOTION:
                mouse_x = (float)event.motion.xrel;
                mouse_y = (float)event.motion.yrel;
                break;

            case SDL_MOUSEBUTTONDOWN:
                Key_Event(IN_TranslateMouseButton(event.button), true);
                break;

            case SDL_MOUSEBUTTONUP:
                Key_Event(IN_TranslateMouseButton(event.button), false);
                break;

            case SDL_QUIT:
                Host_Quit_f();
                break;
        }
    }
}


/*
 * Software-specific functions.
 */
#ifdef RENDER_SOFT
qboolean VID_AllocBuffers(int width, int height) {
    int		tsize, tbuffersize;

    tbuffersize = width * height * sizeof (*d_pzbuffer);

    tsize = D_SurfaceCacheForRes (width, height);

    tbuffersize += tsize;

// see if there's enough memory, allowing for the normal mode 0x13 pixel,
// z, and surface buffers
    if ((host_parms.memsize - tbuffersize + SURFCACHE_SIZE_AT_320X200 +
         0x10000 * 3) < minimum_memory)
    {
        Con_SafePrintf ("Not enough memory for video mode\n");
        return false;		// not enough memory for mode
    }

    vid_surfcachesize = tsize;

    if (d_pzbuffer)
    {
        D_FlushCaches ();
        Hunk_FreeToHighMark (VID_highhunkmark);
        d_pzbuffer = NULL;
    }

    VID_highhunkmark = Hunk_HighMark ();

    d_pzbuffer = Hunk_HighAllocName (tbuffersize, "video");

    vid_surfcache = (byte *)d_pzbuffer +
                    width * height * sizeof (*d_pzbuffer);

    return true;
}

void VID_Update(vrect_t *rects) {
    if(sdl_texture == NULL) {
        return;
    }

    unsigned int *pixels = NULL;
    int *pitch = NULL;

    SDL_LockTexture(sdl_texture, NULL, (void**)&pixels, (int*)&pitch);

    for(int i = 0; i < VID_WIDTH * VID_HEIGHT; i++) {
        byte b = vid_buffer[i];
        pixels[i] = d_8to24table[b];
    }
    SDL_UnlockTexture(sdl_texture);

    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);

    WND_ProcessEvents();
}
#endif // RENDER_SOFT


/*
 * GL-specific functions.
 */
#ifdef RENDER_GL
void GL_BeginRendering(int *x, int *y, int *width, int *height) {
    *x = *y = 0;
    *width = VID_WIDTH;
    *height = VID_HEIGHT;
}

void GL_EndRendering(void) {
    SDL_GL_SwapWindow(sdl_window);
    WND_ProcessEvents();
}
#endif // RENDER_GL
