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

#ifndef CL_INPUT_H
#define CL_INPUT_H

#include "cl_main.h"

typedef struct {
    int down[2];        // key nums holding it down
    int state;            // low bit is down state
} kbutton_t;

extern kbutton_t in_mlook, in_klook;
extern kbutton_t in_strafe;
extern kbutton_t in_speed;

void CL_BaseMove(usercmd_t *cmd);
float CL_KeyState(kbutton_t *key);
void CL_InitInput(void);
void CL_SendMove(usercmd_t *cmd);

#endif // !CL_INPUT_H
