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

#ifndef PR_EDICT_H
#define PR_EDICT_H

#include <stdio.h>

#include "progs.h"

edict_t *ED_Alloc(void);
void ED_Free(edict_t *ed);
void ED_LoadFromFile(char *data);
int ED_NewString(char *string);  // Returns an index of the new string in the non-progs string list.
char *ED_ParseEdict(char *data, edict_t *ent);
void ED_ParseGlobals(char *data);
void ED_Print(edict_t *ed);
void ED_PrintEdicts(void);
void ED_PrintNum(int ent);
void ED_Write(FILE *f, edict_t *ed);
void ED_WriteGlobals(FILE *f);

eval_t *GetEdictFieldValue(edict_t *ed, char *field);

char* PR_GetString(int offset);
void PR_Init(void);
void PR_LoadProgs(void);
int PR_NewNonProgsString(char *string);

#endif // !PR_EDICT_H
