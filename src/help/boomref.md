# BOOM reference v1.3 10/18/98

# Part I Linedef Types

## Section 0. Terminology

In BOOM actions are caused to happen thru linedef types.
BOOM has three kinds of linedef types:

Regular     - the linedef types that were already in DOOM II v1.9

Extended    - linedef types not in DOOM II v1.9 but less than 8192
              in value

Generalized - linedef types over 8192 in value that contain bit
              fields that independently control the actions of the sector
              affected.

A linedef with non-zero linedef type is called a linedef trigger. A
linedef trigger is always activated in one of three ways - pushing on
the first sidedef of the linedef, walking over the linedef, or
shooting the linedef with an impact weapon (fists, chainsaw, pistol,
shotgun, double barreled, or chaingun).

Linedefs activated by pushing on them come in two varieties. A manual
linedef affects the sector on the second sidedef of the line pushed.
A switched linedef affects all sectors that have the same tag field
as the linedef that was pushed.

Nearly all switched, walkover, and gun linedefs operate on the sectors
with the same tag as that in the tag field of the linedef.

Some linedefs are never activated per se, but simply create or control
an effect thru their existence and properties, usually affecting the
sectors sharing the linedef's tags. There are also a few special case
like line-line teleporters and exit linedefs that do not affect
sectors.

Some linedefs are only triggerable once, others are triggerable as
many times as you like.

Triggering types are denoted by the letters P, S, W, and G for
manual(push), switched, walkover, and gun. Their retriggerability is
denoted by a 1 or R following the letter. So the triggering types for
linedefs are:

P1 PR S1 SR W1 WR G1 GR

Often linedef actions depend on values in neighboring sectors. A
neighboring sector is one that shares a linedef in common, just
sharing a vertex is not sufficient.

In DOOM only one action on a sector could occur at a time. BOOM
supports one floor action, one ceiling action, and one lighting
action simultaneously.


## Section 1. Doors

A door is a sector, usually placed between two rooms, whose ceiling
raises to open, and lowers to close.

A door is fully closed when its ceiling height is equal to its floor
height.

A door is fully open when its ceiling height is 4 less than the lowest
neighbor ceiling adjacent to it.

A door may be set to an intermediate state initially, or thru the action
of a linedef trigger that affects ceilings or floors. The door is passable
to a player when its ceiling is at least 56 units higher than its floor.
In general the door is passable to a monster or a thing when its ceiling
is at least the monster or thing's height above the floor.

If a door has a ceiling height ABOVE the fully open height, then an
open door action moves the ceiling to the fully open height
instantly.

If a door has a ceiling height BELOW the fully closed height (that is
the ceiling of the door sector is lower than the floor of the door
sector) a close door action moves the ceiling to the fully closed
height instantly.

### Section 1.1 Door functions

**Open, Wait, Then Close**

On activation, door opens fully, waits a specified period, then
closes fully.

**Open and Stay Open**

On activation, door opens fully and remains there.

**Close and Stay Closed**

On activation, door closes fully, and remains there.

**Close, Wait, Then Open**

On activation, door closes fully, waits a specified period, then
opens fully.

### Section 1.2 Varieties of doors

A door can be triggered by pushing on it, walking over or pressing a
linedef trigger tagged to it, or shooting a linedef tagged to it.
These are called manual, walkover, switched, or gun doors resp.

Since a push door (P1/PR) has no use for its tag, BOOM has extended
the functionality to include changing any tagged sectors to maximum
neighbor lighting on fully opening, then to minimum neighbor lighting
on fully closing. This is true for regular, extended, and generalized
doors with push triggers.

A door trigger can be locked. This means that the door function will only
operate if the player is in possession of the right key(s). Regular
and extended door triggers only care if the player has a key of the
right color, they do not care which. Generalized door triggers
can distinguish between skull and card keys, and can also require
any key, or all keys in order to activate.

A door can have different speeds, slow, normal, fast or turbo.

A door can wait for different amounts of time.

A door may or may not be activatable by monsters.

Any door function except Close and Stay Closed, when closing and
encountering a monster or player's head will bounce harmlessly off
that head and return to fully open. A Close and Stay Closed will rest
on the head until it leaves the door sector.

### Section 1.3 Door linedef types

    Regular and Extended Door Types
    ---------------------------------------------------------------
    #        Class   Trig   Lock   Speed    Wait  Monst    Function

    1        Reg     PR     No     Slow     4s    Yes      Open, Wait, Then Close
    117      Reg     PR     No     Fast     4s    No       Open, Wait, Then Close
    63       Reg     SR     No     Slow     4s    No       Open, Wait, Then Close
    114      Reg     SR     No     Fast     4s    No       Open, Wait, Then Close
    29       Reg     S1     No     Slow     4s    No       Open, Wait, Then Close
    111      Reg     S1     No     Fast     4s    No       Open, Wait, Then Close
    90       Reg     WR     No     Slow     4s    No       Open, Wait, Then Close
    105      Reg     WR     No     Fast     4s    No       Open, Wait, Then Close
    4        Reg     W1     No     Slow     4s    No       Open, Wait, Then Close
    108      Reg     W1     No     Fast     4s    No       Open, Wait, Then Close

    31       Reg     P1     No     Slow     --    No       Open and Stay Open
    118      Reg     P1     No     Fast     --    No       Open and Stay Open
    61       Reg     SR     No     Slow     --    No       Open and Stay Open
    115      Reg     SR     No     Fast     --    No       Open and Stay Open
    103      Reg     S1     No     Slow     --    No       Open and Stay Open
    112      Reg     S1     No     Fast     --    No       Open and Stay Open
    86       Reg     WR     No     Slow     --    No       Open and Stay Open
    106      Reg     WR     No     Fast     --    No       Open and Stay Open
    2        Reg     W1     No     Slow     --    No       Open and Stay Open
    109      Reg     W1     No     Fast     --    No       Open and Stay Open
    46       Reg     GR     No     Slow     --    No       Open and Stay Open

    42       Reg     SR     No     Slow     --    No       Close and Stay Closed
    116      Reg     SR     No     Fast     --    No       Close and Stay Closed
    50       Reg     S1     No     Slow     --    No       Close and Stay Closed
    113      Reg     S1     No     Fast     --    No       Close and Stay Closed
    75       Reg     WR     No     Slow     --    No       Close and Stay Closed
    107      Reg     WR     No     Fast     --    No       Close and Stay Closed
    3        Reg     W1     No     Slow     --    No       Close and Stay Closed
    110      Reg     W1     No     Fast     --    No       Close and Stay Closed

    196      Ext     SR     No     Slow     30s   No       Close, Wait, Then Open
    175      Ext     S1     No     Slow     30s   No       Close, Wait, Then Open
    76       Reg     WR     No     Slow     30s   No       Close, Wait, Then Open
    16       Reg     W1     No     Slow     30s   No       Close, Wait, Then Open

    Regular and Extended Locked Door Types
    ---------------------------------------------------------------
    #        Class   Trig   Lock   Speed    Wait  Monst    Function

    26       Reg     PR     Blue   Slow     4s    No       Open, Wait, Then Close
    28       Reg     PR     Red    Slow     4s    No       Open, Wait, Then Close
    27       Reg     PR     Yell   Slow     4s    No       Open, Wait, Then Close

    32       Reg     P1     Blue   Slow     --    No       Open and Stay Open
    33       Reg     P1     Red    Slow     --    No       Open and Stay Open
    34       Reg     P1     Yell   Slow     --    No       Open and Stay Open
    99       Reg     SR     Blue   Fast     --    No       Open and Stay Open
    134      Reg     SR     Red    Fast     --    No       Open and Stay Open
    136      Reg     SR     Yell   Fast     --    No       Open and Stay Open
    133      Reg     S1     Blue   Fast     --    No       Open and Stay Open
    135      Reg     S1     Red    Fast     --    No       Open and Stay Open
    137      Reg     S1     Yell   Fast     --    No       Open and Stay Open


There are two generalized door linedef types, Generalized Door, and
Generalized Locked Door. The following tables show the possibilities
for each parameter, any combination of parameters is allowed:

Slow and Fast represent the same speeds as slow and fast regular doors. Normal
is twice as fast as slow, Fast is twice normal, Turbo is twice Fast.

    Generalized Door Types
    ---------------------------------------------------------------
    #        Class   Trig   Lock   Speed    Wait  Monst    Function

    3C00H-   Gen     P1/PR  No     Slow     1s    Yes      Open, Wait, Then Close
    4000H            S1/SR         Normal   4s    No       Open and Stay Open
                     W1/WR         Fast     9s             Close and Stay Closed
                     G1/GR         Turbo    30s            Close, Wait, Then Open


    Generalized Locked Door Types
    ---------------------------------------------------------------
    #        Class   Trig   Lock   Speed    Wait  Monst    Function

    3800H-   Gen     P1/PR  Any    Slow     1s    No       Open, Wait, Then Close
    3C00H            S1/SR  Blue   Normal   4s             Open and Stay Open
                     W1/WR  Red    Fast     9s             Close and Stay Closed
                     G1/GR  Yell   Turbo    30s            Close, Wait, Then Open
                            BlueC
                            RedC
                            YellC
                            BlueS
                            RedS
                            YellS
                            All3
                            All6

## Section 2. Floors

### Section 2.1 Floor targets

**Lowest Neighbor Floor (LnF)**

This means that the floor moves to the height of the lowest
neighboring floor including the floor itself. If the floor
direction is up (only possible with generalized floors) motion is
instant, else at the floor's speed.

**Next Neighbor Floor (NnF)**

This means that the floor moves up to the height of the lowest
adjacent floor greater in height than the current, or down to the
height of the highest adjacent floor less in height than the current,
as determined by the floor's direction. Instant motion is not
possible in this case. If no such floor exists, the floor does not move.

**Lowest Neighbor Ceiling (LnC)**

This means that the floor height changes to the height of the lowest
ceiling possessed by any neighboring sector, including that floor's
ceiling. If the target height is in the opposite direction to floor
motion, motion is instant, otherwise at the floor action's speed.

**8 Under Lowest Neighbor Ceiling (8uLnC)**

This means that the floor height changes to 8 less than the height
of the lowest ceiling possessed by any neighboring sector, including
that floor's ceiling. If the target height is in the opposite
direction to floor motion, motion is instant, otherwise at the floor
action's speed.

**Highest Neighbor Floor (HnF)**

This means that the floor height changes to the height of the highest
neighboring floor, excluding the floor itself. If no neighbor floor
exists, the floor moves down to -32000. If the target height is in
the opposite direction to floor motion, the motion is instant,
otherwise it occurs at the floor action's speed.

**8 Above Highest Neighbor Floor (8aHnF)**

This means that the floor height changes to 8 above the height of
the highest neighboring floor, excluding the floor itself. If no
neighbor floor exists, the floor moves down to -31992. If the target
height is in the opposite direction to floor motion, the motion is
instant, otherwise it occurs at the floor action's speed.

