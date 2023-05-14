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

#ifndef HOST_H
#define HOST_H

#include "quakedef.h"

void Host_ClearMemory(void);
void Host_ServerFrame(void);
void Host_Init(quakeparms_t *parms);
void Host_Shutdown(void);
void Host_Error(char *error, ...);
void Host_EndGame(char *message, ...);
void Host_Frame(float time);
void Host_ClientCommands(char *fmt, ...);
void Host_ShutdownServer(qboolean crash);

void SV_BroadcastPrintf(char *fmt, ...);
void SV_ClientPrintf(char *fmt, ...);
void SV_DropClient(qboolean crash);

#endif // !HOST_H
