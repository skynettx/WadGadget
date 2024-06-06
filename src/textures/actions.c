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

#include "actions.h"
#include "stringlib.h"
#include "textures/textures.h"
#include "ui/dialog.h"
#include "view.h"

static void PerformNewTexture(struct directory_pane *active_pane,
                              struct directory_pane *other_pane)
{
	int pos = UI_DirectoryPaneSelected(active_pane) + 1;
	struct textures *txs = TX_TextureList(active_pane->dir);
	struct texture t;

	char *name = UI_TextInputDialogBox(
		"New texture", "Create", 8,
		"Enter name for new texture:");
	if (name == NULL) {
		return;
	}

	memset(&t, 0, sizeof(t));
	strncpy(t.name, name, 8);
	t.width = 128;
	t.height = 128;
	TX_AddTexture(txs, pos, &t);
	VFS_Refresh(active_pane->dir);
	UI_ListPaneKeypress(active_pane, KEY_DOWN);
}

const struct action new_texture_action = {
	KEY_F(7), 'K', "NewTxt", ". New texture",
	PerformNewTexture,
};

static void PerformEditTextures(struct directory_pane *active_pane,
                                struct directory_pane *other_pane)
{
	struct directory *parent;
	struct directory_entry *ent;

	parent = TX_DirGetParent(active_pane->dir, &ent);
	OpenDirent(parent, ent);

	TX_DirReload(active_pane->dir);
	VFS_ClearSet(&active_pane->tagged);
}

const struct action edit_textures_action = {
	KEY_F(4), 'E', "EditCfg", "Edit texture config",
	PerformEditTextures,
};
