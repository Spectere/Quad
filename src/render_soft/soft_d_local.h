/*
Copyright (C) 1996-1997 Id Software, Inc.

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
// d_local.h:  private rasterization driver defs

#ifndef RENDER_SOFT_D_LOCAL
#define RENDER_SOFT_D_LOCAL

#include "soft_r_shared.h"

#define R_SKY_SMASK    0x007F0000
#define R_SKY_TMASK    0x007F0000

#define DS_SPAN_LIST_END    (-128)

#define SURFCACHE_SIZE_AT_320X200   (600*1024)

typedef struct surfcache_s {
    struct surfcache_s *next;
    struct surfcache_s **owner;        // NULL is an empty chunk of memory
    int lightadj[MAXLIGHTMAPS]; // checked for strobe flush
    int dlight;
    int size;        // including header
    unsigned width;
    unsigned height;        // DEBUG only needed for debug
    float mipscale;
    struct texture_s *texture;    // checked for animating textures
    byte data[4];    // width*height elements
} surfcache_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct sspan_s {
    int u, v, count;
} sspan_t;

extern cvar_t d_subdiv16;

extern float scale_for_mip;

extern qboolean d_roverwrapped;
extern surfcache_t *sc_rover;
extern surfcache_t *d_initial_rover;

extern float d_sdivzstepu, d_tdivzstepu, d_zistepu;
extern float d_sdivzstepv, d_tdivzstepv, d_zistepv;
extern float d_sdivzorigin, d_tdivzorigin, d_ziorigin;

extern fixed16_t sadjust, tadjust;
extern fixed16_t bbextents, bbextentt;

extern short *d_pzbuffer;
extern unsigned int d_zrowbytes, d_zwidth;

extern int d_scantable[MAXHEIGHT];

extern int d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

extern int d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

extern pixel_t *d_viewbuffer;

extern short *zspantable[MAXHEIGHT];

extern int d_minmip;
extern float d_scalemip[3];

#endif // !RENDER_SOFT_D_LOCAL
