//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#include "browser/actions.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <curses.h>
#include <stdint.h>
#include <strings.h>

#include "browser/browser.h"
#include "common.h"
#include "conv/error.h"
#include "conv/import.h"
#include "ui/dialog.h"
#include "conv/export.h"
#include "ui/pane.h"
#include "pager/help.h"
#include "pager/hexdump.h"
#include "palette/palfs.h"
#include "stringlib.h"
#include "ui/title_bar.h"
#include "fs/vfs.h"
#include "textures/textures.h"
#include "view.h"
#include "browser/directory_pane.h"
#include "fs/vfile.h"
#include "fs/wad_file.h"
#include "ui/actions_bar.h"
#include "ui/list_pane.h"

#define WAD_JUNK_THRESHOLD_KB  500

bool B_CheckReadOnly(struct directory *dir)
{
	struct directory *orig_dir = dir;

	// If this is a directory list, it's the enclosing WAD we actually
	// want to check.
	if (dir->type == FILE_TYPE_TEXTURE_LIST
	 || dir->type == FILE_TYPE_PNAMES_LIST) {
		dir = TX_DirGetParent(dir, NULL);
	}

	// We req
	if (dir->type == FILE_TYPE_WAD
	 && dir->readonly && !W_IsReadOnly(VFS_WadFile(dir))) {
		if (!UI_ConfirmDialogBox("Edit IWAD?", "Edit IWAD", "Cancel",
		                         "'%s' is an IWAD file. Are you\n"
		                         "sure you want to modify it?",
		                         PathBaseName(dir->path))) {
			return false;
		}
		dir->readonly = false;
		orig_dir->readonly = false;
	}

	if (dir->readonly) {
		UI_MessageBox("'%s' is read-only.", PathBaseName(dir->path));
		return false;
	}

	return true;
}

static void CopyToDir(bool convert)
{
	struct directory *from = active_pane->dir, *to = other_pane->dir;
	struct file_set result = EMPTY_FILE_SET;
	struct file_set *export_set = B_DirectoryPaneTagged(active_pane);
	char buf[32];

	if (export_set->num_entries < 1) {
		UI_MessageBox("You have not selected anything to export.");
		VFS_FreeSet(&result);
		return;
	}

	if (!PerformExport(from, export_set, to, &result, convert)) {
		if (strlen(GetConversionError()) > 0) {
			UI_MessageBox("Error during export:\n%s",
			              GetConversionError());
		}
		VFS_FreeSet(&result);
		return;
	}

	B_DirectoryPaneSetTagged(other_pane, &result);

	// When we do an export or import, we create the new files/lumps
	// in the destination, and then switch to the other pane where they
	// are highlighted. The import/export functions both populate a
	// result set that contains the serial numbers of the new files.
	B_SwitchToPane(other_pane);
	VFS_DescribeSet(to, &result, buf, sizeof(buf));
	if (from->type == to->type) {
		UI_ShowNotice("%s copied.", buf);
	} else {
		UI_ShowNotice("%s exported.", buf);
	}

	VFS_FreeSet(&result);
}

static void CopyToWAD(bool convert)
{
	struct directory *from = active_pane->dir, *to = other_pane->dir;
	struct file_set *import_set = B_DirectoryPaneTagged(active_pane);
	int to_point = B_DirectoryPaneSelected(other_pane) + 1;
	struct file_set result = EMPTY_FILE_SET;
	char buf[32];

	if (import_set->num_entries < 1) {
		UI_MessageBox("You have not selected anything to import.");
		VFS_FreeSet(&result);
		return;
	}

	if (!PerformImport(from, import_set, to, to_point, &result, convert)) {
		if (strlen(GetConversionError()) > 0) {
			UI_MessageBox("Error during import:\n%s",
			              GetConversionError());
		}
		VFS_FreeSet(&result);
		VFS_Rollback(to);
		VFS_Refresh(to);
		return;
	}

	B_DirectoryPaneSetTagged(other_pane, &result);
	B_SwitchToPane(other_pane);
	VFS_DescribeSet(to, &result, buf, sizeof(buf));
	VFS_CommitChanges(to, "import of %s", buf);
	if (from->type == to->type) {
		UI_ShowNotice("%s copied.", buf);
	} else {
		UI_ShowNotice("%s imported.", buf);
	}

	VFS_FreeSet(&result);
}

static void PerformCopy(bool convert)
{
	struct directory_entry *ent = B_DirectoryPaneEntry(active_pane);

	if (ent != NULL && ent->type == FILE_TYPE_DIR) {
		UI_MessageBox("Copying directories is not supported.");
		return;
	}

	if (!B_CheckReadOnly(other_pane->dir)) {
		return;
	}

	ClearConversionErrors();

	switch (other_pane->dir->type) {
	case FILE_TYPE_DIR:
		CopyToDir(convert);
		break;
	case FILE_TYPE_WAD:
		CopyToWAD(convert);
		break;
	default:
		UI_MessageBox("Sorry, this isn't implemented yet.");
		break;
	}
}

