/*
Copyright (C) 2023 Ian Burgmyer

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
// sys_sdl2.h -- Generic SDL2 system interface header

#ifndef SYS_SDL2_H
#define SYS_SDL2_H

#include <SDL.h>
#include <stdio.h>
#include "quakedef.h"

#define MAX_OSPATH 128

extern qboolean isDedicated;

#endif // SYS_SDL2_H