**Ceiling**

The floor moves up until its at ceiling height, instantly if floor
direction is down (only available with generalized types), at the
floor speed if the direction is up.

**24 Units (24)**

The floor moves 24 units in the floor action's direction. Instant
motion is not possible with this linedef type.

**32 Units (32)**

The floor moves 32 units in the floor action's direction. Instant
motion is not possible with this linedef type.

**512 Units (512)**

The floor moves 512 units in the floor action's direction. Instant
motion is not possible with this linedef type.

**Shortest Lower Texture (SLT)**

The floor moves the height of the shortest lower texture on the
boundary of the sector, in the floor direction. Instant motion is not
possible with this type. In the case that there is no surrounding
texture the motion is to -32000 or +32000 depending on direction.

**None**

Some pure texture type changes are provided for changing the floor
texture and/or sector type without moving the floor.

### Section 2.2 Varieties of floors

A floor can be activated by pushing on a linedef bounding it
(generalized types only), or by pushing on a switch with the same tag
as the floor sector, or by walking over a linedef with the same tag
as the floor, or by shooting a linedef with the same tag as the
floor with an impact weapon.

A floor can move either Up or Down.

A floor can move with speeds of Slow, Normal, Fast, or Turbo. If the
target height specified by the floor function (see Floor Targets
above) is in the opposite direction to the floor's motion, then
travel is instantaneous, otherwise its at the speed specified.

A floor action can be a texture change type, in which case after the
action the floor texture of the affected floor, and possibly the
sector type of the affected floor are changed to those of a model
sector. The sector type may be zeroed instead of copied from the
model, or not changed at all. These change types are referred to
below as Tx (texture only), Tx0 (type zeroed), and TxTy (texture and
type changed). The model sector for the change may be the sector on
the first sidedef of the trigger (trigger model) or the sector with
floor at destination height across the lowest numbered two-sided
linedef surrounding the affected sector (numeric model). If no model
sector exists, no change occurs. If a change occurs, floor texture is
always affected, lighting is never affected, even that corresponding
to the sector's type, nor is any other sector property other than the
sector's type.

Numeric model algorithm:

 1. Find all floors adjacent to the tagged floor at destination height
 2. Find the lowest numbered linedef separating those floors from that tagged
 3. The sector on the other side of that linedef is the model

A floor action can have the crush property, in which case players and
monsters are crushed when the floor tries to move above the point
where they fit exactly underneath the ceiling. This means they take
damage until they die, leave the sector, or the floor action is stopped.
A floor action never reverses on encountering an obstacle, even if
the crush property is not true, the floor merely remains in the same
position until the obstacle is removed or dies, then continues.

### Section 2.3 Floor linedef types

    Regular and Extended Floor Types
    -------------------------------------------------------------------
    #     Class   Trig   Dir Spd   Chg  Mdl Mon Crsh  Target

    60    Reg     SR     Dn  Slow  None --  No  No    Lowest Neighbor Floor
    23    Reg     S1     Dn  Slow  None --  No  No    Lowest Neighbor Floor
    82    Reg     WR     Dn  Slow  None --  No  No    Lowest Neighbor Floor
    38    Reg     W1     Dn  Slow  None --  No  No    Lowest Neighbor Floor

    177   Ext     SR     Dn  Slow  TxTy Num No  No    Lowest Neighbor Floor
    159   Ext     S1     Dn  Slow  TxTy Num No  No    Lowest Neighbor Floor
    84    Reg     WR     Dn  Slow  TxTy Num No  No    Lowest Neighbor Floor
    37    Reg     W1     Dn  Slow  TxTy Num No  No    Lowest Neighbor Floor

    69    Reg     SR     Up  Slow  None --  No  No    Next Neighbor Floor
    18    Reg     S1     Up  Slow  None --  No  No    Next Neighbor Floor
    128   Reg     WR     Up  Slow  None --  No  No    Next Neighbor Floor
    119   Reg     W1     Up  Slow  None --  No  No    Next Neighbor Floor

    132   Reg     SR     Up  Fast  None --  No  No    Next Neighbor Floor
    131   Reg     S1     Up  Fast  None --  No  No    Next Neighbor Floor
    129   Reg     WR     Up  Fast  None --  No  No    Next Neighbor Floor
    130   Reg     W1     Up  Fast  None --  No  No    Next Neighbor Floor

    222   Ext     SR     Dn  Slow  None --  No  No    Next Neighbor Floor
    221   Ext     S1     Dn  Slow  None --  No  No    Next Neighbor Floor
    220   Ext     WR     Dn  Slow  None --  No  No    Next Neighbor Floor
    219   Ext     W1     Dn  Slow  None --  No  No    Next Neighbor Floor

    64    Reg     SR     Up  Slow  None --  No  No    Lowest Neighbor Ceiling
    101   Reg     S1     Up  Slow  None --  No  No    Lowest Neighbor Ceiling
    91    Reg     WR     Up  Slow  None --  No  No    Lowest Neighbor Ceiling
    5     Reg     W1     Up  Slow  None --  No  No    Lowest Neighbor Ceiling
    24    Reg     G1     Up  Slow  None --  No  No    Lowest Neighbor Ceiling

    65    Reg     SR     Up  Slow  None --  No  Yes   Lowest Neighbor Ceiling - 8
    55    Reg     S1     Up  Slow  None --  No  Yes   Lowest Neighbor Ceiling - 8
    94    Reg     WR     Up  Slow  None --  No  Yes   Lowest Neighbor Ceiling - 8
    56    Reg     W1     Up  Slow  None --  No  Yes   Lowest Neighbor Ceiling - 8

    45    Reg     SR     Dn  Slow  None --  No  No    Highest Neighbor Floor
    102   Reg     S1     Dn  Slow  None --  No  No    Highest Neighbor Floor
    83    Reg     WR     Dn  Slow  None --  No  No    Highest Neighbor Floor
    19    Reg     W1     Dn  Slow  None --  No  No    Highest Neighbor Floor

    70    Reg     SR     Dn  Fast  None --  No  No    Highest Neighbor Floor + 8
    71    Reg     S1     Dn  Fast  None --  No  No    Highest Neighbor Floor + 8
    98    Reg     WR     Dn  Fast  None --  No  No    Highest Neighbor Floor + 8
    36    Reg     W1     Dn  Fast  None --  No  No    Highest Neighbor Floor + 8

    180   Ext     SR     Up  Slow  None --  No  No    Absolute 24
    161   Ext     S1     Up  Slow  None --  No  No    Absolute 24
    92    Reg     WR     Up  Slow  None --  No  No    Absolute 24
    58    Reg     W1     Up  Slow  None --  No  No    Absolute 24

    179   Ext     SR     Up  Slow  TxTy Trg No  No    Absolute 24
    160   Ext     S1     Up  Slow  TxTy Trg No  No    Absolute 24
    93    Reg     WR     Up  Slow  TxTy Trg No  No    Absolute 24
    59    Reg     W1     Up  Slow  TxTy Trg No  No    Absolute 24

    176   Ext     SR     Up  Slow  None --  No  No    Abs Shortest Lower Texture
    158   Ext     S1     Up  Slow  None --  No  No    Abs Shortest Lower Texture
    96    Reg     WR     Up  Slow  None --  No  No    Abs Shortest Lower Texture
    30    Reg     W1     Up  Slow  None --  No  No    Abs Shortest Lower Texture

    178   Ext     SR     Up  Slow  None --  No  No    Absolute 512
    140   Reg     S1     Up  Slow  None --  No  No    Absolute 512
    147   Ext     WR     Up  Slow  None --  No  No    Absolute 512
    142   Ext     W1     Up  Slow  None --  No  No    Absolute 512

    190   Ext     SR     --  ----  TxTy Trg No  No    None
    189   Ext     S1     --  ----  TxTy Trg No  No    None
    154   Ext     WR     --  ----  TxTy Trg No  No    None
    153   Ext     W1     --  ----  TxTy Trg No  No    None

    78    Ext     SR     --  ----  TxTy Num No  No    None
    241   Ext     S1     --  ----  TxTy Num No  No    None
    240   Ext     WR     --  ----  TxTy Num No  No    None
    239   Ext     W1     --  ----  TxTy Num No  No    None

The following tables show the possibilities for generalized floor
linedef type parameters. Any combination of parameters is allowed:

Slow and Fast represent the same speeds as slow and fast regular
floors. Normal is twice as fast as slow, Fast is twice normal, Turbo
is twice Fast.

    Generalized Floor Types
    ---------------------------------------------------------------------------
    #      Class   Trig   Dir Spd   *Chg *Mdl Mon Crsh  Target

    6000H- Gen     P1/PR  Up  Slow   None Trg Yes Yes   Lowest Neighbor Floor
    7FFFH          S1/SR  Dn  Normal Tx   Num No  No    Next Neighbor Floor
                   W1/WR      Fast   Tx0                Lowest Neighbor Ceiling
                   G1/GR      Turbo  TxTy               Highest Neighbor Floor
                                                        Ceiling
                                                        24
                                                        32
    *Mon(ster) enabled must be No if                    Shortest Lower Texture
    Chg field is not None

    Tx = Texture copied only           Trg = Trigger Model
    Tx0 = Texture copied and Type->0   Num = Numeric Model
    TxTy = Texture and Type copied


## Section 3. Ceilings

### Section 3.1 Ceiling Targets

**Highest Neighbor Ceiling (HnC)**

This means that the ceiling moves to the height of the highest
neighboring ceiling NOT including the ceiling itself. If the ceiling
direction is down (only possible with generalized ceilings) motion is
instant, else at the ceiling's speed. If no adjacent ceiling exists
the ceiling moves to -32000 units.

**Next Neighbor Ceiling (NnC)**

This means that the ceiling moves up to the height of the lowest
adjacent ceiling greater in height than the current, or to the
height of the highest adjacent ceiling less in height than the current,
as determined by the ceiling's direction. Instant motion is not
possible in this case. If no such ceiling exists, the ceiling does
not move.

**Lowest Neighbor Ceiling (LnC)**

This means that the ceiling height changes to the height of the lowest
ceiling possessed by any neighboring sector, NOT including itself.
If the target height is in the opposite direction to ceiling
motion, motion is instant, otherwise at the ceiling action's speed.
If no adjacent ceiling exists the ceiling moves to 32000 units.

**Highest Neighbor Floor (HnF)**

This means that the ceiling height changes to the height of the
highest neighboring floor, excluding the ceiling's floor itself. If
no neighbor floor exists, the ceiling moves down to -32000 or the
ceiling's floor, whichever is higher. If the target height is in the
opposite direction to ceiling motion, the motion is instant,
otherwise it occurs at the ceiling action's speed.

**Floor**

The ceiling moves down until its at floor height, instantly if
ceiling direction is up (only available with generalized types), at
the ceiling speed if the direction is down.

**8 Above Floor (8aF)**

This means that the ceiling height changes to 8 above the height of
the ceiling's floor. If the target height is in the opposite direction
to ceiling motion, the motion is instant, otherwise it occurs at the
ceiling action's speed.