static void PerformCopyConvert(void)
{
	PerformCopy(true);
}

static void PerformCopyNoConvert(void)
{
	PerformCopy(false);
}

const struct action copy_action = {
	KEY_F(5), 'C', "Copy", "> Copy",
	PerformCopyConvert,
};
const struct action copy_noconv_action = {
	SHIFT_KEY_F(5), 0, NULL, "> Copy (no convert)",
	PerformCopyNoConvert,
};
const struct action export_action = {
	KEY_F(5), 'C', "Export", "> Export",
	PerformCopyConvert,
};
const struct action export_noconv_action = {
	SHIFT_KEY_F(5), 0, NULL, "> Export (no convert)",
	PerformCopyNoConvert,
};
const struct action import_action = {
	KEY_F(5), 'C', "Import", "> Import",
	PerformCopyConvert,
};
const struct action import_noconv_action = {
	SHIFT_KEY_F(5), 0, NULL, "> Import (no convert)",
	PerformCopyNoConvert,
};

static void PerformUpdate(void)
{
	UI_MessageBox("Sorry, not implemented yet.");
}

const struct action update_action = {
	KEY_F(3), 'U', "Upd", "> Update",
	PerformUpdate,
};

static void PerformMkdir(void)
{
	char *input_filename, *filename;

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	input_filename = UI_TextInputDialogBox(
	    "Make directory", "Create", 30, "Name for new directory?");
	if (input_filename == NULL) {
		return;
	}
	filename = StringJoin("/", active_pane->dir->path, input_filename,
	                      NULL);
	if (mkdir(filename, 0777) == 0) {
		VFS_Refresh(active_pane->dir);
		B_DirectoryPaneSelectByName(active_pane, input_filename);
	} else {
		UI_MessageBox("Failed to create directory:\n%s",
		              strerror(errno));
	}
	free(input_filename);
	free(filename);
}

const struct action mkdir_action = {
	KEY_F(7), 'K', "Mkdir", ". Make directory",
	PerformMkdir,
};

static char *CreateWadInDir(struct directory *from, struct file_set *from_set,
                            struct directory *to, bool convert)
{
	struct file_set result = EMPTY_FILE_SET;
	struct directory *newfile;
	char *filename, *filename2;

	filename = UI_TextInputDialogBox(
		from_set->num_entries > 0 ? "Make new WAD"
		                          : "Make new empty WAD",
		"Create", 30, "Enter name for new WAD file:");

	if (filename == NULL) {
		return NULL;
	}

	// Forgot to include .wad extension?
	if (strchr(filename, '.') == NULL) {
		filename2 = StringJoin("", filename, ".wad", NULL);
		free(filename);
		filename = filename2;
	}

	if (VFS_EntryByName(to, filename) != NULL
	 && !UI_ConfirmDialogBox("Confirm Overwrite", "Overwrite", "Cancel",
	                         "Overwrite existing '%s'?", filename)) {
		free(filename);
		return NULL;
	}

	filename2 = StringJoin("", to->path, "/", filename, NULL);

	if (!W_CreateFile(filename2)) {
		UI_MessageBox("%s\nFailed to create new WAD file.", filename);
		free(filename);
		free(filename2);
		return NULL;
	}
	newfile = VFS_OpenDir(filename2);
	if (newfile == NULL) {
		UI_MessageBox("%s\nFailed to open new file after creating.",
		              filename);
		free(filename);
		free(filename2);
		return NULL;
	}

	free(filename2);
	ClearConversionErrors();

	if (PerformImport(from, from_set, newfile, 0, &result, convert)) {
		VFS_CommitChanges(newfile, "new WAD");
		UI_ShowNotice("New WAD contains %d lumps.",
		              result.num_entries);
	} else {
		if (strlen(GetConversionError()) > 0) {
			UI_MessageBox("Failed importing to new WAD:\n%s",
			              GetConversionError());
		}
		free(filename);
		filename = NULL;
	}
	VFS_Refresh(to);
	VFS_FreeSet(&result);
	VFS_CloseDir(newfile);

	return filename;
}

