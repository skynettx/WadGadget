//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#include "wad_file.h"

struct lump_type;

extern const struct lump_type lump_type_level;
extern const struct lump_type lump_type_special;
extern const struct lump_type lump_type_sound;
extern const struct lump_type lump_type_graphic;
extern const struct lump_type lump_type_mus;
extern const struct lump_type lump_type_midi;
extern const struct lump_type lump_type_demo;
extern const struct lump_type lump_type_pcspeaker;
extern const struct lump_type lump_type_sized;
extern const struct lump_type lump_type_plaintext;
extern const struct lump_type lump_type_unknown;

const struct lump_type *LI_IdentifyLump(struct wad_file *f,
                                        unsigned int lump_index);
const char *LI_DescribeLump(const struct lump_type *t, struct wad_file *f,
                            unsigned int lump_index);