**24 Units (24)**

The ceiling moves 24 units in the ceiling action's direction. Instant
motion is not possible with this linedef type.

**32 Units (32)**

The ceiling moves 32 units in the ceiling action's direction. Instant
motion is not possible with this linedef type.

**Shortest Upper Texture (SUT)**

The ceiling moves the height of the shortest upper texture on the
boundary of the sector, in the ceiling direction. Instant motion is not
possible with this type. In the case that there is no surrounding
texture the motion is to -32000 or +32000 depending on direction.

### Section 3.2 Varieties of ceilings

A ceiling can be activated by pushing on a linedef bounding it
(generalized types only), or by pushing on a switch with the same tag
as the ceiling sector, or by walking over a linedef with the same tag
as the ceiling, or by shooting a linedef with the same tag as the
ceiling with an impact weapon (generalized types only).

A ceiling can move either Up or Down.

A ceiling can move with speeds of Slow, Normal, Fast, or Turbo. If the
target height specified by the ceiling function (see Ceiling Targets
above) is in the opposite direction to the ceiling's motion, then
travel is instantaneous, otherwise its at the speed specified.

A ceiling action can be a texture change type, in which case after
the action the ceiling texture of the affected ceiling, and possibly
the sector type of the affected ceiling are changed to those of a
model sector. The sector type may be zeroed instead of copied from
the model, or not changed at all. These change types are referred to
below as Tx (texture only), Tx0 (type set to 0), and TxTy (texture
and type copied from model). The model sector for the change may be
the sector on the first sidedef of the trigger (trigger model) or the
sector with ceiling at destination height across the lowest numbered
two-sided linedef surrounding the affected sector (numeric model). If
no model sector exists, no change occurs. If a change occurs, ceiling
texture is always affected, lighting is never affected, even that
corresponding to the sector's type, nor is any other sector property
other than the sector's type.

Numeric model algorithm:

 1. Find all ceilings adjacent to the tagged ceiling at destination height
 2. Find the lowest numbered linedef separating those ceilings from that tagged
 3. The sector on the other side of that linedef is the model

A ceiling action can have the crush property (generalized types only),
in which case players and monsters are crushed when the ceiling tries
to move below the point where they fit exactly underneath the
ceiling. This means they take damage until they die, leave the
sector, or the ceiling action is stopped.  A ceiling action never
reverses on encountering an obstacle, even if the crush property is
not true, the ceiling merely remains in the same position until the
obstacle is removed or dies, then continues.

### Section 3.3 Ceiling linedef types

    Regular and Extended Ceiling Types
    -------------------------------------------------------------------
    #     Class   Trig   Dir Spd   *Chg *Mdl Mon Crsh Target

    43    Reg     SR     Dn  Fast  None  --  No  No   Floor
    41    Reg     S1     Dn  Fast  None  --  No  No   Floor
    152   Ext     WR     Dn  Fast  None  --  No  No   Floor
    145   Ext     W1     Dn  Fast  None  --  No  No   Floor

    186   Ext     SR     Up  Slow  None  --  No  No   Highest Neighbor Ceiling
    166   Ext     S1     Up  Slow  None  --  No  No   Highest Neighbor Ceiling
    151   Ext     WR     Up  Slow  None  --  No  No   Highest Neighbor Ceiling
    40    Reg     W1     Up  Slow  None  --  No  No   Highest Neighbor Ceiling

    187   Ext     SR     Dn  Slow  None  --  No  No   8 Above Floor
    167   Ext     S1     Dn  Slow  None  --  No  No   8 Above Floor
    72    Reg     WR     Dn  Slow  None  --  No  No   8 Above Floor
    44    Reg     W1     Dn  Slow  None  --  No  No   8 Above Floor

    205   Ext     SR     Dn  Slow  None  --  No  No   Lowest Neighbor Ceiling
    203   Ext     S1     Dn  Slow  None  --  No  No   Lowest Neighbor Ceiling
    201   Ext     WR     Dn  Slow  None  --  No  No   Lowest Neighbor Ceiling
    199   Ext     W1     Dn  Slow  None  --  No  No   Lowest Neighbor Ceiling

    206   Ext     SR     Dn  Slow  None  --  No  No   Highest Neighbor Floor
    204   Ext     S1     Dn  Slow  None  --  No  No   Highest Neighbor Floor
    202   Ext     WR     Dn  Slow  None  --  No  No   Highest Neighbor Floor
    200   Ext     W1     Dn  Slow  None  --  No  No   Highest Neighbor Floor

    Generalized Ceiling Types
    ---------------------------------------------------------------------------
    #      Class   Trig   Dir Spd   *Chg *Mdl Mon Crsh  Target

    4000H- Gen     P1/PR  Up  Slow   None Trg Yes Yes   Highest Neighbor Ceiling
    5FFFH          S1/SR  Dn  Normal Tx   Num No  No    Next Neighbor Ceiling
                   W1/WR      Fast   Tx0                Lowest Neighbor Ceiling
                   G1/GR      Turbo  TxTy               Highest Neighbor Floor
                                                        Floor
                                                        24
                                                        32
    *Mon(ster) enabled must be No if                    Shortest Upper Texture
    Chg field is not None

    Tx = Texture copied only           Trg = Trigger Model
    Tx0 = Texture copied and Type->0   Num = Numeric Model
    TxTy = Texture and Type copied

## Section 4. Platforms (Lifts)

A platform is basically a floor action involving two heights. The
simple raise platform actions, for example, differ from the
corresponding floor actions in that if they encounter an obstacle,
they reverse direction and return to their former height.

The most often used kind of platform is a lift which travels from its
current height to the target height, then waits a specified time and
returns to it former height.

### Section 4.1 Platform Targets

**Lowest Neighbor Floor**

This means that the platforms "low" height is the height of the
lowest surrounding floor, including the platform itself. The "high"
height is the original height of the floor.

**Next Lowest Neighbor Floor**

This means that the platforms "low" height is the height of the
highest surrounding floor lower than the floor itself. If no lower
floor exists, no motion occurs as the "low" and "high" heights are
then both equal to the floors current height.

**Lowest Neighbor Ceiling**

This means that the platforms "low" height is the height of the
lowest surrounding ceiling unless this is higher than the floor
itself. If no adjacent ceiling exists, or is higher than the floor no
motion occurs as the "low" and "high" heights are then both equal to
the floors current height.

**Lowest and Highest Floor**

This target sets the "low" height to the lowest neighboring floor,
including the floor itself, and the "high" height to the highest
neighboring floor, including the floor itself. When this target is
used the floor moves perpetually between the two heights. Once
triggered this type of linedef runs permanently, even if the motion
is temporarily suspended with a Stop type. No other floor action can
be commanded on the sector after this type is begun.

**Ceiling**

This target sets the "high" height to the ceiling of the sector and
the "low" height to the floor height of the sector and is only used in
the instant toggle type that switches the floor between the ceiling
and its original height on each activation. This is also the ONLY
instant platform type.

**Raise Next Floor**

This means that the "high" height is the lowest surrounding floor
higher than the platform. If no higher adjacent floor exists no
motion will occur.

**Raise 24 Units**

The "low" height is the original floor height, the "high" height is 24
more.

**Raise 32 Units**

The "low" height is the original floor height, the "high" height is 32
more.

### Section 4.2 Varieties of Platforms

A platform can be activated by pushing on a linedef bounding it
(generalized types only), or by pushing on a switch with the same tag
as the platform sector, or by walking over a linedef with the same tag
as the platform, or by shooting a linedef with the same tag as the
platform with an impact weapon.

A platform can move with speeds of Slow, Normal, Fast, or Turbo.
Only the instant toggle platform moves instantly, all others move at
the platform's speed.

A platform can have a delay, in between when it reaches "high" height
and returns to "low" height, or at both ends of the motion in the
case of perpetual lifts.

A platform action can be a texture change type, in which case after
the action the floor texture of the affected floor, and possibly the
sector type of the affected floor are changed to those of a model
sector. The sector type may be zeroed instead of copied from the
model, or not changed at all. These change types are referred to
below as Tx (texture only), Tx0 (type zeroed), and TxTy (texture and
type changed). The model sector for the change is always the sector
on the first sidedef of the trigger (trigger model).  If a change
occurs, floor texture is always affected, lighting is never affected,
even that corresponding to the sector's type, nor is any other sector
property other than the sector's type.

### Section 4.3 Platform Linedef types

    Regular and Extended Platform Types
    -------------------------------------------------------------------
    #     Class  Trig   Dly Spd  Chg  Mdl Mon Target

    66    Reg    SR     --  Slow Tx   Trg No  Raise 24 Units
    15    Reg    S1     --  Slow Tx   Trg No  Raise 24 Units
    148   Ext    WR     --  Slow Tx   Trg No  Raise 24 Units
    143   Ext    W1     --  Slow Tx   Trg No  Raise 24 Units

    67    Reg    SR     --  Slow Tx0  Trg No  Raise 32 Units
    14    Reg    S1     --  Slow Tx0  Trg No  Raise 32 Units
    149   Ext    WR     --  Slow Tx0  Trg No  Raise 32 Units
    144   Ext    W1     --  Slow Tx0  Trg No  Raise 32 Units

    68    Reg    SR     --  Slow Tx0  Trg No  Raise Next Floor
    20    Reg    S1     --  Slow Tx0  Trg No  Raise Next Floor
    95    Reg    WR     --  Slow Tx0  Trg No  Raise Next Floor
    22    Reg    W1     --  Slow Tx0  Trg No  Raise Next Floor
    47    Reg    G1     --  Slow Tx0  Trg No  Raise Next Floor

    181   Ext    SR     3s  Slow None --  No  Lowest and Highest Floor (perpetual)
    162   Ext    S1     3s  Slow None --  No  Lowest and Highest Floor (perpetual)
    87    Reg    WR     3s  Slow None --  No  Lowest and Highest Floor (perpetual)
    53    Reg    W1     3s  Slow None --  No  Lowest and Highest Floor (perpetual)

    182   Ext    SR     --  ---- ---- --  --  Stop
    163   Ext    S1     --  ---- ---- --  --  Stop
    89    Reg    WR     --  ---- ---- --  --  Stop
    54    Reg    W1     --  ---- ---- --  --  Stop

    62    Reg    SR     3s  Slow None --  No  Lowest Neighbor Floor (lift)
    21    Reg    S1     3s  Slow None --  No  Lowest Neighbor Floor (lift)
    88    Reg    WR     3s  Slow None --  No  Lowest Neighbor Floor (lift)
    10    Reg    W1     3s  Slow None --  No  Lowest Neighbor Floor (lift)

    123   Reg    SR     3s  Fast None --  No  Lowest Neighbor Floor (lift)
    122   Reg    S1     3s  Fast None --  No  Lowest Neighbor Floor (lift)
    120   Reg    WR     3s  Fast None --  No  Lowest Neighbor Floor (lift)
    121   Reg    W1     3s  Fast None --  No  Lowest Neighbor Floor (lift)

    211   Ext    SR     --  Inst None --  No  Ceiling (toggle)
    212   Ext    WR     --  Inst None --  No  Ceiling (toggle)

    Generalized Lift Types
    ---------------------------------------------------------------------------
    #      Class   Trig   Dly Spd    Mon  Target

    3400H- Gen     P1/PR  1s  Slow   Yes  Lowest Neighbor Floor
    37FFH          S1/SR  3s  Normal No   Next Lowest Neighbor Floor
                   W1/WR  5s  Fast        Lowest Neighbor Ceiling
                   G1/GR  10s Turbo       Lowest and Highest Floor (perpetual)