static void CreateWad(bool convert)
{
	struct directory_pane *from_pane, *to_pane;
	struct file_set *import_set;
	char *filename;

	if (active_pane->dir->type == FILE_TYPE_WAD
	 && other_pane->dir->type == FILE_TYPE_DIR) {
		// Export from existing WAD to new WAD
		from_pane = active_pane;
		to_pane = other_pane;
		if (from_pane->tagged.num_entries == 0) {
			UI_MessageBox("You have not selected any "
			              "lumps to export.");
			return;
		}
	} else if (active_pane->dir->type == FILE_TYPE_DIR) {
		// Create new WAD and import tagged files.
		from_pane = active_pane;
		to_pane = active_pane;
	} else {
		return;
	}

	import_set = &from_pane->tagged;
	filename = CreateWadInDir(from_pane->dir, import_set, to_pane->dir,
	                          convert);
	if (filename != NULL) {
		B_DirectoryPaneSearch(to_pane, filename);
		free(filename);
		B_SwitchToPane(to_pane);
	}
}

static void CreateWadConvert(void)
{
	CreateWad(true);
}

static void CreateWadNoConvert(void)
{
	CreateWad(false);
}

const struct action make_wad_action = {
	KEY_F(9), 'F', "MkWAD", ". Make WAD",
	CreateWadConvert,
};
const struct action make_wad_noconv_action = {
	SHIFT_KEY_F(9), 0, NULL, ". Make WAD (no convert)",
	CreateWadNoConvert,
};
const struct action export_wad_action = {
	KEY_F(9), 'F', "ExpWAD", ".> Export as WAD",
	CreateWadConvert,
};

static unsigned int *IndexesForTagged(struct directory *dir,
                                      struct file_set *set, size_t *cnt)
{
	struct directory_entry *ent;
	unsigned int *result;
	int i;

	// Build array of indexes into dir->entries[] for each tagged item.
	result = checked_calloc(set->num_entries, sizeof(int));

	i = 0;
	*cnt = 0;
	while ((ent = VFS_IterateSet(dir, set, &i)) != NULL) {
		assert(*cnt < set->num_entries);
		result[*cnt] = ent - dir->entries;
		++*cnt;
	}

	return result;
}

static bool IndexesAreContiguous(unsigned int *indexes, size_t cnt)
{
	int i;

	for (i = 0; i < cnt - 1; i++) {
		if (indexes[i + 1] != indexes[i] + 1) {
			return false;
		}
	}

	return true;
}

// MakeMoveMapping returns an array containing an index for each entry in
// vfs->entries that indicates the new location that entry should be
// swapped into.
static int *MakeMoveMapping(struct directory *dir, struct file_set *fs,
                            unsigned int *move_start)
{
	struct directory_entry *ent;
	int *result = checked_calloc(dir->num_entries, sizeof(int));
	int i, j, adj, move_end;

	for (i = 0; i < dir->num_entries; i++) {
		result[i] = -1;
	}

	// All entries before move_start will be moved out, so move_start
	// may effectively move backward in the array. Figure out
	// how much to adjust by.
	i = 0;
	adj = 0;
	while ((ent = VFS_IterateSet(dir, fs, &i)) != NULL) {
		unsigned int curr_index = ent - dir->entries;

		if (curr_index >= *move_start) {
			break;
		}
		adj++;
	}
	*move_start -= adj;

	// Now go through all tagged entries and assign their new
	// positions.
	i = 0;
	j = *move_start;
	move_end = *move_start;
	while ((ent = VFS_IterateSet(dir, fs, &i)) != NULL) {
		unsigned int curr_index = ent - dir->entries;

		result[curr_index] = j;
		++j;
		++move_end;
	}

	// Assign all the others to the gaps.
	j = 0;
	for (i = 0; i < dir->num_entries; i++) {
		if (result[i] != -1) {
			continue;
		}
		while (j >= *move_start && j < move_end) {
			++j;
		}
		result[i] = j;
		++j;
	}

	return result;
}

// TODO: This should be a VFS function.
static void MoveEntries(struct directory *dir, struct file_set *fs,
                        unsigned int *insert_start)
{
	int *new_index;
	int i, j;

	new_index = MakeMoveMapping(dir, fs, insert_start);

	// Go through all entries. Each forms a chain of swaps to be
	// followed until we end up back at the start.
	for (i = 0; i < dir->num_entries; i++) {
		j = new_index[i];
		while (j != i) {
			int next_j;
			VFS_SwapEntries(dir, i, j);
			next_j = new_index[j];
			// Don't swap again.
			new_index[j] = j;
			j = next_j;
		}
	}

	free(new_index);
}

static bool NullTextureCheck(struct directory *dir, struct file_set *tagged,
                             const char *operation, const char *yes)
{
	if (dir->num_entries == 0
	 || !VFS_SetHas(tagged, dir->entries[0].serial_no)
	 || dir->entries[0].type != FILE_TYPE_TEXTURE
	 || !StringHasPrefix(dir->entries[0].name, "AA")
	 || !StringHasSuffix(dir->path, "/TEXTURE1")) {
		return true;
	}

	return UI_ConfirmDialogBox(
		"Null texture warning", yes, "Cancel",
		"The '%s' texture is a dummy that\n"
		"needs be the first texture in the list.\n"
		"If you %s it, whatever texture becomes\n"
		"first in the list will not work properly.\n"
		"\nAre you sure you want to %s it?",
		dir->entries[0].name, operation, operation);
}

