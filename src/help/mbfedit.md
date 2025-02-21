# Marine's Best Friend Editing Features

## Contents

1. [New Linedef Types](#1-new-linedef-types)
2. [New Thing Types / Flags](#2-new-thing-types--flags)
3. [New Mobj Flags](#3-new-mobj-flags)
4. [DEH/BEX Mnemonic Names](#4-dehbex-mnemonic-names)
5. [DEH/BEX Code Pointers](#5-dehbex-code-pointers)
6. [DEHACKED lump](#6-dehacked-lump)
7. [OPTIONS lump](#7-options-lump)

## 1. New Linedef Types

**271: Transfer first sidedef's upper texture to tagged sectors' skies**

**272: Same as 271, only flipped horizontally**

These linedefs transfer wall textures to skies. F_SKY1 must still be used as
the floor or ceiling texture in the sectors for which sky is desired, but the
presence of a 271 or 272 property-transfer linedef can change the sky texture
to something other than a level-based default.

Every sector with F_SKY1 floor or ceiling which shares the same sector tag
as the 271 or 272 linedef will use a sky texture based on the upper texture
of the first sidedef in the 271 or 272 linedef. Sectors with F_SKY1 floors or
ceilings which are not tagged with 271 or 272 linedefs, behave just like Doom.

Horizontal offsets or scrolling in the transferred texture, is converted into
rotation of the sky texture. Vertical offsets or scrolling is transferred as
well, allowing for precise adjustments of sky position. Unpegging in the sky
transfer linedef does not affect the sky texture.

Horizontal scrolling of the transferred upper wall texture is converted into
rotation of the sky texture, but it occurs at a very slow speed relative to
the moving texture, allowing for long-period rotations (such as one complete
revolution per Doom real-time hour).

Effects other than sky-transfer effects are not excluded from the affected
sector(s), and tags can be shared so long as the effects are unambiguous.
For example, a wall-scrolling linedef can share a sector tag with both its
affectee linedef (the one being scrolled), and with the sector that the
latter controls. There is no ambiguity because one effect (scrolling) applies
to a linedef, while the other effect (sky transfer) applies to a sector.

If a sector underneath a special sky needs to be set up to have a different
purpose (for example, if it is a lift or a stairbuilder), then two tags will
need to be created, and the transfer linedef and any scrolling linedefs will
need to be duplicated as well, so that the same effect as far as the sky goes,
is duplicated in two separate sector tags. This will not affect sky appearance,
but it will allow a special sector which needs a unique tag, to sit under such
a sky.

Animated textures may be transferred to skies as well.

In Doom, skies were flipped horizontally. To maintain compatibility with this
practice, the 272 linedef flips the wall image horizontally. The 271 linedef
does not flip the wall image, and it is intended to make it easier to take
existing non-flipped wall textures and transfer them to skies.

Sky textures which are different must be separated by non-sky textures, or
else the results are undefined.

sky.wad is an illustration of the sky effect.

## 2. New Thing Types / Flags

There are a couple of new Thing types / flags:

    A. Friendly monster flag
    B. Reserved flag
    C. Dogs

### 2A. Friendly monster flag

At level startup, every object in a level's map is spawned with the FRIEND
mobj flag set if bit 7 (mask 128) is set in the Thing's flag bits. This
allows two of the same species in a map to be marked as being of a different
persuasion. However, for various design reasons, if a thing is marked as being
friendly in its mobj definition, then it will always be friendly when spawned
as a map object, regardless of the Thing's flags.

So far, editor support for this feature is weak, but if a value >= 128 can
be entered for a Thing's flags, then generally speaking, you only need to add
128 to the value to turn it into a friendly thing. Real players are always
considered friendly, regardless of their Thing flags.

### 2B. Reserved bit

The thing flag bit which is 8 positions left of the least significant bit, and
which has a bitmask value of 256, is reserved to be 0. If this bit is not 0,
then MBF will clear all bits which were unused by Doom, which effectively ANDs
the flags with the value 31. This is to accomodate Doom wads edited by such
editors as HellMaker, which put 1's inside bits that were unused in Doom.

It is designed so that any thing flag bits added since Doom, should be cleared
in order to get Doom behavior. With over 90% of Doom editors out there, unused
(or simply unrecognized) thing flag bits are, by default, set to 0. But in
HellMaker and maybe a few other editors, 1's are used, and with wads developed
under these editors, we assume that the wad is strictly a Doom wad, and hence
none of the flags added to the engine, post-Doom, should be set to '1'.

### 2C. Dogs

Dogs, which are mobj #140, have Doomednum 888 (i.e. Thing Type #888). They
are not spawned as friendly dogs, unless explicitly marked as friendly by the
friendly thing flag, or unless the entire species (140) is marked friendly.
But this doesn't affect the normal spawning of dogs as friendly coop player
replacements -- players or their bot replacements are always friends.

Dogs have these lumps embedded inside the .exe, which can be modified by wads:

DOGS* sprites with frames A-I (0-8).

    SFX #   Lump    Description
    --------------------------------
     109  DSDGSIT   Waking up bark
     110  DSDGATK   Attacking bark
     111  DSDGACT   Roaming bark
     112  DSDGDTH   Death sound
     113  DSDGPAIN  Pain sound

## 3. New Mobj Flags

There are three new Map Object (mobj) flags in MBF:

    A. FRIEND   (0x40000000)
    B. BOUNCES  (0x20000000)
    C. TOUCHY   (0x10000000)

### 3A. FRIEND  (0x40000000)  Mobj Flag

When the FRIEND mobj flag is set, a monster becomes the friend of the
player(s), and tries to help them.

Friendliness is a Map Object flag, not merely a Thing flag, because a monster's
friendliness can change during the game. For example, Arch-Viles transfer their
good or evil intentions towards the player, to the victims they resurrect. A
friendly Arch-Vile turns corpses into new friends, no matter what their motives
were when they died, and an unfriendly Arch-Vile turns them into player
enemies. Boss Spawners spawn monsters with the same friendliness as themselves.
Pain Elementals spawn Lost Souls with the same motives.

Friendly monsters with long-range missile capability (missilestates) fire back
only one shot each time they receive a painful hit from a friend such as the
player (monsters with automatic weapons fire back a short burst). Friendly
monsters without long-range missile capability do not retaliate when hit by
friendly fire.

Friendly monsters return to a player if they cannot find any enemy targets, or
if they lack long-range firing capability and they cannot seem to attack any
opponent for a significant period of time.

Player autoaiming does not fire at friends if there are any enemies within
autoaiming range. This avoids accidental shooting of friends by the player,
but does not rule out shooting friends if they are the only ones in sight
or they are the obvious target.

### 3B. BOUNCES  (0x20000000)  Mobj Flag

The BOUNCES mobj flag tells MBF that an object is "bouncy". What this means
exactly, depends on the rest of the flags:

**BOUNCES + MISSILE:**

With BOUNCES + MISSILE, the object behaves like a beta BFG fireball, exploding
on contact with walls and with non-inert objects, but bouncing off of floors
and ceilings. See the "Classic BFG" for an example of this flags combination.

**BOUNCES + NOGRAVITY:**

Unless it's a MISSILE too, the object generally bounces off of all surfaces
without loss of momentum.

**BOUNCES + FLOAT:**

Without NOGRAVITY, BOUNCES + FLOAT gives the ability for a monster to jump
when it needs to reach a high target, but to otherwise stay pretty low. See
dogfly.deh for an example of this flags combination.

For non-sentient objects, BOUNCES + FLOAT simply creates a higher bounce back.

BOUNCES + FLOAT + DROPOFF bounces back even higher than BOUNCES + FLOAT.

**BOUNCES:**

When BOUNCES isn't combined with MISSILE, it causes the object to simply bounce
off of floors and walls. If the object is under gravity (the NOGRAVITY flag is
not set), then its momentum is cut in half at each collision, in the component
of momentum which is perpendicular to the contacted surface. The object also
rolls up and down stairs, if under gravity. This flags combination is
illustrated in grenade.deh.

### 3C. TOUCHY  (0x10000000)  Mobj Flag

TOUCHY indicates that an object dies (or explodes) upon contact with a solid
object of a different species. It is used mainly to create mines which detonate
on contact (see mine.deh for an example). Other normal causes of death are not
ruled out, however, by the TOUCHY flag.

Pain Elementals and Lost Souls are considered part of the same species for the
purposes of dying by touch (but Barons and Hell Knights are not), because if
this weren't true, PEs and LSes would die almost immediately. (Also, a touchy
PE guarded by a flock of LSes has a nice ring to it :)

Different players are considered different species, so DM by touch is possible,
and suicide by touch is possible with voodoo dolls.

## 4. DEH/BEX Mnemonic Names

FRIEND, BOUNCES, and TOUCHY are allowed as DEH/BEX mnemonics for flags. In MBF,
the mnemonics may be arbitrarily separated by + (plus) | (vertical bar) or ,
(comma), as well as optional whitespace. Values are ORed, which means listing
a flag multiple times is the same as listing it once.

TRANSLATION1 and TRANSLATION2 are also allowed as DEH/BEX mnemonics, and are
suggested replacements for the Boom TRANSLATION mnemonic. TRANSLATION1 selects
the least significant bit of the color translation, while TRANSLATION2 selects
the most significant bit. MBF is bug-compatibile with Boom's TRANSLATION
mnemonic, interpreting TRANSLATION to mean the same as TRANSLATION1.

## 5. DEH/BEX Code Pointers

Several new code pointers have been added:

**A_Detonate** is similar to A_Explode in that it generate a radius of explosion
damage. However, instead of a fixed radius of 128, the radius is set by
changing the "missile damage" of the object being exploded. This allows
explosion damage to be varied easily.

**A_Mushroom** creates a mushroom-like explosion, throwing fireballs up in all
directions. There is primary damage from the explosion itself, and then
secondary damage from contact with the fireballs. The missile damage of the
spawning object controls how many fireballs are created. Optionally, the
misc1 ("Unknown 1") and misc2 ("Unknown 2") fields may be used to control
the angle and speed of the fireballs. These fields are fixed-point (16.16),
instead of whole numbers.

**A_Spawn** spawns an object in the same position as the spawner. The misc1 and
misc2 fields of the code pointer's frame (indicated by "Unknown 1" and
"Unknown 2" in DeHackEd), determine the type and z-position of the spawned
object, respectively. Note that if the spawner and spawnee are both solid,
they might stick together.

**A_Turn** rotates the object's angle by the number of degrees indicated by the
misc1 ("Unknown 1") field.

**A_Face** sets the object's angle to number of degrees in the misc1 ("Unknown 1")
field.

**A_Scratch** performs a melee (close-range) attack on the object's current target.
misc1 ("Unknown 1") indicates damage, while misc2 ("Unknown 2") indicates the
sound effect to use.

**A_PlaySound** plays a sound effect. misc1 ("Unknown 1") is the sound effect
number, and misc2 ("Unknown 2"), if nonzero, makes the sound full-volume
(like boss wakeup sounds).

**A_RandomJump** jumps randomly to the state in misc1 ("Unknown 1") with a
probability indicated by the misc2 ("Unknown 2") field (0-255). If the
jump doesn't occur, then it falls through to the next state. Note that
the frame's "tics" field must not be -1, for this to work correctly.

**A_LineEffect** allows activation of remote linedef effects by objects. misc1
("Unknown 1") indicates the linedef type, while misc2 ("Unknown 2") indicates
the sector tag. The tagged sector(s) are activated as though a player has
activated a linedef of the indicated type and tag. If the linedef is a S1 or
W1 type, then the effect blocks out all further A_LineEffect effects for the
object, no matter whether they are S1/W1 or SR/WR types.

**A_Die** causes an object to kill itself (i.e. commit suicide). It is primarily
used to make countdown timers in grenades.

## 6. DEHACKED lump

The DEHACKED lump allows .deh/.bex files to be embedded inside of wads. If
there are multiple wads with DEHACKED lumps, or even multiple DEHACKED lumps
inside the same wad, then the last wad or lump which is read, has priority
where there is a conflict. However, the presence of a DEHACKED lump doesn't
completely invalidate previous DEHACKED lumps -- the effects are cumulative,
although later deh files can undo the effects of earlier ones.

The order in which .deh/.bex files are read and interpreted is:

 1. .deh/.bex files listed on the command-line
 2. .wad files listed on the command-line with DEHACKED lumps
 3. Preincluded .wad files with DEHACKED lumps
 4. Preincluded .deh/.bex files (in General menu)

DEHACKED lumps should consist of ordinary .deh/.bex file text.

Include directives are not allowed inside DEHACKED lumps, since the whole
purpose of DEHACKED lumps is to be entirely self-contained within a wad.

## 7. OPTIONS lump

The OPTIONS lump allows wads to set game options, such as Doom Compatibility
options, chat strings, and enemies options.

The OPTIONS lump has the same format as mbf.cfg: A text file listing option
names and values, optionally separated by blank or comment lines. mbf.cfg
indicates options which are allowed to be set in wads, with asterisks (*).

When OPTIONS are set from wads, they are skipped on the menu screens.

Wad authors are urged to use these options responsibly, because they should
not be used to limit players, but to enhance the game. Many options such as
automap colors are allowed to be set from wads, but they should only be done
so if there are strong reasons to do so.

Players who are offended by wads setting certain options for them, should ask
wad authors not to set them, or should consider playing other wads. As a last
resort, players can use Deutex/Wintex/NWT/etc. to remove or modify the OPTIONS
lump themselves. They can also create a dummy wad with its own OPTIONS lump,
which completely replaces any OPTIONS set in wads. An OPTIONS lump with nothing
but blanks or comments, can be used to effectively nullify earlier ones.

Some options must be set in wads for compatibility reasons (otherwise the
wad may not work and the player might even get stuck in a level), while many
options are not necessary for compatibility.

Lee Killough                                            Last Updated 12/21/1998
