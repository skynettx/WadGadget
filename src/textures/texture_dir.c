//
// Copyright(C) 2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "conv/error.h"
#include "fs/vfile.h"
#include "fs/vfs.h"
#include "stringlib.h"
#include "ui/dialog.h"

#include "textures/textures.h"

// Implementation of a VFS directory that is backed by a textures list.
// Currently incomplete.
struct texture_dir {
	struct directory dir;
	struct textures *txs;
	struct pnames *pn;

	// Parent directory; always a WAD file.
	struct directory *parent_dir;
	uint64_t lump_serial;
};

static void TextureDirRefresh(void *_dir)
{
	struct texture_dir *dir = _dir;
	unsigned int i;
	struct directory_entry *ent;

	VFS_FreeEntries(&dir->dir);

	dir->dir.num_entries = dir->txs->num_textures;
	dir->dir.entries = checked_calloc(
	dir->txs->num_textures, sizeof(struct directory_entry));

	for (i = 0; i < dir->txs->num_textures; i++) {
		ent = &dir->dir.entries[i];
		ent->type = FILE_TYPE_TEXTURE;
		ent->name = checked_calloc(9, 1);
		memcpy(ent->name, dir->txs->textures[i]->name, 8);
		ent->name[8] = '\0';
		ent->size = 0;
		ent->serial_no = dir->txs->serial_nos[i];
	}
}

static VFILE *TextureDirOpen(void *dir, struct directory_entry *entry)
{
	return NULL;
}

static struct directory *TextureDirOpenDir(void *_dir,
                                           struct directory_entry *ent)
{
	struct texture_dir *dir = _dir;

	if (ent == VFS_PARENT_DIRECTORY) {
		VFS_DirectoryRef(dir->parent_dir);
		return dir->parent_dir;
	}

	return NULL;
}

static void TextureDirRemove(void *_dir, struct directory_entry *entry)
{
	struct texture_dir *dir = _dir;
	unsigned int idx = entry - dir->dir.entries;

	if (idx >= dir->txs->num_textures) {
		return;
	}

	free(dir->txs->textures[idx]);
	memmove(&dir->txs->textures[idx], &dir->txs->textures[idx + 1],
	        (dir->dir.num_entries - idx - 1) * sizeof(struct texture *));
	memmove(&dir->txs->serial_nos[idx], &dir->txs->serial_nos[idx + 1],
	        (dir->dir.num_entries - idx - 1) * sizeof(uint64_t));
	--dir->txs->num_textures;
	dir->txs->modified = true;
}

static void TextureDirRename(void *_dir, struct directory_entry *entry,
                             const char *new_name)
{
	struct texture_dir *dir = _dir;
	unsigned int idx = entry - dir->dir.entries;
	char *namedest;
	int i;

	if (idx >= dir->txs->num_textures) {
		return;
	}

	namedest = dir->txs->textures[idx]->name;

	for (i = 0; i < 8; i++) {
		namedest[i] = toupper(new_name[i]);
		if (namedest[i] == '\0') {
			break;
		}
	}

	dir->txs->modified = true;
}

static void TextureDirCommit(void *dir)
{
	// TODO: write the lump
}

static void TextureDirDescribe(char *buf, size_t buf_len, int cnt)
{
	if (cnt == 1) {
		snprintf(buf, buf_len, "1 texture");
	} else {
		snprintf(buf, buf_len, "%d textures", cnt);
	}
}

static bool TextureDirSave(struct texture_dir *dir)
{
	struct directory_entry *ent;
	struct wad_file *wf;
	VFILE *out, *texture_out;
	unsigned int idx;

	if (!dir->txs->modified) {
		return true;
	}

	ent = VFS_EntryBySerial(dir->parent_dir, dir->lump_serial);
	if (ent == NULL) {
		return false;
	}

	texture_out = TX_MarshalTextures(dir->txs);
	if (texture_out == NULL) {
		return false;
	}

	wf = VFS_WadFile(dir->parent_dir);
	assert(wf != NULL);
	idx = ent - dir->parent_dir->entries;
	out = W_OpenLumpRewrite(wf, idx);
	if (out == NULL) {
		vfclose(texture_out);
		return false;
	}

	vfcopy(texture_out, out);
	vfclose(texture_out);
	vfclose(out);
	W_CommitChanges(wf);

	return true;
}

static void TextureDirFree(void *_dir)
{
	struct texture_dir *dir = _dir;

	TextureDirSave(dir);
	if (dir->txs != NULL) {
		TX_FreeTextures(dir->txs);
	}
	if (dir->pn != NULL) {
		TX_FreePnames(dir->pn);
	}
	VFS_DirectoryUnref(dir->parent_dir);
}

struct directory_funcs texture_dir_funcs = {
	TextureDirRefresh,
	TextureDirOpen,
	TextureDirOpenDir,
	TextureDirRemove,
	TextureDirRename,
	TextureDirCommit,
	TextureDirDescribe,
	TextureDirFree,
};

static bool TextureDirLoad(struct texture_dir *dir)
{
	struct directory_entry *ent;
	VFILE *input;

	ent = VFS_EntryBySerial(dir->parent_dir, dir->lump_serial);
	if (ent == NULL) {
		return false;
	}

	input = VFS_OpenByEntry(dir->parent_dir, ent);
	if (input == NULL) {
		return false;
	}

	dir->txs = TX_UnmarshalTextures(input);
	return dir->txs != NULL;
}

static bool LoadPnames(struct texture_dir *dir)
{
	VFILE *input;
	struct directory_entry *ent =
		VFS_EntryByName(dir->parent_dir, "PNAMES");

	if (ent == NULL) {
		ConversionError("WAD does not contain a PNAMES lump.");
		return false;
	}

	input = VFS_OpenByEntry(dir->parent_dir, ent);
	if (input == NULL) {
		ConversionError("Failed to open PNAMES lump.");
		return false;
	}

	dir->pn = TX_UnmarshalPnames(input);
	if (input == NULL) {
		ConversionError("Failed to unmarshal PNAMES");
		return false;
	}

	return true;
}

struct directory *TX_OpenTextureDir(struct directory *parent,
                                    struct directory_entry *ent)
{
	struct texture_dir *dir =
		checked_calloc(1, sizeof(struct texture_dir));

	dir->dir.type = FILE_TYPE_TEXTURE_LIST;
	dir->dir.path = StringJoin("/", parent->path, ent->name, NULL);
	dir->dir.refcount = 1;
	dir->dir.entries = NULL;
	dir->dir.num_entries = 0;
	dir->dir.directory_funcs = &texture_dir_funcs;

	dir->parent_dir = parent;
	VFS_DirectoryRef(dir->parent_dir);
	dir->lump_serial = ent->serial_no;

	if (!TextureDirLoad(dir) || !LoadPnames(dir)) {
		TextureDirFree(dir);
		return NULL;
	}

	TextureDirRefresh(dir);

	return &dir->dir;
}
