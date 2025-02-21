//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef TERMFUNCS_H_INCLUDED
#define TERMFUNCS_H_INCLUDED

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>
#include <stdbool.h>

struct timeval;

#define COLORX_DARKGREY       (COLOR_BLACK + 8)
#define COLORX_BRIGHTBLUE     (COLOR_BLUE + 8)
#define COLORX_BRIGHTGREEN    (COLOR_GREEN + 8)
#define COLORX_BRIGHTCYAN     (COLOR_CYAN + 8)
#define COLORX_BRIGHTRED      (COLOR_RED + 8)
#define COLORX_BRIGHTMAGENTA  (COLOR_MAGENTA + 8)
#define COLORX_BRIGHTYELLOW   (COLOR_YELLOW + 8)
#define COLORX_BRIGHTWHITE    (COLOR_WHITE + 8)

struct palette {
	size_t num_colors;
	struct { int c, r, g, b; } colors[16];
};

struct saved_flags {
	int fcntl_opts;
	struct termios termios;
};

void TF_SetCursesModes(void);
void TF_SuspendCursesMode(void);

void TF_SavePalette(struct palette *p);
void TF_SetPalette(struct palette *p);
void TF_SetColorPairs(void);
void TF_SetNewPalette(void);
void TF_RestoreOldPalette(void);

void TF_SetRawMode(struct saved_flags *f, bool blocking);
void TF_RestoreNormalMode(struct saved_flags *f);
int TF_PollingReadChar(struct timeval *start);

void TF_ClearScreen(void);
void TF_SendRaiseWindowOp(void);

#endif /* #ifndef TERMFUNCS_H_INCLUDED */
