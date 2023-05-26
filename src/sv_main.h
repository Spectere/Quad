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

#ifndef SV_MAIN_H
#define SV_MAIN_H

#include "common.h"
#include "mathlib.h"
#include "progs.h"

void SV_CheckForNewClients(void);
void SV_ClearDatagram(void);
void SV_Init(void);
int SV_ModelIndex(char *name);
void SV_SaveSpawnparms();
void SV_SendClientMessages(void);
void SV_SpawnServer(char *server);
void SV_StartParticle(vec3_t org, vec3_t dir, int color, int count);
void SV_StartSound(edict_t *entity, int channel, char *sample, int sys_volume, float attenuation);
void SV_WriteClientdataToMessage(edict_t *ent, sizebuf_t *msg);

#endif // !SV_MAIN_H