## Section 5. Crusher Ceilings

A crusher ceiling is a linedef type that causes the ceiling to cycle
between its starting height and 8 above the floor, damaging
monsters and players that happen to be in between. Barrels explode
when crushed.

Once a crusher ceiling is started it remains running for the
remainder of the level even if temporarily suspended with a stop type.
No other ceiling action can be used in that sector thereafter.

### Section 5.1 Varieties of Crushers

A crusher can be activated by pushing on a linedef bounding it
(generalized types only), or by pushing on a switch with the same tag
as the crusher sector, or by walking over a linedef with the same tag
as the crusher, or by shooting a linedef with the same tag as the
crusher with an impact weapon (generalized types only).

A crusher has a speed: slow, normal, fast, or turbo. The slower the
speed, the more damage the crusher does when crushing, simply thru
being applied longer. When a slow or normal crusher is moving down
and encounters something to crush, it slows down even more, by a
factor of 8. This persists until it reaches bottom of stroke and
starts up again. Fast and turbo crushers do not slow down.

A crusher can be silent. The regular silent crusher makes platform
stop and start noises at top and bottom of stroke. The generalized
silent crusher is completely quiet.

A crusher linedef is provided to stop a crusher in its current
position. Care should be used that this doesn't lock the player out
of an area of the wad if the crusher is too low to pass. A crusher
can be restarted, but not changed, with any crusher linedef.

### Section 5.2 Crusher Linedef Types

    Regular and Extended Crusher Types
    -------------------------------------------------------------------
    #     Class  Trig   Spd  Mon Silent Action

    184   Ext    SR     Slow No  No     Start
    49    Reg    S1     Slow No  No     Start
    73    Reg    WR     Slow No  No     Start
    25    Reg    W1     Slow No  No     Start

    183   Ext    SR     Fast No  No     Start
    164   Ext    S1     Fast No  No     Start
    77    Reg    WR     Fast No  No     Start
    6     Reg    W1     Fast No  No     Start

    185   Ext    SR     Slow No  Yes    Start
    165   Ext    S1     Slow No  Yes    Start
    150   Ext    WR     Slow No  Yes    Start
    141   Reg    W1     Slow No  Yes    Start

    188   Ext    SR     ---- --  --     Stop
    168   Ext    S1     ---- --  --     Stop
    74,   Reg    WR     ---- --  --     Stop
    57,   Reg    W1     ---- --  --     Stop

    Generalized Crusher Types
    ---------------------------------------------------------------------------
    #      Class   Trig   Spd    Mon  Silent

    2F80H- Gen     P1/PR  Slow   Yes  Yes
    2FFFH          S1/SR  Normal No   No
                   W1/WR  Fast
                   G1/GR  Turbo

## Section 6. Stair Builders

A stair builder is a linedef type that sets a sequence of sectors
defined by a complex rule to an ascending or descending sequence of
heights.

The rule for stair building is as follows:

1. The first step to rise or fall is the tagged sector. It rises or
falls by the stair stepsize, at the stair speed, in the stair
direction..

2. To find the next step (sector) affected examine each two-sided
linedef with first sidedef facing into the previous step in numerical
order. For each such linedef if the sector on the other side is
already active, or has already risen from this stair, the linedef is
skipped. If textures are not ignored then any linedef with a
different floor texture on its second side is also skipped. The next step
to rise is the sector on the other side of the first linedef not
skipped. If none exists, stair building ceases.

The next step moves to the height of the previous step plus or minus
the stepsize depending on the direction the stairs build, up or down.
If the motion is in the same direction as the stairs build, it occurs
at stair build speed, otherwise it is instant.

3. Repeat step 2 until stair building ceases.

### Section 6.1 Varieties of Stair Builders

A stair can be activated by pushing on a linedef bounding it
(generalized types only), or by pushing on a switch with the same tag
as the stair sector, or by walking over a linedef with the same tag
as the stair, or by shooting a linedef with the same tag as the
stair with an impact weapon (generalized types only).

Only extended and generalized stair types are retriggerable. The
extended retriggerable stairs are mostly useless, though triggering a
stair with stepsize 8 twice might be used. The generalized
retriggerable stairs alternate building up and down on each
activation which is much more useful.

A stair can build up or down (generalized types only).

A stair can have a step size of 4, 8, 16, or 24. Only generalized
types support stepsize of 4 or 24.

A stair can have build speeds of slow, normal, fast or turbo. Only
generalized types support speeds of normal or turbo.

A stair can stop on encountering a different texture or ignore
(generalized types only) different textures and continue.

Only the regular build fast, stepsize 16 stair has the property that
monsters and players can be crushed by the motion, all others do not
crush.

### Section 6.2 Regular and Extended Stair Builder Types

    Regular and Extended Stairs Types
    -------------------------------------------------------------------
    #     Class  Trig  Dir Spd   Step Ignore Mon

    258   Ext    SR    Up  Slow  8    No     No
    7     Reg    S1    Up  Slow  8    No     No
    256   Ext    WR    Up  Slow  8    No     No
    8     Reg    W1    Up  Slow  8    No     No

    259   Ext    SR    Up  Fast  16   No     No
    127   Reg    S1    Up  Fast  16   No     No
    257   Ext    WR    Up  Fast  16   No     No
    100   Reg    W1    Up  Fast  16   No     No

    Generalized Stairs Types
    ---------------------------------------------------------------------------
    #      Class   Trig   Dir Spd     Step  Ignore Mon

    3000H- Gen     P1/PR  Up  Slow    4     Yes    Yes
    33FFH          S1/SR  Dn  Normal  8     No     No
                   W1/WR      Fast    16
                   G1/GR      Turbo   24

## Section 7 Elevators

An elevator is a linedef type that moves both floor and ceiling
together. All elevator linedefs are extended, there are no regular or
generalized elevator types. Instant elevator motion is not possible,
and monsters cannot activate elevators. All elevator triggers are
either switched or walkover.

### Section 7.1 Elevator Targets

**Next Highest Floor**

The elevator floor moves to the lowest adjacent floor higher than the
elevator's floor, the ceiling staying the same height above the floor.
If there is no higher floor the elevator doesn't move.

**Next Lowest Floor**

The elevator floor moves to the highest adjacent floor lower than the
current floor, the ceiling staying the same height above the floor. If
there is no lower floor the elevator doesn't move.

**Current Floor**

The elevator floor moves to the height of the floor on the first
sidedef of the triggering line, the ceiling remaining the same height
above the elevator floor.

### Section 7.2 Elevator Linedef Types

    Extended Elevator types
    -------------------------------------------------------------------
    #     Class  Trig  Spd    Target

    230   Ext    SR    Fast   Next Highest Floor
    229   Ext    S1    Fast   Next Highest Floor
    228   Ext    WR    Fast   Next Highest Floor
    227   Ext    W1    Fast   Next Highest Floor

    234   Ext    SR    Fast   Next Lowest Floor
    233   Ext    S1    Fast   Next Lowest Floor
    232   Ext    WR    Fast   Next Lowest Floor
    231   Ext    W1    Fast   Next Lowest Floor

    238   Ext    SR    Fast   Current Floor
    237   Ext    S1    Fast   Current Floor
    236   Ext    WR    Fast   Current Floor
    235   Ext    W1    Fast   Current Floor

## Section 7 Lighting

The lighting linedef types change the lighting in the tagged sector.
All are regular or extended types, there are no generalized lighting
types. All are switched or walkovers.

### Section 7.1 Lighting Targets

**Lights to Minimum Neighbor**

Each tagged sector is set to the minimum light level found in any
adjacent sector. The tagged sectors are changed in numerical order,
and this may influence the result.

**Lights to Maximum Neighbor**

Each tagged sector is set to the maximum light level found in any
adjacent sector. The tagged sectors are changed in numerical order,
and this may influence the result.

**Blinking**

Each tagged sector begins blinking between two light levels. The
brighter level is the light level in the tagged sector. The darker
level is the minimum neighbor light level, or 0 if all neighbor
sectors have lighting greater than or equal to the sector's at the
time of activation. The blinking is non-synchronous, beginning 1-9
gametics after activation, with a dark period of 1 sec (35 gametics)
and a bright period of 1/7 sec (5 gametics).

**35 Units**

Each tagged sector is set to a light level of 35 units.

**255 Units**

Each tagged sector is set to a light level of 255 units.

### Section 7.2 Lighting Linedef Types

    Regular and Extended Lighting types
    -------------------------------------------------------------------
    #     Class  Trig  Target

    139   Reg    SR    35 Units
    170   Ext    S1    35 Units
    79    Reg    WR    35 Units
    35    Reg    W1    35 Units

    138   Reg    SR    255 Units
    171   Ext    S1    255 Units
    81    Reg    WR    255 Units
    13    Reg    W1    255 Units

    192   Ext    SR    Maximum Neighbor
    169   Ext    S1    Maximum Neighbor
    80    Reg    WR    Maximum Neighbor
    12    Reg    W1    Maximum Neighbor

    194   Ext    SR    Minimum Neighbor
    173   Ext    S1    Minimum Neighbor
    157   Ext    WR    Minimum Neighbor
    104   Reg    W1    Minimum Neighbor

    193   Ext    SR    Blinking
    172   Ext    S1    Blinking
    156   Ext    WR    Blinking
    17    Reg    W1    Blinking

## Section 8 Exits

An exit linedef type ends the current level bringing up the
intermission screen with its report of kills/items/secrets or frags
in the case of deathmatch. Obviously, none are retriggerable, and
none require tags, since no sector is affected.

### Section 8.1 Exit Varieties

An exit can be activated by pushing on a switch with the exit type
or  by walking over a linedef with the exit type, or by shooting a
linedef with the exit type with an impact weapon.

An exit can be normal or secret. The normal exit simply ends the
level and takes you to the next level via the intermission screen. A
secret exit only works in a special level depending on the IWAD being
played. In DOOM the secret exits can be on E1M3, E2M5, or E3M6. In
DOOM II they can be in levels 15 and 31. If a secret exit is used in
any other level, it brings you back to the start of the current
level. In DOOM the secret exits go from E1M3 to E1M9, E2M5 to E2M9,
and E3M6 to E3M9. In DOOM II they go from 15 to 31 and from 31 to 32.
In DOOM II a normal exit from 31 or 32 goes to level 16.