static void PerformRearrange(void)
{
	struct directory *dir = active_pane->dir;
	char descr[16];
	unsigned int *indexes, insert_point;
	bool noop;
	size_t cnt;

	if (active_pane->tagged.num_entries == 0) {
		UI_MessageBox("You have not selected any lumps to move.");
		return;
	}

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	indexes = IndexesForTagged(dir, &active_pane->tagged, &cnt);
	insert_point = B_DirectoryPaneSelected(active_pane) + 1;
	noop = IndexesAreContiguous(indexes, cnt)
	    && insert_point >= indexes[0]
	    && insert_point <= indexes[cnt - 1] + 1;
	free(indexes);

	VFS_DescribeSet(dir, &active_pane->tagged, descr, sizeof(descr));

	if (noop) {
		UI_ShowNotice("They're all in that position already!");
	} else if (NullTextureCheck(active_pane->dir, &active_pane->tagged,
	                            "move", "Move")) {
		MoveEntries(dir, &active_pane->tagged, &insert_point);
		VFS_CommitChanges(dir, "move of %s", descr);
		VFS_Refresh(dir);
		B_DirectoryPaneSelectEntry(active_pane,
		                           &dir->entries[insert_point]);
		UI_ShowNotice("%s moved.", descr);
	}
}

const struct action rearrange_action = {
	KEY_F(2), 'V', "Rearr", "Move (rearrange)",
	PerformRearrange,
};

static int CompareEntries(struct directory *dir, unsigned int i1,
                          unsigned int i2)
{
	int cmp = strncmp(dir->entries[i1].name, dir->entries[i2].name, 8);

	// Fallback comparison preserves existing order.
	if (cmp == 0) {
		return i1 - i2;
	} else {
		return cmp;
	}
}

// TODO: This should be a VFS function.
static void SortEntries(struct directory *dir, unsigned int *indexes,
                        unsigned int count)
{
	unsigned int i, pivot;

	if (count < 2) {
		return;
	}

	// Pick pivot from middle (to avoid degenerate case if already sorted,
	// and swap it to the start.
	pivot = count / 2;
	VFS_SwapEntries(dir, indexes[pivot], indexes[0]);
	pivot = 0;

	for (i = 1; i < count; i++) {
		if (CompareEntries(dir, indexes[i], indexes[pivot]) < 0) {
			// This belongs before the pivot.
			VFS_SwapEntries(dir, indexes[i], indexes[pivot]);
			++pivot;
			// Swap back pivot into place
			VFS_SwapEntries(dir, indexes[i], indexes[pivot]);
		}
	}

	SortEntries(dir, indexes, pivot);
	SortEntries(dir, indexes + pivot + 1, count - pivot - 1);
}

static void PerformSortEntries(void)
{
	struct directory *dir = active_pane->dir;
	unsigned int *indexes;
	char descr[16];
	int i;
	size_t num_tagged = active_pane->tagged.num_entries;

	if (num_tagged == 0) {
		UI_MessageBox("You have not selected anything to sort.");
		return;
	}

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	indexes = IndexesForTagged(dir, &active_pane->tagged, &num_tagged);

	// Sanity check; it usually doesn't make sense to sort if they're
	// not a contiguous sequence.
	if (!IndexesAreContiguous(indexes, num_tagged)
	 && !UI_ConfirmDialogBox("Sort", "Continue", "Cancel",
	                         "Tagged items are not contiguous.\n"
	                         "Continue?")) {
		free(indexes);
		return;
	}

	// Check if already sorted.
	for (i = 0; i < num_tagged - 1; i++) {
		if (CompareEntries(dir, indexes[i], indexes[i + 1]) > 0) {
			break;
		}
	}

	VFS_DescribeSet(dir, &active_pane->tagged, descr, sizeof(descr));

	if (i < num_tagged - 1) {
		SortEntries(dir, indexes, num_tagged);
		UI_ShowNotice("%s sorted.", descr);
	} else if (UI_ConfirmDialogBox("Sort", "Sort", "Cancel",
	                               "%s already sorted.\nSort into "
	                               "reverse order?", descr)) {
		// Reverse sort doesn't even require using SortEntries(). The
		// lumps are already sorted, so we just need to reverse
		// them.
		for (i = 0; i < num_tagged / 2; i++) {
			VFS_SwapEntries(dir, indexes[i],
			                indexes[num_tagged - i - 1]);
		}
		UI_ShowNotice("%s reverse sorted.", descr);
	}

	free(indexes);

	VFS_CommitChanges(dir, "sort of %s", descr);
	VFS_Refresh(active_pane->dir);
}

