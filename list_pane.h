#ifndef INCLUDE_LIST_PANE_H
#define INCLUDE_LIST_PANE_H

#include <curses.h>

struct list_pane {
	WINDOW *pane;
	unsigned int window_offset, selected;
	unsigned int active;
	const char *title;
	const char *(*get_entry_str)(struct list_pane *p, unsigned int i);
};

void UI_DrawListPane(struct list_pane *pane);
void UI_ListPaneInput(struct list_pane *p, int key);
void UI_ListPaneActive(struct list_pane *p, int active);

#endif /* #ifndef INCLUDE_LIST_PANE_H */