### Section 8.2 Exit Linedef Types

    Regular and Extended Exit types
    -------------------------------------------------------------------
    #     Class  Trig  Type

    11    Reg    S1    Normal
    52    Reg    W1    Normal
    197   Ext    G1    Normal

    51    Reg    S1    Secret
    124   Reg    W1    Secret
    198   Ext    G1    Secret

## Section 9 Teleporters

A teleporter is a linedef that when crossed or switched makes the
player or monster appear elsewhere in the level. There are only
regular and extended teleporters, no generalized teleporters exist
(yet).  Teleporters are the only direction sensitive walkover
triggers in DOOM (to allow them to be exited without teleporting
again). They must be crossed from the first sidedef side in order to
work.

The place that the player or monster appears is set in one of two
ways. The more common is by a teleport thing in the sector tagged to
the teleport line. The newer completely player preserving line-line
silent teleport makes the player appear relative to the exit line
(identified by having the same tag as the entry line) with the same
orientation, position, momentum and height that they had relative to
the entry line just before teleportation. A special kind of line-line
teleporter is also provided that reverses the player's orientation by
180 degrees.

In the case of several lines tagged the same as the teleport line, or
several sectors tagged the same, or several teleport things in the
tagged sector, in all cases the lowest numbered one is used.

In DOOM crossing a W1 teleport line in the wrong direction used it
up, and it was never activatable. In BOOM this is not the case, it
does not teleport but remains active until next crossed in the right
direction.

If the exit of a teleporter is blocked a monster (or anything besides
a player) will not teleport thru it. However if the exit of teleporter
is blocked a player will, including into a down crusher, a monster,
or another player. In the latter two cases the monster/player
occupying the spot dies messily, and this is called a telefrag.

### Section 9.1 Teleport Varieties

A teleporter can be walkover or switched, and retriggerable or not.

A teleporter can have destination set by a teleport thing in the
tagged sector, or by a line tagged the same as the teleport line.
These are called thing teleporters and line teleporters resp.

A teleporter can preserve orientation or set orientation from the
position, height, and angle of the teleport thing. Note a thing
destination teleporter always sets position, though it may preserve
orientation otherwise.

A teleporter can produce green fog and a whoosh noise, or it can be
silent, in which case it does neither.

A teleporter can transport monsters only, or both players and
monsters.

### Section 9.2 Teleport Linedef types

    Regular and Extended Teleport types
    -------------------------------------------------------------------
    #     Class  Trig  Silent Mon Plyr Orient    Dest

    195   Ext    SR    No     Yes Yes  Set       TP thing in tagged sector
    174   Ext    S1    No     Yes Yes  Set       TP thing in tagged sector
    97    Reg    WR    No     Yes Yes  Set       TP thing in tagged sector
    39    Reg    W1    No     Yes Yes  Set       TP thing in tagged sector

    126   Reg    WR    No     Yes No   Set       TP thing in tagged sector
    125   Reg    W1    No     Yes No   Set       TP thing in tagged sector
    269   Ext    WR    Yes    Yes No   Set       TP thing in tagged sector
    268   Ext    W1    Yes    Yes No   Set       TP thing in tagged sector

    210   Ext    SR    Yes    Yes Yes  Preserve  TP thing in tagged sector
    209   Ext    S1    Yes    Yes Yes  Preserve  TP thing in tagged sector
    208   Ext    WR    Yes    Yes Yes  Preserve  TP thing in tagged sector
    207   Ext    W1    Yes    Yes Yes  Preserve  TP thing in tagged sector

    244   Ext    WR    Yes    Yes Yes  Preserve  Line with same tag
    243   Ext    W1    Yes    Yes Yes  Preserve  Line with same tag
    263   Ext    WR    Yes    Yes Yes  Preserve  Line with same tag (reversed)
    262   Ext    W1    Yes    Yes Yes  Preserve  Line with same tag (reversed)

    267   Ext    WR    Yes    Yes No   Preserve  Line with same tag
    266   Ext    W1    Yes    Yes No   Preserve  Line with same tag
    265   Ext    WR    Yes    Yes No   Preserve  Line with same tag (reversed)
    264   Ext    W1    Yes    Yes No   Preserve  Line with same tag (reversed)

## Section 10 Donuts

A donut is a very specialized linedef type that lowers a pillar in a
surrounding pool, while raising the pool and changing its texture and
type.

The tagged sector is the pillar, and its lowest numbered line must be
two-sided. The sector on the other side of that is the pool. The pool
must have a two-sided line whose second sidedef does not adjoin the
pillar, and the sector on the second side of the lowest numbered such
linedef is the model for the pool's texture change. The model sector
floor also provides the destination height both for lowering the
pillar and raising the pool.

No generalized donut linedefs exist, and all are switched or
walkover.

### Section 10.1 Donut Linedef types

    Regular and Extended Donut types
    -------------------------------------------------------------------
    #     Class  Trig

    191   Ext    SR
    9     Reg    S1
    155   Ext    WR
    146   Ext    W1

## Section 11 Property Transfer

These linedefs are special purpose and are used to transfer
properties from the linedef itself or the sector on its first sidedef
to the tagged sector(s). None are triggered, they simply exist.

### Extended Property Transfer Linedefs

 #    Class  Trig Description

**213   Ext --   Set Tagged Floor Lighting to Lighting on 1st Sidedef's Sector**

Used to give the floor of a sector a different light level from the
remainder of the sector. For example bright lava in a dark room.

**261   Ext --   Set Tagged Ceiling Lighting to Lighting on 1st Sidedef's Sector**

Used to give the ceiling of a sector a different light level from the
remainder of the sector.

**260   Ext --   Make Tagged Lines (or this line if tag==0) Translucent**

Used to make 2s normal textures translucent. If tag==0, then this linedef's
normal texture is made translucent if it's 2s, and the default translucency
map TRANMAP is used as the filter. If tag!=0, then all similarly-tagged 2s
linedefs' normal textures are made translucent, and if this linedef's first
sidedef contains a valid lump name for its middle texture (as opposed to a
texture name), and the lump is 64K long, then that lump will be used as the
translucency filter instead of the default TRANMAP, allowing different
filters to be used in different parts of the same maps. If the first side's
normal texture is not a valid translucency filter lump name, it must be a
valid texture name, and will be displayed as such on this linedef.

**242   Ext --   Set Tagged Lighting, Flats Heights to 1st Sidedef's Sector,
               and set colormap based on sidedef textures.**

This allows the tagged sector to have two levels -- an actual floor and
ceiling, and another floor or ceiling where more flats are rendered. Things
will stand on the actual floor or hang from the actual ceiling, while this
function provides another rendered floor and ceiling at the heights of the
sector on the first sidedef of the linedef. Typical use is "deep water" that
can be over the player's head.

     ----------------------------------  < real sector's ceiling height
    |         real ceiling             | < control sector's ceiling texture
    |                                  |
    |                                  | < control sector's lightlevel
    |              A                   |
    |                                  | < upper texture as colormap
    |                                  |
    |                                  | < control sector's floor texture
     ----------------------------------  < control sector's ceiling height
    |         fake ceiling             | < real sector's ceiling texture
    |                                  |
    |                                  | < real sector's lightlevel
    |              B                   |
    |                                  | < normal texture as colormap
    |                                  |
    |          fake floor              | < real sector's floor texture
     ----------------------------------  < control sector's floor height
    |                                  | < control sector's ceiling texture
    |                                  |
    |                                  | < control sector's lightlevel
    |              C                   |
    |                                  | < lower texture as colormap
    |                                  |
    |          real floor              | < control sector's floor texture
     ----------------------------------  < real sector's floor height

Boom sectors controlled by a 242 linedef are partitioned into 3 spaces.
The viewer's xyz coordinates uniquely determine which space they are in.

If they are in space B (normal space), then the floor and ceiling textures
and lightlevel from the real sector are used, and the colormap from the 242
linedef's first sidedef's normal texture is used (COLORMAP is used if it's
invalid or missing). The floor and ceiling are rendered at the control
sector's heights.

If they are in space C ("underwater"), then the floor and ceiling textures
and lightlevel from the control sector are used, and the lower texture in
the 242 linedef's first sidedef is used as the colormap.

If they are in space A ("head over ceiling"), then the floor and ceiling
textures and lightlevel from the control sector are used, and the upper
texture in the 242 linedef's first sidedef is used as the colormap.

If only two of these adjacent partitions in z-space are used, such as
underwater and normal space, one has complete control over floor textures,
ceiling textures, light level, and colormaps, in each of the two partitions.
The control sector determines the textures and lighting in the more "unusual"
case (e.g. underwater).

It's also possible for the fake floor to extend below the real floor, in
which case an invisible platform / stair effect is created. In that case,
the picture looks like this (barring any ceiling effects too):

     ----------------------------------  < real sector's ceiling texture
    |   real ceiling = fake ceiling    |
    |                                  |
    |                                  |
    |              B                   | < real sector's lightlevel
    |                                  | < normal texture's colormap
    |                                  |
    |          real floor              |
     ----------------------------------  < invisible, no texture drawn
    |                                  |
    |                                  |
    |                                  | < real sector's lightlevel
    |              C                   | < normal texture's colormap
    |                                  |
    |                                  |
    |          fake floor              | < real sector's floor texture
     ----------------------------------  < fake sector's floor height

In this case, since the viewer is always at or above the fake floor, no
colormap/lighting/texture changes occur -- the fake floor just gets drawn
at the control sector's height, but at the real sector's lighting and
texture, while objects stand on the higher height of the real floor.

It's the viewer's position relative to the fake floor and/or fake ceiling,
which determines whether the control sector's lighting and textures should
be used, and which colormap should be used. If the viewer is always between
the fake floor and fake ceiling, then no colormap, lighting, or texture
changes occur, and the view just sees the real sector's textures and light
level drawn at possibly different heights.

If the viewer is below the fake floor height set by the control sector, or is
above the fake ceiling height set by the control sector, then the corresponding
colormap is used (lower or upper texture name), and the textures and lighting
are taken from the control sector rather than the real sector. They are still
stacked vertically in standard order -- the control sector's ceiling is always
drawn above the viewer, and the control sector's floor is always drawn below
the viewer.

The kaleidescope effect only occurs when F_SKY1 is used as the control sector's
floor or ceiling. If F_SKY1 is used as the control sector's ceiling texture,
then under water, only the control sector's floor appears, but it "envelops"
the viewer. Similarly, if F_SKY1 is used as the control sector's floor texture,
then when the player's head is over a fake ceiling, the control sector's
ceiling is used throughout.

F_SKY1 causes HOM when used as a fake ceiling between the viewer and normal
space. Since there is no other good use for it, this kaleidescope is an
option turned on by F_SKY1. Note that this does not preclude the use of sky
REAL ceilings over deep water -- this is the control sector's ceiling, the
one displayed when the viewer is underwater, not the real one.