const struct action sort_entries_action = {
	SHIFT_KEY_F(2), ']', "Sort", "Sort",
	PerformSortEntries,
};

static void PerformNewLump(void)
{
	int selected = B_DirectoryPaneSelected(active_pane);
	struct wad_file *f = VFS_WadFile(active_pane->dir);

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	char *name = UI_TextInputDialogBox(
		"New lump", "Create", 8,
		"Enter name for new lump:");
	if (name == NULL) {
		return;
	}
	// TODO: Should we be creating through VFS?
	W_AddEntries(f, selected + 1, 1);
	W_SetLumpName(f, selected + 1, name);
	VFS_CommitChanges(active_pane->dir, "creation of '%.8s' lump", name);
	free(name);
	VFS_Refresh(active_pane->dir);
	UI_ListPaneKeypress(active_pane, KEY_DOWN);
}

const struct action new_lump_action = {
	KEY_F(7), 'K', "NewLump", ". New lump",
	PerformNewLump,
};

static void PerformRename(void)
{
	char *input_filename;
	struct file_set *tagged = B_DirectoryPaneTagged(active_pane);
	int selected = B_DirectoryPaneSelected(active_pane);
	char *old_name = active_pane->dir->entries[selected].name;
	uint64_t serial_no = active_pane->dir->entries[selected].serial_no;
	bool success;

	if (tagged->num_entries == 0) {
		UI_MessageBox(
		    "You have not selected anything to rename.");
		return;
	} else if (tagged->num_entries > 1) {
		UI_MessageBox(
		    "You can't rename more than one thing at once.");
		return;
	}

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	input_filename = UI_TextInputDialogBox(
	    "Rename", "Rename", 30, "New name for '%s'?", old_name);
	if (input_filename == NULL) {
		return;
	}
	success = VFS_Rename(active_pane->dir,
	                     &active_pane->dir->entries[selected],
	                     input_filename);
	if (success) {
		VFS_CommitChanges(active_pane->dir, "rename");
	} else {
		if (VFS_CanUndo(active_pane->dir)) {
			VFS_Rollback(active_pane->dir);
		}
		UI_MessageBox("Rename failed:\n%s", VFS_LastError());
	}
	VFS_Refresh(active_pane->dir);
	free(input_filename);
	B_DirectoryPaneSelectBySerial(active_pane, serial_no);
}

const struct action rename_action = {
	KEY_F(6), 'E', "Ren", ". Rename",
	PerformRename,
};

static void PerformDeleteNoConfirm(void)
{
	struct directory *dir = active_pane->dir;
	struct file_set *tagged = B_DirectoryPaneTagged(active_pane);
	bool success = true;
	char buf[64];
	int i;

	if (tagged->num_entries == 0) {
		UI_MessageBox("You have not selected anything to delete.");
		return;
	}

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	// We must build the description for the popup here, before
	// we delete the files.
	VFS_DescribeSet(dir, tagged, buf, sizeof(buf));

	// Note that there's a corner-case gotcha here. VFS serial
	// numbers for files are inode numbers, and through hardlinks
	// multiple files can have the same inode number. However,
	// the way things are implemented here, we only ever delete one
	// of each serial number. So the wrong file can end up being
	// deleted, but we'll never delete both.
	for (i = 0; success && i < tagged->num_entries; i++) {
		struct directory_entry *ent;
		ent = VFS_EntryBySerial(dir, tagged->entries[i]);
		if (ent == NULL) {
			continue;
		}
		success = VFS_Remove(dir, ent);
	}
	if (success) {
		VFS_CommitChanges(dir, "delete of %s", buf);
		UI_ShowNotice("%s deleted.", buf);
		VFS_ClearSet(&active_pane->tagged);
	} else {
		if (VFS_CanUndo(dir)) {
			VFS_Rollback(dir);
		}
		UI_MessageBox("Delete failed:\n%s", VFS_LastError());
	}
	VFS_Refresh(dir);
	B_DirectoryPaneReselect(active_pane);
	B_DirectoryPaneReselect(other_pane);
}

const struct action delete_no_confirm_action = {
	SHIFT_KEY_F(8), 0, NULL, "Delete (no confirm)",
	PerformDeleteNoConfirm,
};

