//
// Copyright(C) 2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef PAGER__PAGER_H_INCLUDED
#define PAGER__PAGER_H_INCLUDED

#include <curses.h>
#include <stdbool.h>
#include <stddef.h>

#include "ui/actions_bar.h"
#include "ui/pane.h"

struct pager_config;
struct pager_link;

typedef void (*pager_draw_line_fn)(WINDOW *win, unsigned int line,
                                   void *user_data);
typedef void (*pager_get_link_fn)(struct pager_config *cfg, int idx,
                                  struct pager_link *link);

struct pager_link {
	int lineno;
	int column;
};

struct pager_config {
	const char *title;
	const char *help_file;
	pager_draw_line_fn draw_line;
	void *user_data;
	size_t num_lines;
	const struct action **actions;
	pager_get_link_fn get_link;
	int current_link;
	int num_links;
	int current_column;
};

struct pager {
	struct pane pane;
	WINDOW *search_pad;
	WINDOW *line_win;
	unsigned int window_offset;
	int search_line;
	char *last_search;
	struct pane_stack *stack;
	struct pager_config *cfg;
	char subtitle[15];
};

void P_InitPager(struct pager *p, struct pager_config *cfg);
void P_FreePager(struct pager *p);
void P_BlockOnInput(struct pager *p);
void P_RunPager(struct pager *p, bool fullscreen);
void P_SwitchConfig(struct pager_config *cfg);
void P_JumpToLine(struct pager *p, int lineno);
void P_JumpWithinWindow(struct pager *p, int lineno);
void P_ClearSearch(struct pager *p);
void P_OpenPager(struct pager *p);
void P_ClosePager(struct pager *p);

extern struct pager *current_pager;
extern const struct action exit_pager_action;
extern const struct action pager_help_action;
extern const struct action pager_search_action;
extern const struct action pager_search_again_action;
extern const struct action pager_prev_link_action;
extern const struct action pager_next_link_action;

#endif /* #ifndef PAGER__PAGER_H_INCLUDED */