A colormap has the same size and format as Doom's COLORMAP. Extra colormaps
may defined in Boom by adding them between C_START and C_END markers in wads.
Colormaps between C_START and C_END are automatically merged by Boom with any
previously defined colormaps.

WATERMAP is a colormap predefined by Boom which can be used to provide a
blue-green tint while the player is under water. WATERMAP can be modified by
pwads.

Ceiling bleeding may occur if required upper textures are not used.

**223   Ext --   Length Sets Friction in tagged Sector,Sludge<100, Ice>100**

The length of the linedef with type 223 controls the amount of friction
experienced by the player in the tagged sector, when in contact with the
floor. Lengths less than 100 are sticker than normal, lengths greater than
100 are slipperier than normal. The effect is only present in the tagged
sector when its friction enable bit (bit 8) in the sector type is set.
This allows the flat to be changed in conjunction with turning the effect
on or off thru texture/type changes.


### Constant pushers

Two types of constant pushers are available, wind and current. Depending
on whether you are above, on, or below (special water sectors) the ground
level, the amount of force varies.

The length of the linedef defines the 'full' magnitude of the force, and
the linedef's angle defines the direction.

     line type         above  on   under
     ---------         -----  --   -----
     wind       224    full  half  none
     current    225    none  full  full

The linedef should be tagged to the sector where you want the effect. The
special type of the sector should have bit 9 set (0x200). If this bit
is turned off, the effect goes away. For example, a fan creating a wind
could be turned off, and the wind dies, by changing the sector type and
clearing that bit.

Constant pushers can be combined with scrolling effects and point
pushers.

**224  Ext  --   Length/Direction Sets Wind Force in tagged Sectors**

**225  Ext  --   Length/Direction Sets Current Force in tagged Sectors**

### Point pushers

Two types of point pushers are available, push and pull.

The previous implementation of these was SECTOR-SPECIFIC. The new
implementation ignores sector boundaries and provides the effect in a
circular area whose center is defined by a Thing of type 5001 (push)
or 5002 (pull). You now also don't have to set any option flags on these
Things. A new linedef type of 226 is used to control the effect, and this
line should be tagged to the sector with the 5001/5002 Thing.

The length of the linedef defines the 'full' magnitude of the force, and
the force is inversely proportional to distance from the point source. If
the length of the controlling linedef is L, then the force is reduced to
zero at a distance of 2L.

The angle of the controlling linedef is not used.

The sector where the 5001/5002 Things reside must have bit 9 set (0x200)
in its type. If this is turned off, the effect goes away.

Point pushers can be combined with scrolling effects and constant
pushers.

**226  Ext  --   Length Sets Point Source Wind/Current Force in Tagged Sectors**

## Section 12 Scrolling Walls, Flats, Objects

### Static Scrollers

The most basic kind of scrolling linedef causes some part of the tagged
sector or tagged wall to scroll, in proportion to the length of the
linedef the trigger is on, and according to the direction that trigger
linedef lies. For each 32 units of length of the trigger, the tagged
object scrolls 1 unit per frame. Since the length of a linedef doesn't
change during gameplay, these types are static, the scrolling effect
remains constant during gameplay. However, these effects can be combined
with, and/or canceled by, other scrollers.


**250    -- Scroll Tagged Ceiling**

The ceiling of the tagged sector scrolls in the direction of the linedef
trigger, 1 unit per frame for each 32 units of linedef trigger length.
Objects attached to the ceiling do not move.

**251    -- Scroll Tagged Floor**

The floor of the tagged sector scrolls in the direction of the linedef
trigger, 1 unit per frame for each 32 units of linedef trigger length.
Objects resting on the floor do not move.

**252    -- Carry Objects on Tagged Floor**

Objects on the floor of the tagged sector move in the direction of the
linedef trigger, 1 unit per frame for each 32 units of linedef trigger
length. The floor itself does not scroll.

**253    -- Scroll Tagged Floor, Carry Objects**

Both the floor of the tagged sector and objects resting on that floor
move in the direction of the linedef trigger, 1 unit per frame for each
32 units of linedef trigger length.

**254    -- Scroll Tagged Wall, Same as Floor/Ceiling**

Walls with the same tag as the linedef trigger scroll at the same
rate as a floor or ceiling controlled by one of 250-253, allowing
their motion to be synchronized. When the linedef trigger is not
parallel to the wall, the component of the linedef in the direction
perpendicular to the wall causes the wall to scroll vertically. The
length of the component parallel to the wall sets the horizontal
scroll rate, the length of the component perpendicular to the wall
sets the vertical scroll rate.

### Simple Static Scrollers

For convenience three simpler static scroll functions are provided
for when you just need a wall to scroll and don't need to control its
rate and don't want to bother with proportionality.

**255    -- Scroll Wall Using Sidedef Offsets**

For simplicity, a static scroller is provided that scrolls the first
sidedef of a linedef, based on its x- and y- offsets. No tag is used.
The x offset controls the rate of horizontal scrolling, 1 unit per
frame per x offset, and the y offset controls the rate of vertical
scrolling, 1 unit per frame per y offset.

**48     -- Animated wall, Scrolls Left**

A linedef with this type scrolls its first sidedef left at a
constant rate of 1 unit per frame.

**85     -- Animated wall, Scrolls Right**

A linedef with this type scrolls its first sidedef right at a
constant rate of 1 unit per frame.

### Dynamic Scrolling

To achieve dynamic scrolling effects, the position or rate of scrolling
is controlled by the relative position of the floor and ceiling of the
sector on the scrolling trigger's first sidedef. The direction of
scrolling remains controlled by the direction of the linedef trigger.
Either the floor or ceiling may move in the controlling sector, or
both. The control variable is the amount of change in the sum of the
floor and ceiling heights.

All scroll effects are additive, and thus two or more effects may
reinforce and/or cancel each other.

### Displacement Scrollers

In the first kind, displacement scrolling, the position of the scrolled
objects or walls changes proportionally to the motion of the floor or
ceiling in the sector on the first sidedef of the scrolling trigger.
The proportionality is set by the length of the linedef trigger. If it
is 32 units long, the wall, floor, ceiling, or object moves 1 unit for
each unit the floor or ceiling of the controlling sector moves. If it
is 64 long, they move 2 units per unit of relative floor/ceiling motion
in the controlling sector and so on.

**245    -- Scroll Tagged Ceiling w.r.t. 1st Side's Sector**

The tagged sector's ceiling texture scrolls in the direction of the
scrolling trigger line, when the sector on the trigger's first sidedef
changes height. The amount moved is the height change times the trigger
length, divided by 32. Objects attached to the ceiling do not move.

**246    -- Scroll Tagged Floor w.r.t. 1st Side's Sector**

The tagged sector's floor texture scrolls in the direction of the
scrolling trigger line when the sector on the trigger's first sidedef
changes height. The amount moved is the height change times the trigger
length, divided by 32. Objects on the floor do not move.

**247    -- Push Objects on Tagged Floor wrt 1st Side's Sector**

Objects on the tagged sector's floor move in the direction of the
scrolling trigger line when the sector on the trigger's first sidedef
changes height. The amount moved is the height change times the trigger
length, divided by 32.

**248    -- Push Objects & Tagged Floor wrt 1st Side's Sector**

The tagged sector's floor texture, and objects on the floor, move in
the direction of the scrolling trigger line when the sector on the
trigger's first sidedef changes height.  The amount moved is the height
change times the trigger length, divided by 32.

**249    -- Scroll Tagged Wall w.r.t 1st Side's Sector**

Walls with the same tag as the linedef trigger scroll at the same
rate as a floor or ceiling controlled by one of 245-249, allowing
their motion to be synchronized. When the linedef trigger is not
parallel to the wall, the component of the linedef in the direction
perpendicular to the wall causes the wall to scroll vertically. The
length of the component parallel to the wall sets the horizontal
scroll displacement, the length of the component perpendicular to
the wall sets the vertical scroll displacement. The distance scrolled
is the controlling sector's height change times the trigger length,
divided by 32.

### Accelerative Scrollers

The second kind of dynamic scrollers, accelerative scrollers, also
react to height changes in the sector on the first sidedef of the
linedef trigger, but the RATE of scrolling changes, not the
displacement. That is, changing the controlling sector's height
speeds up or slows down the scrolling by the change in height
times the trigger's length, divided by 32.

An on/off scroller can be made by using an accelerative scroller
and any linedef that changes the controlling sector's heights.
If a scroll effect which is initially on is desired, then the
accelerative scroller should be combined with a static one which
turns the scroll effect on initially. The accelerative scroller
would then need to be set up to cancel the static scroller's
effect when the controlling sector's height changes.

**214    -- Accel Tagged Ceiling w.r.t. 1st Side's Sector**

The tagged sector's ceiling's rate of scroll changes in the direction
of the trigger linedef (use vector addition if already scrolling) by
the change in height of the sector on the trigger's first sidedef
times the length of the linedef trigger, divided by 32. For example,
if the ceiling is motionless originally, the linedef trigger is 32
long, and the ceiling of the controlling sector moves 1 unit, the
tagged ceiling will start scrolling at 1 unit per frame.

**215    -- Accel Tagged Floor w.r.t. 1st Side's Sector**

The tagged sector's floor's rate of scroll changes in the direction of
the trigger linedef (use vector addition if already scrolling) by the
change in height of the sector on the trigger's first sidedef times
the length of the linedef trigger, divided by 32. For example, if
the floor is motionless originally, the linedef trigger is 32 long,
and the ceiling of the controlling sector moves 1 unit, the tagged
floor will start scrolling at 1 unit per frame.

**216    -- Accel Objects on Tagged Floor wrt 1st Side's Sector**

Objects on the tagged sector's floor's rate of motion changes in the
direction of the trigger linedef (use vector addition if already
moving) by the change in height of the sector on the trigger's first
sidedef times the length of the linedef trigger divided by 32. For
example, if the objects are motionless originally, the linedef trigger
is 32 long, and the ceiling of the controlling sector moves 1 unit,
the objects on the tagged floor will start moving at 1 unit per frame.

**217    -- Accel Objects&Tagged Floor wrt 1st Side's Sector**

The tagged sector's floor, and objects on it, change its rate of
motion in the direction of the trigger linedef (use vector addition
if already moving) by the change in height of the sector on the
trigger's first sidedef times the length of the linedef trigger, divided
by 32. For example, if the floor and objects are motionless originally,
the linedef trigger is 32 long, and the ceiling of the controlling
sector moves 1 unit, the objects and the tagged floor will start moving
at 1 unit per frame.

**218    -- Accel Tagged Wall w.r.t 1st Side's Sector**

Walls with the same tag as the linedef trigger increase their
scroll rate in sync with a floor or ceiling controlled by one of
214-217. When the linedef trigger is not parallel to the wall, the
component of the linedef in the direction perpendicular to the wall
causes the wall to increase its vertical scroll rate. The length of
the component parallel to the wall sets the change in horizontal
scroll rate, the length of the component perpendicular to the wall
sets the change in vertical scroll rate. The rate change is the height
change times the trigger length, divided by 32.