static void PerformDelete(void)
{
	struct directory *dir = active_pane->dir;
	struct file_set *tagged = B_DirectoryPaneTagged(active_pane);
	char buf[64];

	if (tagged->num_entries == 0) {
		UI_MessageBox("You have not selected anything to delete.");
		return;
	}

	if (!B_CheckReadOnly(active_pane->dir)) {
		return;
	}

	VFS_DescribeSet(dir, tagged, buf, sizeof(buf));
	if (!UI_ConfirmDialogBox("Confirm Delete", "Delete", "Cancel",
	                         "Delete %s?", buf)) {
		return;
	}
	if (!NullTextureCheck(dir, tagged, "delete", "Delete")) {
		return;
	}

	PerformDeleteNoConfirm();
}

const struct action delete_action = {
	KEY_F(8), 'X', "Del", "Delete",
	PerformDelete,
};

static void PerformMarkPattern(void)
{
	struct directory_entry *first_match;
	size_t old_cnt;

	char *glob = UI_TextInputDialogBox(
		"Mark pattern", "Mark", 15,
		"Enter a wildcard pattern (eg. *.png):");
	if (glob == NULL) {
		return;
	}
	old_cnt = active_pane->tagged.num_entries;
	first_match = VFS_AddGlobToSet(active_pane->dir,
	                               &active_pane->tagged, glob);
	if (first_match == NULL) {
		UI_ShowNotice("No matches found.");
	} else {
		B_DirectoryPaneSelectEntry(active_pane, first_match);
		UI_ShowNotice("%d marked.",
		              active_pane->tagged.num_entries - old_cnt);
	}
	free(glob);
}

const struct action mark_pattern_action = {
	0, 'G', "MarkPat", ". Mark pattern",
	PerformMarkPattern,
};

static void PerformUnmarkAll(void)
{
	if (active_pane->tagged.num_entries == 0) {
		UI_ShowNotice("Nothing is marked.");
	} else {
		VFS_ClearSet(&active_pane->tagged);
		UI_ShowNotice("All marks cleared.");
	}
}

const struct action unmark_all_action = {
	KEY_F(10), 'A', "UnmrkAll", "Unmark all",
	PerformUnmarkAll,
};

static void PerformMark(void)
{
	int selected = B_DirectoryPaneSelected(active_pane);
	struct directory_entry *ent;

	if (B_CheckPathPaste()) {
		return;
	}

	if (active_pane->pane.selected <= 0) {
		return;
	}
	ent = &active_pane->dir->entries[selected];
	if (VFS_SetHas(&active_pane->tagged, ent->serial_no)) {
		VFS_RemoveFromSet(&active_pane->tagged, ent->serial_no);
	} else if (ent->type == FILE_TYPE_DIR) {
		UI_ShowNotice("Directories cannot be marked.");
	} else {
		VFS_AddToSet(&active_pane->tagged, ent->serial_no);
	}
	UI_ListPaneKeypress(active_pane, KEY_DOWN);
}

const struct action mark_action = {
	' ', 0, "Un/mark", "Mark/unmark",
	PerformMark,
};

// Called when closing a file to check if it needs cleaning out.
static void CheckCompactWad(struct directory_pane *pane)
{
	struct wad_file *wf = VFS_WadFile(pane->dir);
	const char *filename;
	uint32_t junk_bytes_kb;

	if (wf == NULL) {
		return;
	}

	// Has file been changed?
	if (VFS_CanUndo(pane->dir) == 0) {
		return;
	}
	// Insignificant amount of junk?
	junk_bytes_kb = W_NumJunkBytes(wf) / 1000;
	if (junk_bytes_kb < WAD_JUNK_THRESHOLD_KB) {
		return;
	}
	filename = PathBaseName(pane->dir->path);
	if (!UI_ConfirmDialogBox(
		"Compact WAD", "Compact", "Ignore",
		"'%s' contains %dKB of junk data.\nCompact now?",
		filename, junk_bytes_kb)) {
		return;
	}
	if (W_CompactWAD(wf)) {
		UI_ShowNotice("WAD compacted; %dKB saved.", junk_bytes_kb);
	} else {
		UI_MessageBox("Error when compacting '%s'.", filename);
	}
}

static void PerformQuit(void)
{
	// Editing may have left some junk data. Prompt to see if
	// the user wants to clean it out.
	CheckCompactWad(active_pane);
	CheckCompactWad(other_pane);
	UI_ExitMainLoop();
}

const struct action quit_action = {
	27, 'Q', "Quit", "Quit",
	PerformQuit,
};

static void PerformReload(void)
{
	VFS_Refresh(active_pane->dir);
}

const struct action reload_action = {
	0, 'R', "Reload", "Reload",
	PerformReload,
};

static void NavigateNew(struct directory_pane *curr_pane,
                        struct directory *new_dir)
{
	struct directory_pane *new_pane;
	struct directory_entry *ent;

	ent = B_DirectoryPaneEntry(curr_pane);
	new_pane = UI_NewDirectoryPane(NULL, new_dir);

