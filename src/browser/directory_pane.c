//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#include "browser/directory_pane.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "ui/colors.h"
#include "stringlib.h"
#include "ui/pane.h"

static int HeaderEntries(struct directory_pane *dp)
{
	if (dp->dir->parent_name != NULL) {
		return 1;
	} else {
		return 0;
	}
}

static void DrawEntry(WINDOW *win, int idx, void *data)
{
	struct directory_pane *dp = data;
	const struct directory_entry *ent;
	const char *s, *p, *prefix = " ";
	int ent_idx;
	unsigned int w, i;
	char size[10] = "";

	w = getmaxx(win) - 1;

	if (idx == LIST_PANE_END_MARKER) {
		if (dp->dir->num_entries == 0) {
			mvwaddstr(win, 0, 0, " (empty)");
		}
		return;
	}

	ent_idx = idx - HeaderEntries(dp);

	if (ent_idx == -1) {
		prefix = "^ ";
		s = dp->dir->parent_name;
		wattron(win, COLOR_PAIR(PAIR_DIRECTORY));
	} else {
		ent = &dp->dir->entries[ent_idx];
		switch (ent->type) {
			case FILE_TYPE_DIR:
				wattron(win, A_BOLD);
				prefix = "/";
				break;
			case FILE_TYPE_WAD:
				wattron(win, COLOR_PAIR(PAIR_WAD_FILE));
				break;
			default:
				wattron(win, COLOR_PAIR(PAIR_WHITE_BLACK));
				break;
		}

		// Show insert point for where we'll import into the WAD:
		if (!dp->pane.active && idx == dp->pane.selected
		 && dp->dir->type != FILE_TYPE_DIR
		 && dp->dir->type != FILE_TYPE_PALETTES
		 && dp->dir->type != FILE_TYPE_PNAMES_LIST) {
			if ((termattrs() & A_UNDERLINE) != 0) {
				wattron(win, A_UNDERLINE);
			} else {
				prefix = "_";
			}
		}

		// We only show size for lumps (like NWT); for files it
		// is too cluttered (plus filenames can be much longer)
		if (ent->type == FILE_TYPE_LUMP) {
			VFS_DescribeSize(ent, size);
		}
		s = ent->name;
	}
	if (dp->pane.active && idx == dp->pane.selected) {
		wattron(win, A_REVERSE);
	} else if (ent_idx >= 0 &&
	           VFS_SetHas(&dp->tagged,
	                      dp->dir->entries[ent_idx].serial_no)) {
		wattron(win, COLOR_PAIR(PAIR_TAGGED));
	}
	mvwaddstr(win, 0, 0, prefix);
	i = strlen(prefix);
	for (p = s; i < w && *p != '\0'; i++, p++) {
		waddch(win, *p);
	}
	for (; i < w; i++) {
		waddch(win, ' ');
	}
	if (ent_idx == -1) {
		mvwaddch(win, 0, 0, ACS_LLCORNER);
		waddch(win, ACS_HLINE);
	} else if (strlen(size) > 0) {
		mvwaddstr(win, 0, w - strlen(size) - 2, " ");
		waddstr(win, size);
		waddch(win, ' ');
	}
	wattroff(win, A_UNDERLINE);
	wattroff(win, A_REVERSE);
	wattroff(win, A_BOLD);
	wattroff(win, COLOR_PAIR(PAIR_WHITE_BLACK));
	wattroff(win, COLOR_PAIR(PAIR_DIRECTORY));
	wattroff(win, COLOR_PAIR(PAIR_WAD_FILE));
	wattroff(win, COLOR_PAIR(PAIR_TAGGED));
}

static unsigned int NumEntries(void *data)
{
	struct directory_pane *dp = data;
	return dp->dir->num_entries + HeaderEntries(dp);
}

void B_DirectoryPaneReselect(struct directory_pane *p)
{
	int num_entries = NumEntries(p);
	if (p->pane.selected >= num_entries) {
		p->pane.selected = num_entries - 1;
	}
}

void B_DirectoryPaneSelectEntry(struct directory_pane *p,
                                struct directory_entry *ent)
{
	unsigned int idx = ent - p->dir->entries;
	if (idx >= p->dir->num_entries) {
		return;
	}
	UI_ListPaneSelect(&p->pane, idx + HeaderEntries(p));
}

void B_DirectoryPaneSelectByName(struct directory_pane *p, const char *name)
{
	struct directory_entry *entry = VFS_EntryByName(p->dir, name);
	if (entry != NULL) {
		B_DirectoryPaneSelectEntry(p, entry);
	}
}

void B_DirectoryPaneSelectBySerial(struct directory_pane *p,
                                   uint64_t serial_no)
{
	struct directory_entry *entry = VFS_EntryBySerial(p->dir, serial_no);
	if (entry != NULL) {
		B_DirectoryPaneSelectEntry(p, entry);
	}
}

static bool PrefixSearch(struct directory_pane *dp, const char *needle,
                         int start_index)
{
	size_t needle_len = strlen(needle);
	const struct directory_entry *ent;
	int i;

	for (i = start_index; i < dp->dir->num_entries; i++) {
		ent = &dp->dir->entries[i];
		if (!strncasecmp(ent->name, needle, needle_len)) {
			dp->pane.selected = i + HeaderEntries(dp);
			dp->pane.window_offset = dp->pane.selected >= 10 ?
			    dp->pane.selected - 10 : 0;
			return true;
		}
	}

	return false;
}