## Section 13 Detailed Generalized Linedef Specification

BOOM has added generalized linedef types that permit the parameters
of linedef actions to be nearly independently chosen.

To support these, DETH has been modified to support them via dialogs
for each generalized type; each dialog allows the parameters for
that type to be independently specified by choice from a array of
radio buttons. A parser has also been added to support reading back
the function of a generalized linedef from its type number.

NOTE to wad authors:

This requires a lot of type numbers, 20608 in all so far. Some
editors may object to the presence of these new types thru assuming
that a lookup table of size 256 suffices (or some other reason). For
those that must continue to use such an editor, it may be necessary
to stick to the old linedef types, which are still present.

We are also providing command line tools to set these linedef types,
independent of which editor you happen to use, but they are a lot
less slick, and more difficult to use than DETH. See TRIGCALC.EXE and
CLED.EXE in the EDITUTIL.ZIP package of the BOOM distribution.

### Generalized Linedef Ranges

There are types for Floors, Ceilings, Doors, Locked Doors, Lifts,
Stairs, and Crushers. The allocation of linedef type field numbers is
according to the following table:

    Type            Start      Length   (Dec)
    -----------------------------------------------------------------
    Floors          0x6000     0x2000  (8192)
    Ceilings        0x4000     0x2000  (8192)
    Doors           0x3c00     0x0400  (1024)
    Locked Doors    0x3800     0x0400  (1024)
    Lifts           0x3400     0x0400  (1024)
    Stairs          0x3000     0x0400  (1024)
    Crushers        0x2F80     0x0080   (128)
    -----------------------------------------------------------------
    Totals:  0x2f80-0x7fff     0x5080  (20608)

The following sections define the placement and meaning of the bit
fields within each linedef category. Fields in the description are
listed in increasing numeric order.

Some nomenclature:

Target height designations:

H means highest, L means lowest, N means next, F means floor, C means
ceiling, n means neighbor, Cr means crush, sT means shortest
lower texture.

Texture change designations:

c0n  change texture, change sector type to 0, numeric model change
c0t  change texture, change sector type to 0, trigger model change
cTn  change texture only, numeric model change
cTt  change texture only, trigger model change
cSn  change texture and sector type to model's, numeric model change
cSt  change texture and sector type to model's, trigger model change

A trigger model change uses the sector on the first side of the
trigger for its model. A numeric model change looks at the sectors
adjoining the tagged sector at the target height, and chooses the one
across the lowest numbered two sided line for its model. If no model
exists, no change occurs. Note that in DOOM II v1.9, no model meant
an illegal sector type was generated.


#### Generalized floors (8192 types)

    field   description                       NBits   Mask     Shift
    ------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3     0x0007      0
    speed   slow/normal/fast/turbo             2     0x0018      3
    model   trig/numeric -or- nomonst/monst    1     0x0020      5
    direct  down/up                            1     0x0040      6
    target  HnF/LnF/NnF/LnC/C/sT/24/32         3     0x0380      7
    change  nochg/zero/txtonly/type            2     0x0c00      10
    crush   no/yes                             1     0x1000      12

    DETH Nomenclature:

    W1[m] F->HnF DnS [c0t] [Cr]
    WR[m] F->LnF DnN [c0n]
    S1[m] F->NnF DnF [cTt]
    SR[m] F->LnC DnT [cTn]
    G1[m] F->C   UpS [cSt]
    GR[m] FbysT  UpN [cSn]
    D1[m] Fby24  UpF
    DR[m] Fby32  UpT

Notes:

 1. When change is nochg, model is 1 when monsters can activate trigger
    otherwise monsters cannot activate it.
 2. The change fields mean the following:
    nochg   - means no texture change or type change
    zero    - means sector type is zeroed, texture copied from model
    txtonly - means sector type unchanged, texture copied from model
    type    - means sector type and floor texture are copied from model
 3. down/up specifies the "normal" direction for moving. If the
    target specifies motion in the opposite direction, motion is instant.
    Otherwise it occurs at speed specified by speed field.
 4. Speed is 1/2/4/8 units per tic
 5. If change is nonzero then model determines which sector is copied.
    If model is 0 its the sector on the first side of the trigger.
    if model is 1 (numeric) then the model sector is the sector at
    destination height on the opposite side of the lowest numbered
    two sided linedef around the tagged sector. If it doesn't exist
    no change occurs.

#### Generalized ceilings (8192 types)

    field   description                        NBits   Mask     Shift
    ------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR             3     0x0007      0
    speed   slow/normal/fast/turbo              2     0x0018      3
    model   trig/numeric -or- nomonst/monst     1     0x0020      5
    direct  down/up                             1     0x0040      6
    target  HnC/LnC/NnC/HnF/F/sT/24/32          3     0x0380      7
    change  nochg/zero/txtonly/type             2     0x0c00      10
    crush   no/yes                              1     0x1000      12

    DETH Nomenclature:

    W1[m] C->HnC DnS [Cr] [c0t]
    WR[m] C->LnC DnN      [c0n]
    S1[m] C->NnC DnF      [cTt]
    SR[m] C->HnF DnT      [cTn]
    G1[m] C->F   UpS      [cSt]
    GR[m] CbysT  UpN      [cSn]
    D1[m] Cby24  UpF
    DR[m] Cby32  UpT

Notes:

 1. When change is nochg, model is 1 when monsters can activate trigger
    otherwise monsters cannot activate it.
 2. The change fields mean the following:
    nochg   - means no texture change or type change
    zero    - means sector type is zeroed, texture copied from model
    txtonly - means sector type unchanged, texture copied from model
    type    - means sector type and ceiling texture are copied from model
 3. down/up specifies the "normal" direction for moving. If the
    target specifies motion in the opposite direction, motion is instant.
    Otherwise it occurs at speed specified by speed field.
 4. Speed is 1/2/4/8 units per tic
 5. If change is nonzero then model determines which sector is copied.
    If model is 0 its the sector on the first side of the trigger.
    if model is 1 (numeric) then the model sector is the sector at
    destination height on the opposite side of the lowest numbered
    two sided linedef around the tagged sector. If it doesn't exist
    no change occurs.

#### Generalized doors (1024 types)

    field   description                       NBits    Mask     Shift
    ------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3      0x0007      0
    speed   slow/normal/fast/turbo             2      0x0018      3
    kind    odc/o/cdo/c                        2      0x0060      5
    monster n/y                                1      0x0080      7
    delay   1/4/9/30 (secs)                    2      0x0300      8

    DETH Nomenclature:

    W1[m] OpnD{1|4|9|30}Cls S
    WR[m] Opn               N
    S1[m] ClsD{1|4|9|30}Opn F
    SR[m] Cls               T
    G1[m]
    GR[m]
    D1[m]
    DR[m]

Notes:

 1. The odc (Open, Delay, Close) and cdo (Close, Delay, Open) kinds use
    the delay field. The o (Open and Stay) and c (Close and Stay) kinds
    do not.
 2. The precise delay timings in gametics are: 35/150/300/1050
 3. Speed is 2/4/8/16 units per tic

#### Generalized locked doors (1024 types)

    field   description                       NBits    Mask     Shift
    ------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3      0x0007      0
    speed   slow/normal/fast/turbo             2      0x0018      3
    kind    odc/o                              1      0x0020      5
    lock    any/rc/bc/yc/rs/bs/ys/all          3      0x01c0      6
    sk=ck   n/y                                1      0x0200      9

    DETH Nomenclature:

    W1[m] OpnDCls           S Any
    WR[m] Opn               N R{C|S|K}
    S1[m]                   F B{C|S|K}
    SR[m]                   T Y{C|S|K}
    G1[m]                     All{3|6}
    GR[m]
    D1[m]
    DR[m]

Notes:

 1. Delay for odc kind is constant at 150 gametics or about 4.333 secs
 2. The lock field allows any key to open a door, or a specific key to
    open a door, or all keys to open a door.
 3. If the sk=ck field is 0 (n) skull and cards are different keys,
    otherwise they are treated identically. Hence an "all" type door
    requires 3 keys if sk=ck is 1, and 6 keys if sk=ck is 0.
 4. Speed is 2/4/8/16 units per tic

#### Generalized lifts (1024 types)

    field   description                       NBits    Mask     Shift
    -------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3      0x0007      0
    speed   slow/normal/fast/turbo             2      0x0018      3
    monster n/y                                1      0x0020      5
    delay   1/3/5/10 (secs)                    2      0x00c0      6
    target  LnF/NnF/LnC/LnF<->HnF(perp.)       2      0x0300      8

    DETH Nomenclature:

    W1[m] Lft  F->LnFD{1|3|5|10}    S
    WR[m]      F->NnFD{1|3|5|10}    N
    S1[m]      F->LnCD{1|3|5|10}    F
    SR[m]      HnF<->LnFD{1|3|5|10} T
    G1[m]
    GR[m]
    D1[m]
    DR[m]

Notes:

 1. The precise delay timings in gametics are: 35/105/165/350
 2. Speed is 1/2/4/8 units per tic
 3. If the target specified is above starting floor height, or does not
    exist the lift does not move when triggered. NnF is Next Lowest
    Neighbor Floor.
 4. Starting a perpetual lift between lowest and highest neighboring floors
    locks out all other floor actions on the sector, even if it is stopped
    with the non-extended stop perpetual floor function.

#### Generalized stairs (1024 types)

    field   description                       NBits    Mask     Shift
    -------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3      0x0007      0
    speed   slow/normal/fast/turbo             2      0x0018      3
    monster n/y                                1      0x0020      5
    step    4/8/16/24                          2      0x00c0      6
    dir     dn/up                              1      0x0100      8
    igntxt  n/y                                1      0x0200      9

    DETH Nomenclature:

    W1[m] Stair Dn s4  S [Ign]
    WR[m]       Up s8  N
    S1[m]          s16 F
    SR[m]          s24 T
    G1[m]
    GR[m]
    D1[m]
    DR[m]

Notes:

 1. Speed is .25/.5/2/4 units per tic
 2. If igntxt is 1, then the staircase will not stop building when
    a step does not have the same texture as the previous.
 3. A retriggerable stairs builds up and down alternately on each
    trigger.

#### Generalized crushers (128 types)

    field   description                       NBits    Mask     Shift
    -------------------------------------------------------------------

    trigger W1/WR/S1/SR/G1/GR/D1/DR            3      0x0007      0
    speed   slow/normal/fast/turbo             2      0x0018      3
    monster n/y                                1      0x0020      5
    silent  n/y                                1      0x0040      6

    DETH Nomenclature:

    W1[m] Crusher S [Silent]
    WR[m]         N
    S1[m]         F
    SR[m]         T
    G1[m]
    GR[m]
    D1[m]
    DR[m]

Notes:

 1. Speed is 1/2/4/8 units per second, faster means slower damage as usual.
 2. If silent is 1, the crusher is totally quiet, no start/stop sounds