	// Select subfolder we just navigated out of?
	if (ent == VFS_PARENT_DIRECTORY) {
		const char *old_path = curr_pane->dir->path;
		B_DirectoryPaneSearch(new_pane, PathBaseName(old_path));
	}

	if (new_pane != NULL) {
		// We're closing the current pane; if it is a WAD we might
		// want to clean out any junk data we left behind.
		CheckCompactWad(curr_pane);
		B_ReplacePane(curr_pane, new_pane);
		B_SwitchToPane(new_pane);
	}
}

static void ViewLump(struct directory *dir, struct directory_entry *ent)
{
	if (StringHasPrefix(ent->name, "TEXTURE")) {
		struct directory *new_dir;
		ClearConversionErrors();
		new_dir = TX_OpenTextureDir(dir, ent);
		if (new_dir == NULL) {
			UI_MessageBox("Error opening texture directory:\n%s",
			              GetConversionError());
			return;
		}
		NavigateNew(active_pane, new_dir);
		return;
	}

	if (!strcasecmp(ent->name, "PNAMES")) {
		struct directory *new_dir;
		ClearConversionErrors();
		new_dir = TX_OpenPnamesDir(dir, ent);
		if (new_dir == NULL) {
			UI_MessageBox("Error opening PNAMES directory:\n%s",
			              GetConversionError());
			return;
		}
		NavigateNew(active_pane, new_dir);
		return;
	}

	OpenDirent(dir, ent, false);
}

static void PerformView(void)
{
	struct directory *dir, *new_dir;
	struct directory_entry *ent;

	dir = active_pane->dir;
	ent = B_DirectoryPaneEntry(active_pane);

	switch (ent->type) {
	case FILE_TYPE_DIR:
	case FILE_TYPE_WAD:
		// Change directory?
		new_dir = VFS_OpenDirByEntry(dir, ent);
		if (new_dir == NULL) {
			UI_MessageBox("Error when opening '%s'.", ent->name);
			return;
		}
		NavigateNew(active_pane, new_dir);
		break;

	case FILE_TYPE_FILE:
		OpenDirent(dir, ent, false);
		break;

	case FILE_TYPE_LUMP:
		ViewLump(dir, ent);
		break;

	default:
		break;
	}
}

const struct action view_action = {
	'\r', 0,  "View", "View",
	PerformView,
};

static void PerformCompact(void)
{
	struct directory_entry *ent;
	struct directory *wad_dir;
	struct wad_file *wf;
	uint32_t junk_bytes;
	int selected;

	selected = B_DirectoryPaneSelected(active_pane);
	if (selected < 0) {
		return;
	}

	ent = B_DirectoryPaneEntry(active_pane);

	// Change directory?
	if (ent->type != FILE_TYPE_WAD) {
		UI_MessageBox("Can't compact; '%s' is not a WAD file.",
		              ent->name);
		return;
	}

	// We open the WAD file through the VFS directory API. This ensures
	// that we don't corrupt things by opening the same WAD twice.
	wad_dir = VFS_OpenDirByEntry(active_pane->dir, ent);
	if (wad_dir == NULL) {
		UI_MessageBox("Failed to open '%s'.", ent->name);
		return;
	}

	wf = VFS_WadFile(wad_dir);
	assert(wf != NULL);

	if (W_IsReadOnly(wf)) {
		B_CheckReadOnly(wad_dir);
		goto fail;
	}

	junk_bytes = W_NumJunkBytes(wf);
	if (junk_bytes == 0) {
		UI_ShowNotice("'%s' cannot be made any smaller.", ent->name);
		goto fail;
	}
	if (!UI_ConfirmDialogBox("Compact WAD", "Compact", "Cancel",
	                         "'%s' contains %d junk bytes.\n"
	                         "Compact WAD? This operation cannot\n"
	                         "be undone.", ent->name, junk_bytes)) {
		goto fail;
	}
	if (!B_CheckReadOnly(wad_dir)) {
		goto fail;
	}
	if (!W_CompactWAD(wf)) {
		UI_MessageBox("Failed to compact '%s'.", ent->name);
		goto fail;
	}

	UI_ShowNotice("WAD compacted; %dKB saved.", junk_bytes / 1000);

	VFS_Refresh(wad_dir);
	VFS_Refresh(active_pane->dir);

	VFS_ClearHistory(wad_dir);

fail:
	VFS_CloseDir(wad_dir);
}

const struct action compact_action = {
	KEY_F(2), 'T',  "Compact", "Compact WAD file",
	PerformCompact,
};

static void PerformHexdump(void)
{
	int selected = B_DirectoryPaneSelected(active_pane);
	struct directory_entry *ent;
	VFILE *input;

	if (selected < 0) {
		return;
	}
	ent = &active_pane->dir->entries[selected];

	input = VFS_OpenByEntry(active_pane->dir, ent);
	if (input == NULL) {
		return;
	}

	P_RunHexdumpPager(ent->name, input);
}

