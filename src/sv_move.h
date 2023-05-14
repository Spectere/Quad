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

#ifndef SV_MOVE_H
#define SV_MOVE_H

#include "quakedef.h"

qboolean SV_CheckBottom(edict_t *ent);
qboolean SV_movestep(edict_t *ent, vec3_t move, qboolean relink);
void SV_MoveToGoal(void);
void SV_RunClients(void);

#endif // !SV_MOVE_H