# Part II Sector Types

BOOM is backward compatible with DOOM's sector types. All types 0-17
have the same meaning they did under DOOM. Types 18-31 are reserved
for extended sector types, that work like DOOM's and share the
limitation that only one type can be active in a sector at once. No
extended sector types are implemented at this time.

## Section 14. Regular Sector types

From Matt Fell's Unofficial DOOM Spec the DOOM sector types are:

    Dec Hex Class   Description
    -------------------------------------------------------------------
     0  00  -       Normal, no special characteristic.
     1  01  Light   random off
     2  02  Light   blink 0.5 second
     3  03  Light   blink 1.0 second
     4  04  Both    -10/20% health AND light blink 0.5 second
     5  05  Damage  -5/10% health
     7  07  Damage  -2/5% health
     8  08  Light   oscillates
     9  09  Secret  a player must stand in this sector to get credit for
                      finding this secret. This is for the SECRETS ratio
                      on inter-level screens.
    10  0a  Door    30 seconds after level start, ceiling closes like a door.
    11  0b  End     -10/20% health. If a player's health is lowered to less
                      than 11% while standing here, then the level ends! Play
                      proceeds to the next level. If it is a final level
                      (levels 8 in DOOM 1, level 30 in DOOM 2), the game ends!
    12  0c  Light   blink 0.5 second, synchronized
    13  0d  Light   blink 1.0 second, synchronized
    14  0e  Door    300 seconds after level start, ceiling opens like a door.
    16  10  Damage  -10/20% health

    17  11  Light   flickers on and off randomly

Note that BOOM will NEVER exit the game on an illegal sector type, as
was the case with DOOM. The sector type will merely be ignored.

## Section 15. Generalized sector types

BOOM also provides generalized sector types, based on bit fields,
that allow several sector type properties to be independently
specified for a sector. Texture change linedefs can be used to switch
some or all of these properties dynamically, outside lighting.

Bits 0 thru 4 specify the lighting type in the sector, the same codes
that DOOM used are employed:

    Dec Bits 4-0   Description
    -------------------------------------------------------------------
    0   00000      Normal lighting
    1   00001      random off
    2   00010      blink 0.5 second
    3   00011      blink 1.0 second
    4   00100      -10/20% health AND light blink 0.5 second
    8   01000      oscillates
    12  01100      blink 0.5 second, synchronized
    13  01101      blink 1.0 second, synchronized
    17  10001      flickers on and off randomly

Bits 5 and 6 set the damage type of the sector, with the usual
5/10/20 damage units per second.

    Dec Bits 6-5   Description
    -------------------------------------------------------------------
    0   00         No damage
    32  01         5  units damage per sec (halve damage in TYTD skill)
    64  10         10 units damage per sec
    96  11         20 units damage per sec

Bit 7 makes the sector count towards the secrets total at game end

    Dec Bit 7      Description
    -------------------------------------------------------------------
    0    0         Sector is not secret
    128  1         Sector is secret

Bit 8 enables the ice/mud effect controlled by linedef 223

    Dec Bit 8      Description
    -------------------------------------------------------------------
    0    0         Sector friction disabled
    256  1         Sector friction enabled

Bit 9 enables the wind effects controlled by linedefs 224-226

    Dec Bit 9      Description
    -------------------------------------------------------------------
    0    0         Sector wind disabled
    512  1         Sector wind enabled

Bits 10 and 11 are not implemented yet, but are reserved for use in
sound control. Bit 10 will suppress all sounds within the sector,
while bit 11 will disable any sounds due to floor or ceiling motion
by the sector.

# Part III Linedef Flags

Only one new linedef flag is implemented, called PassThru, which
allows a push or switch linedef trigger to pass the push action thru
it to ones within range behind it. In this way BOOM is capable of
setting off many actions with a single push. Note that the limitation
of one floor action, one ceiling action, and one lighting action per
sector affected still applies however.

The new linedef flag is bit 9, value 512, in the linedef flags word.

# Part IV Thing Flags

BOOM has implemented two new thing flags, "not in DM" and "not in
COOP", which in combination with the existing "not in Single" flag,
usually called "Multiplayer", allow complete control over a level's
inventory and monsters, in any game mode.

If you want a thing to be only available in Single play, you set both
the "not in DM" and "not in COOP" flags. Other combinations are
similar.

 * "not in DM"   is bit 5, value 32, in the thing flags word.
 * "not in COOP" is bit 6, value 64, in the thing flags word.

# Part V Thing Types

BOOM has implemented two new thing types as well, MT_PUSH (5001), and
MT_PULL(5002). These control the origin of the point source wind
effect controlled by linedef types 225 and 226, and set whether the
wind blows towards or away from the origin depending on which is
used.

# Part VI BOOM Predefined Lumps

BOOM supports resources embedded in the .EXE that are loaded before
all other resources. They can be replaced by resources of the same
name in the IWAD or add-on PWADs. These resources can be extracted
into a wad thru use of the command:

BOOM -dumplumps predef.wad

You can then extract the resources from predef.wad to alter them for
replacement purposes.

## Menu text

    M_HORSEN       Mouse horizontal sensitivity menu
    M_VERSEN       Mouse vertical sensitivity menu
    M_SETUP        Setup menu entry in main menu
    M_KEYBND       Key Bindings entry in setup menu
    M_AUTO         Automap entry in setup menu
    M_CHAT         Chat string entry in setup menu
    M_ENEM         Enemies options entry in setup menu
    M_STAT         Status bar and Hud entry in setup menu
    M_WEAP         Weapons entry in setup menu
    M_MESS         Message option entry in setup menu
    M_COLORS       Palette color picker diagram
    M_PALNO        Xed-out symbol for unused automap features
    M_BUTT1        Setup reset button off state
    M_BUTT2        Setup reset button on state

## Font chars

    STCFN096       ` character for standard DOOM font

    STBR123        HUD font bargraph char, full, 4 vertical bars
    STBR124        HUD font bargraph char, partial, 3 vertical bars
    STBR125        HUD font bargraph char, partial, 2 vertical bars
    STBR126        HUD font bargraph char, partial, 1 vertical bars
    STBR127        HUD font bargraph char, empty, 0 vertical bars

    DIG0           HUD font 0 char
    DIG1           HUD font 1 char
    DIG2           HUD font 2 char
    DIG3           HUD font 3 char
    DIG4           HUD font 4 char
    DIG5           HUD font 5 char
    DIG6           HUD font 6 char
    DIG7           HUD font 7 char
    DIG8           HUD font 8 char
    DIG9           HUD font 9 char
    DIGA           HUD font A char
    DIGB           HUD font B char
    DIGC           HUD font C char
    DIGD           HUD font D char
    DIGE           HUD font E char
    DIGF           HUD font F char
    DIGG           HUD font G char
    DIGH           HUD font H char
    DIGI           HUD font I char
    DIGJ           HUD font J char
    DIGK           HUD font K char
    DIGL           HUD font L char
    DIGM           HUD font M char
    DIGN           HUD font N char
    DIGO           HUD font O char
    DIGP           HUD font P char
    DIGQ           HUD font Q char
    DIGR           HUD font R char
    DIGS           HUD font S char
    DIGT           HUD font T char
    DIGU           HUD font U char
    DIGV           HUD font V char
    DIGW           HUD font W char
    DIGX           HUD font X char
    DIGY           HUD font Y char
    DIGZ           HUD font Z char
    DIG45          HUD font - char
    DIG47          HUD font / char
    DIG58          HUD font : char
    DIG91          HUD font [ char
    DIG93          HUD font ] char

## Status bar

    STKEYS6        Both blue keys graphic
    STKEYS7        Both yellow keys graphic
    STKEYS8        Both red keys graphic

## Box chars

    BOXUL          Upper left corner of box
    BOXUC          Center of upper border of box
    BOXUR          Upper right corner of box
    BOXCL          Center of left border of box
    BOXCC          Center of box
    BOXCR          Center of right border of box
    BOXLL          Lower left corner of box
    BOXLC          Center of lower border of box
    BOXLR          Lower right corner of box

## Lumps missing in v1.1

    STTMINUS       minus sign for neg frags in status bar
    WIMINUS        minus sign for neg frags on end screen
    M_NMARE        nightmare skill menu string
    STBAR          status bar background
    DSGETPOW       sound for obtaining power-up

## Text end screen

    ENDBOOM        Text displayed at end of game, 80X25 with attributes

## Sprites

    TNT1A0         Invisible sprite for push/pull controller things

## Switches and Animations

    SWITCHES       Definition of switch textures recognized
    ANIMATED       Definition of animated textures and flats recognized

## SWITCHES format:

The SWITCHES lump makes the names of the switch textures used in the
game known to the engine. It consists of a list of records each 20
bytes long, terminated by an entry (not used) whose last 2 bytes are 0.

    9 bytes        Null terminated string naming "off" state texture for switch
    9 bytes        Null terminated string naming "on" state texture for switch
    2 bytes        Short integer for the IWADs switch will work in
                    1=shareware
                    2=registered/retail DOOM or shareware
                    3=DOOM II, registered/retail DOOM, or shareware
                    0=terminates list of known switches

## ANIMATED format:

The ANIMATED lump makes the names of the animated flats and textures
known to the engine. It consists of a list of records, each 23 bytes
long, terminated by a record (not used) whose first byte is -1 (255).

    1 byte         -1 to terminate list, 0 if a flat, 1 if a texture
    9 bytes        Null terminated string naming last texture/flat in animation
    9 bytes        Null terminated string naming first texture/flat in animation
    4 bytes        Animation speed, number of frames between animation changes

The utility package contains a program, SWANTBLS.EXE that will
generate both the SWITCHES and ANIMATED lumps from a text file. An
example is provided, DEFSWANI.DAT that generates the standard set of
switches and animations for DOOM.

## Colormaps

    WATERMAP       Custom greenish colormap for underwater use

The WATERMAP lump has the same format as COLORMAP, 34 tables of 256
bytes each, see the UDS for details. Note that colormaps you add to
a PWAD must be between C_START and C_END markers.

The utility package contains a program, CMAPTOOL.EXE, that will
generate custom colormaps for use with BOOM. It also includes 7
premade colormaps, and an add-on wad containing them.


## Color translation tables

    CRBRICK        Translates red range to brick red
    CRTAN          Translates red range to tan
    CRGRAY         Translates red range to gray
    CRGREEN        Translates red range to green
    CRBROWN        Translates red range to brown
    CRGOLD         Translates red range to dark yellow
    CRRED          Translates red range to red range (no change)
    CRBLUE         Translates red range to light blue
    CRBLUE2        Translates red range to dark blue (status bar numerals)
    CRORANGE       Translates red range to orange
    CRYELLOW       Translates red range to light yellow

These tables are used to translate the red font characters to other
colors. They are made replaceable in order to support custom palettes
in TC's better. Note however that the font character graphics must
be defined using the palette indices for the standard red range of the
palette 176-191.
