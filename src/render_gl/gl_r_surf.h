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

#ifndef RENDER_GL_RSURF_H
#define RENDER_GL_RSURF_H

#include "../render_common/common_render.h"
#include "gl_model.h"

void GL_BuildLightmaps(void);
void GL_DisableMultitexture(void);
void GL_EnableMultitexture(void);
void R_DrawBrushModel(entity_t *e);
void R_DrawWaterSurfaces(void);
void R_DrawWorld(void);
void R_RenderBrushPoly(msurface_t *fa);
texture_t *R_TextureAnimation(texture_t *base);

#endif // !RENDER_GL_RSURF_H
