
#include "wad_file.h"

struct lump_type;

extern const struct lump_type lump_type_empty;
extern const struct lump_type lump_type_level;
extern const struct lump_type lump_type_special;
extern const struct lump_type lump_type_sound;
extern const struct lump_type lump_type_graphic;
extern const struct lump_type lump_type_music;
extern const struct lump_type lump_type_flat;
extern const struct lump_type lump_type_demo;
extern const struct lump_type lump_type_pcspeaker;
extern const struct lump_type lump_type_unknown;

const struct lump_type *LI_IdentifyLump(struct wad_file *f,
                                        unsigned int lump_index);
const char *LI_DescribeLump(const struct lump_type *t, struct wad_file *f,
                            unsigned int lump_index);

