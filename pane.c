//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pane.h"

#define MAX_SCREEN_PANES 10

static struct pane *screen_panes[MAX_SCREEN_PANES];
static unsigned int num_screen_panes = 0;
static int main_loop_exited = 0;

void UI_PaneKeypress(void *pane, int key)
{
	struct pane *p = pane;

	if (p->keypress != NULL) {
		p->keypress(p, key);
	}
}

static int GetPaneIndex(void *pane)
{
	unsigned int i;

	for (i = 0; i < num_screen_panes; i++) {
		if (screen_panes[i] == pane) {
			return i;
		}
	}

	return -1;
}

void UI_PaneShow(void *pane)
{
	// In case already shown, remove first; we will add it back
	// at the top of the stack.
	UI_PaneHide(pane);

	assert(num_screen_panes < MAX_SCREEN_PANES);

	screen_panes[num_screen_panes] = pane;
	++num_screen_panes;
}

int UI_PaneHide(void *pane)
{
	int idx = GetPaneIndex(pane);

	if (idx < 0) {
		return 0;
	}

	memmove(&screen_panes[idx], &screen_panes[idx + 1],
	        (num_screen_panes - idx - 1) * sizeof(struct pane *));
	--num_screen_panes;
	return 1;
}

void UI_DrawAllPanes(void)
{
	static WINDOW *fullscr_win = NULL;
	int i;

	// We maintain a background full-screen window that we just erase
	// entirely before we draw the others. This ensures that any "crud"
	// left over after a window is closed will get erased.
	if (fullscr_win == NULL) {
		fullscr_win = newwin(0, 0, 0, 0);
	}
	werase(fullscr_win);
	wnoutrefresh(fullscr_win);

	for (i = 0; i < num_screen_panes; i++) {
		struct pane *p = screen_panes[i];
		if (p->draw != NULL) {
			p->draw(p);
			wnoutrefresh(p->window);
		}
	}
	doupdate();
}

void UI_RaisePaneToTop(void *pane)
{
	if (UI_PaneHide(pane)) {
		UI_PaneShow(pane);
	}
}

static bool HandleKeypress(void)
{
	int key, i;

	// TODO: This should handle multiple keypresses before returning.
	key = getch();
	if (key == ERR) {
		return false;
	}

	// Keypress goes to the top pane that has a keypress handler.
	for (i = num_screen_panes - 1; i >= 0; i--) {
		if (screen_panes[i]->keypress != NULL) {
			UI_PaneKeypress(screen_panes[i], key);
			break;
		}
	}

	return true;
}

static void HandleKeypresses(void)
{
	// Block on the first keypress.
	nodelay(stdscr, 0);
	HandleKeypress();

	// We now need to do at least one screen update. But read any
	// additional keypresses first.
	nodelay(stdscr, 1);
	while (HandleKeypress()) {
	}
}

void UI_RunMainLoop(void)
{
	while (!main_loop_exited) {
		UI_DrawAllPanes();
		HandleKeypresses();
	}

	main_loop_exited = 0;
}

void UI_ExitMainLoop(void)
{
	main_loop_exited = 1;
}