static bool SubstringSearch(struct directory_pane *dp, const char *needle,
                            int start_index)
{
	size_t haystack_len, needle_len = strlen(needle);
	const struct directory_entry *ent;
	int i, j;

	for (i = start_index; i < dp->dir->num_entries; i++) {
		ent = &dp->dir->entries[i];
		haystack_len = strlen(ent->name);
		if (haystack_len < needle_len) {
			continue;
		}
		for (j = 0; j < haystack_len - needle_len + 1; j++) {
			if (!strncasecmp(&ent->name[j], needle, needle_len)) {
				dp->pane.selected = i + HeaderEntries(dp);
				dp->pane.window_offset = dp->pane.selected >= 10 ?
				    dp->pane.selected - 10 : 0;
				return true;
			}
		}
	}

	return false;
}

void B_DirectoryPaneSearch(void *p, const char *needle)
{
	struct directory_pane *dp = p;

	if (strlen(needle) == 0) {
		return;
	}

	if (dp->dir->parent_name != NULL && !strcmp(needle, "..")) {
		dp->pane.selected = 0;
		dp->pane.window_offset = 0;
		return;
	}

	// Check for prefix first, so user can type entire lump name.
	if (!PrefixSearch(dp, needle, 0)) {
		// If nothing found, try a substring match.
		(void) SubstringSearch(dp, needle, 0);
	}
}

bool B_DirectoryPaneSearchAgain(void *p, const char *needle)
{
	struct directory_pane *dp = p;
	int start_index = dp->pane.selected;

	if (strlen(needle) == 0
	 || (dp->dir->parent_name != NULL && !strcmp(needle, ".."))) {
		return false;
	}

	// When searching again, we only do substring matches.
	return SubstringSearch(dp, needle, start_index)
	    || SubstringSearch(dp, needle, 0);
}

int B_DirectoryPaneSelected(struct directory_pane *p)
{
	return UI_ListPaneSelected(&p->pane) - HeaderEntries(p);
}

struct file_set *B_DirectoryPaneTagged(struct directory_pane *p)
{
	if (p->tagged.num_entries > 0) {
		return &p->tagged;
	} else {
		static struct file_set result;
		int selected = B_DirectoryPaneSelected(p);
		if (selected >= 0) {
			// Some devious pointer magic here.
			result.entries = &p->dir->entries[selected].serial_no;
			result.num_entries = 1;
		} else {
			result.entries = NULL;
			result.num_entries = 0;
		}
		return &result;
	}
}

void B_DirectoryPaneSetTagged(struct directory_pane *p, struct file_set *set)
{
	struct directory_entry *dirent;
	int idx = 0;

	VFS_CopySet(&p->tagged, set);

	// Jump to first in the set.
	dirent = VFS_IterateSet(p->dir, &p->tagged, &idx);
	if (dirent != NULL) {
		B_DirectoryPaneSelectBySerial(p, dirent->serial_no);
	}
}

static const struct list_pane_funcs directory_pane_funcs = {
	DrawEntry,
	NumEntries,
};

struct directory_entry *B_DirectoryPaneEntry(struct directory_pane *p)
{
	int selected = B_DirectoryPaneSelected(p);

	if (selected < 0) {
		return VFS_PARENT_DIRECTORY;
	}
	return &p->dir->entries[selected];
}

static bool DrawPane(void *p)
{
	struct directory_pane *dp = p;
	WINDOW *win = dp->pane.pane.window;
	int x, w, space;
	UI_ListPaneDraw(p);

	w = getmaxx(win);
	space = w - strlen(dp->pane.title) - 6;

	x = w - 2;

	if (dp->tagged.num_entries != 0 && space >= 6) {
		char buf[16];

		if (space >= 18) {
			snprintf(buf, sizeof(buf), "[%d marked]",
			         (int) dp->tagged.num_entries);
		} else {
			snprintf(buf, sizeof(buf), "[%d]",
			         (int) dp->tagged.num_entries);
		}
		x -= strlen(buf);
		space -= strlen(buf);

		wattron(win, COLOR_PAIR(PAIR_PANE_COLOR));
		mvwaddstr(win, 0, x, buf);
		wattroff(win, COLOR_PAIR(PAIR_PANE_COLOR));
	}

	if (dp->dir->readonly && space >= 4) {
		x -= 4;
		space -= 4;

		wattron(win, COLOR_PAIR(PAIR_PANE_COLOR));
		mvwaddstr(win, 0, x, "[RO]");
		wattroff(win, COLOR_PAIR(PAIR_PANE_COLOR));
	}

	return true;
}

struct directory_pane *UI_NewDirectoryPane(
	WINDOW *w, struct directory *dir)
{
	struct directory_pane *p;

	p = calloc(1, sizeof(struct directory_pane));
	UI_ListPaneInit(&p->pane, w, &directory_pane_funcs, p);
	p->pane.pane.draw = DrawPane;
	// TODO: Free
	UI_ListPaneSetTitle(&p->pane, PathBaseName(dir->path));
	p->dir = dir;
	// Select first item (assuming there is one):
	UI_ListPaneKeypress(&p->pane, KEY_DOWN);

	return p;
}
