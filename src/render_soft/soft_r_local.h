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
// r_local.h -- private refresh defs

#ifndef RENDER_SOFT_R_LOCAL_H
#define RENDER_SOFT_R_LOCAL_H

#include "soft_r_shared.h"

#define ALIAS_BASE_SIZE_RATIO        (1.0 / 11.0)   // normalizing factor so player model works out to about
                                                    //  1 pixel per triangle

#define BMODEL_FULLY_CLIPPED    0x10   // value returned by R_BmodelCheckBBox ()
                                       //  if bbox is trivially rejected

//===========================================================================
// viewmodel lighting

typedef struct {
    int ambientlight;
    int shadelight;
    float *plightvec;
} alight_t;

//===========================================================================
// clipped bmodel edges

typedef struct bedge_s {
    mvertex_t *v[2];
    struct bedge_s *pnext;
} bedge_t;

typedef struct {
    float fv[3];        // viewspace x, y
} auxvert_t;

//===========================================================================

extern cvar_t r_draworder;
extern cvar_t r_speeds;
extern cvar_t r_timegraph;
extern cvar_t r_graphheight;
extern cvar_t r_clearcolor;
extern cvar_t r_waterwarp;
extern cvar_t r_fullbright;
extern cvar_t r_drawentities;
extern cvar_t r_aliasstats;
extern cvar_t r_dspeeds;
extern cvar_t r_drawflat;
extern cvar_t r_ambient;
extern cvar_t r_reportsurfout;
extern cvar_t r_maxsurfs;
extern cvar_t r_numsurfs;
extern cvar_t r_reportedgeout;
extern cvar_t r_maxedges;
extern cvar_t r_numedges;

#define XCENTERING    (1.0 / 2.0)
#define YCENTERING    (1.0 / 2.0)

#define BACKFACE_EPSILON    0.01

//===========================================================================

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct clipplane_s {
    vec3_t normal;
    float dist;
    struct clipplane_s *next;
    byte leftedge;
    byte rightedge;
    byte reserved[2];
} clipplane_t;

extern clipplane_t view_clipplanes[4];

//=============================================================================

extern mplane_t screenedge[4];

extern vec3_t r_origin;

extern vec3_t r_entorigin;

extern float screenAspect;
extern float verticalFieldOfView;
extern float xOrigin, yOrigin;

extern int r_visframecount;

//=============================================================================

extern int vstartscan;

//
// current entity info
//
extern qboolean insubmodel;
extern vec3_t r_worldmodelorg;

extern int c_faceclip;
extern int r_polycount;
extern int r_wholepolycount;

extern int *pfrustum_indexes[4];

// !!! if this is changed, it must be changed in asm_draw.h too !!!
#define    NEAR_CLIP    0.01

extern int ubasestep, errorterm, erroradjustup, erroradjustdown;

extern fixed16_t sadjust, tadjust;
extern fixed16_t bbextents, bbextentt;

extern float entity_rotation[3][3];

extern int r_currentkey;
extern int r_currentbkey;

typedef struct btofpoly_s {
    int clipflags;
    msurface_t *psurf;
} btofpoly_t;

#define MAX_BTOFPOLYS    5000    // FIXME: tune this

extern int numbtofpolys;
extern btofpoly_t *pbtofpolys;

//=========================================================
// Alias models
//=========================================================

#define MAXALIASVERTS        2000    // TODO: tune this
#define ALIAS_Z_CLIP_PLANE    5

extern int numverts;
extern int a_skinwidth;
extern aliashdr_t *paliashdr;
extern mdl_t *pmdl;
extern finalvert_t *pfinalverts;
extern auxvert_t *pauxverts;

//=========================================================
// turbulence stuff

#define AMP     (8 * 0x10000)
#define AMP2    3
#define SPEED   20

//=========================================================

extern int r_amodels_drawn;
extern edge_t *auxedges;
extern int r_numallocatededges;
extern edge_t *r_edges, *edge_p, *edge_max;

extern edge_t *newedges[MAXHEIGHT];
extern edge_t *removeedges[MAXHEIGHT];

extern int screenwidth;

// FIXME: make stack vars when debugging done
extern edge_t edge_head;
extern edge_t edge_tail;
extern edge_t edge_aftertail;
extern int r_bmodelactive;
extern vrect_t *pconupdate;

extern float aliasxscale, aliasyscale, aliasxcenter, aliasycenter;
extern float r_aliastransition, r_resfudge;

extern int r_outofsurfaces;
extern int r_outofedges;

extern mvertex_t *r_pcurrentvertbase;

extern float r_time1;
extern float dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
extern float se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;
extern int r_frustum_indexes[4 * 6];
extern int r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
extern qboolean r_surfsonstack;
extern cshift_t cshift_water;
extern qboolean r_dowarpold, r_viewchanged;

extern mleaf_t *r_viewleaf, *r_oldviewleaf;

extern vec3_t r_emins, r_emaxs;
extern mnode_t *r_pefragtopnode;
extern int r_clipflags;
extern int r_dlightframecount;

#endif // !RENDER_SOFT_R_LOCAL_H