const struct action hexdump_action = {
	0, 'D', "Hexdump", "Hexdump",
	PerformHexdump
};

static void PerformUndo(void)
{
	struct directory *dir = active_pane->dir;
	const char *msg;
	int first_change;

	if (VFS_CanUndo(dir) == 0) {
		if (VFS_CanRedo(dir) == 0) {
			UI_ShowNotice("There is nothing to undo.");
		} else {
			// User has either undone every change, or has
			// exceeded the undo history maintained by the
			// WAD file code.
			UI_ShowNotice("Cannot undo any further.");
		}
		return;
	}

	if (!B_CheckReadOnly(dir)) {
		// This shouldn't actually be possible.
		return;
	}

	msg = VFS_LastCommitMessage(dir);

	VFS_Undo(dir, 1);
	first_change = VFS_Refresh(dir);

	// Move the cursor to the first lump identified as having changed:
	if (first_change >= 0) {
		B_DirectoryPaneSelectEntry(active_pane,
		                           &dir->entries[first_change]);
	}

	// Undo screws up serial numbers.
	VFS_ClearSet(&active_pane->tagged);
	B_DirectoryPaneReselect(active_pane);

	UI_ShowNotice("Undid %s.", msg);
}

const struct action undo_action = {
	0, 'Z', "Undo", "Undo",
	PerformUndo,
};

static void PerformRedo(void)
{
	struct directory *dir = active_pane->dir;
	int first_change;

	if (VFS_CanRedo(dir) == 0) {
		UI_ShowNotice("There is nothing to redo.");
		return;
	}

	if (!B_CheckReadOnly(dir)) {
		return;
	}

	VFS_Redo(dir, 1);
	first_change = VFS_Refresh(dir);

	// Move the cursor to the first lump identified as having changed:
	if (first_change >= 0) {
		B_DirectoryPaneSelectEntry(active_pane,
		                           &dir->entries[first_change]);
	}

	// Undo screws up serial numbers.
	VFS_ClearSet(&active_pane->tagged);
	B_DirectoryPaneReselect(active_pane);

	UI_ShowNotice("Redid %s.", VFS_LastCommitMessage(dir));
}

const struct action redo_action = {
	0, 'Y', "Redo", "| Redo",
	PerformRedo,
};

static void ShowHelp(void)
{
	int i;
	const struct {
		enum file_type ft;
		const char *fn;
	} help_files_per_type[] = {
		{FILE_TYPE_DIR, "dir_view.md"},
		{FILE_TYPE_WAD, "wad_view.md"},
		{FILE_TYPE_TEXTURE_LIST, "texture_editor.md"},
		{FILE_TYPE_PNAMES_LIST, "pnames_editor.md"},
		{FILE_TYPE_PALETTES, "palette.md"},
	};

	for (i = 0; i < arrlen(help_files_per_type); i++) {
		if (help_files_per_type[i].ft == active_pane->dir->type) {
			const char *filename = help_files_per_type[i].fn;
			P_RunHelpPager(filename);
			return;
		}
	}

	UI_MessageBox("Can't find a help page for file_type %d",
	              active_pane->dir->type);
}

const struct action help_action = {
	KEY_F(1), 'H', "Help", "Help",
	ShowHelp,
};

static void PerformShell(void)
{
	struct directory_entry *ent;
	char *marked_env = NULL;
	int idx = 0;

	if (chdir(active_pane->dir->path) != 0) {
		return;
	}

	while ((ent = VFS_IterateSet(active_pane->dir, &active_pane->tagged,
	                             &idx)) != NULL) {
		char *new_marked_env;

		if (marked_env == NULL) {
			new_marked_env = checked_strdup(ent->name);
		} else {
			new_marked_env = StringJoin(" ", marked_env,
			                            ent->name, NULL);
		}

		free(marked_env);
		marked_env = new_marked_env;
	}

	if (marked_env != NULL) {
		assert(setenv("MARKED", marked_env, 1) == 0);
	}

	RunShell();

	assert(unsetenv("MARKED") == 0);
	free(marked_env);

	VFS_RefreshAll();
	B_DirectoryPaneReselect(active_pane);
	B_DirectoryPaneReselect(other_pane);
}

const struct action open_shell_action = {
	KEY_F(4), 'O', "Shell", "Command Prompt here",
	PerformShell,
};

static void OpenPalettes(void)
{
	NavigateNew(active_pane, PAL_OpenDirectory(active_pane->dir));
}

const struct action open_palettes_action = {
	0, 'P', "Palettes", "| Palettes",
	OpenPalettes,
};
