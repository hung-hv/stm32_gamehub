/*
 * mario_map.c
 *
 *  Part of: Drivers/Game_Engine — World 1-1 tilemap data.
 *
 *  Map layout: 15 rows (top→bottom) x 60 columns (left→right).
 *  Each tile is 16x16 pixels → screen coverage 960 x 240 px.
 *
 *  Tile IDs:
 *    0 = TILE_EMPTY    (Sky / Air)
 *    1 = TILE_GROUND   (Dirt / Soil)
 *    2 = TILE_BRICK    (Breakable brown brick)
 *    3 = TILE_QUESTION (Mystery item box)
 *    4 = TILE_BLOCK    (Solid grey stone)
 *    5 = TILE_PIPE_TL  (Pipe top-left  lip)
 *    6 = TILE_PIPE_TR  (Pipe top-right lip)
 *    7 = TILE_PIPE_BL  (Pipe body left)
 *    8 = TILE_PIPE_BR  (Pipe body right)
 *
 *  World 1-1 feature summary (column ranges) — 90 columns total:
 *  --- Original section (cols 0-59) ---
 *    Cols 10-11 : Pipe #1  – 2 tiles tall (rows 11-13)
 *    Cols 14-17 : Brick/Question platform (row 8)  B B ? B
 *    Cols 20-21 : Pipe #2  – 3 tiles tall (rows 10-13)
 *    Cols 25-27 : Brick/Question platform (row 8)  ? B ?
 *    Cols 30-31 : Pipe #3  – 4 tiles tall (rows  9-13)
 *    Cols 34-38 : Upper brick platform (row 4)
 *    Cols 36-38 : Pit #1 (rows 13-14 empty)
 *    Cols 48-49 : Pit #2 (rows 13-14 empty)
 *    Cols 52-55 : End staircase (BLOCK tiles, rows 9-12)
 *  --- Extended section (cols 60-89) ---
 *    Cols 62-64 : Brick/Question platform (row 8)  B ? B
 *    Cols 68-69 : Pipe #4  – 2 tiles tall (rows 11-13)
 *    Cols 74-75 : Pit #3 (rows 13-14 empty)
 *    Cols 82-85 : Final staircase (BLOCK tiles, rows 9-12)
 */

#include "mario_map.h"
#include "st7789_map.h"

/* Each inner brace is one row of 90 tile IDs.
 * Values are grouped in blocks of 10 (with a space) for readability.
 * Column index comments mark every 10th position.               */
/*                    col: 00-09                10-19                20-29                30-39                40-49                50-59                60-69                70-79                80-89  */
uint8_t Mario_World_1_1[15][90] = {

    /* Row 0 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 1 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 2 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 3 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 4 – upper brick platform (cols 34-38) */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,2,2,2,2,2,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 5 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 6 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 7 – sky */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 8 – brick/question platforms
     *   cols 14=B 15=B 16=? 17=B  |  cols 25=? 26=B 27=?  |  cols 62=B 63=? 64=B  */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,2,2,3,2,0,0,  0,0,0,0,0,3,2,3,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,2,3,2,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0},

    /* Row 9 – pipe #3 top (cols 30-31), staircase step 4 (col 55), final stair col 85 */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  5,6,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,4,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,4,0,0,0,0},

    /* Row 10 – pipe #2 top (cols 20-21), pipe #3 body (cols 30-31),
     *          staircase step 3 (cols 54-55), final stair (cols 84-85)                */
    {0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  5,6,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,4,4,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,4,4,0,0,0,0},

    /* Row 11 – pipe #1 top (cols 10-11), pipe #2 body (cols 20-21),
     *          pipe #3 body (cols 30-31), staircase step 2 (cols 53-55),
     *          pipe #4 top (cols 68-69), final stair (cols 83-85)           */
    {0,0,0,0,0,0,0,0,0,0,  5,6,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,4,4,4,0,0,0,0,  0,0,0,0,0,0,0,0,5,6,  0,0,0,0,0,0,0,0,0,0,  0,0,0,4,4,4,0,0,0,0},

    /* Row 12 – pipe #1 body (cols 10-11), pipe #2 body (cols 20-21),
     *          pipe #3 body (cols 30-31), staircase step 1 (cols 52-55),
     *          pipe #4 body (cols 68-69), final stair (cols 82-85)          */
    {0,0,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  7,8,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,4,4,4,4,0,0,0,0,  0,0,0,0,0,0,0,0,7,8,  0,0,0,0,0,0,0,0,0,0,  0,0,4,4,4,4,0,0,0,0},

    /* Row 13 – ground; pipes overwrite at cols 10-11,20-21,30-31,68-69;
     *          pit #1 at cols 36-38; pit #2 at cols 48-49; pit #3 at cols 74-75;
     *          final stair BLOCK at cols 82-85                               */
    {1,1,1,1,1,1,1,1,1,1,  7,8,1,1,1,1,1,1,1,1,  7,8,1,1,1,1,1,1,1,1,  7,8,1,1,1,1,0,0,0,1,  1,1,1,1,1,1,1,1,0,0,  1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,7,8,  1,1,1,1,0,0,1,1,1,1,  1,1,4,4,4,4,1,1,1,1},

    /* Row 14 – ground; pit #1 at cols 36-38; pit #2 at cols 48-49;
     *          pit #3 at cols 74-75; final stair BLOCK at cols 82-85         */
    {1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,0,0,0,1,  1,1,1,1,1,1,1,1,0,0,  1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,  1,1,1,1,0,0,1,1,1,1,  1,1,4,4,4,4,1,1,1,1},
};

/* ==========================================================================
 *  Brick tile sprite — 16×16 pixels, RGB565 big-endian byte pairs.
 *  Stored in flash (.rodata).
 *
 *  Colour palette:
 *    BR_M  0x7A43  mortar / joint  (~#783048)
 *    BR_F  0xCA84  brick face      (~#C85020)
 *    BR_H  0xEC09  highlight top   (~#E88048)
 *    BR_S  0x8940  shadow bottom   (~#882800)
 *
 *  Layout (top-half bricks: mortar at x=0,8 / bottom-half: offset mortar at x=4,12):
 *    rows  0–7 : two full bricks side-by-side (7px wide each, 1px mortar)
 *    rows  8–15: two offset bricks (4+7+3 px, mortar at x=4 and x=12)
 * ========================================================================== */

/* Local colour macros — big-endian pairs, undef'd after use */
#define BR_M  0x7A,0x43
#define BR_F  0xCA,0x84
#define BR_H  0xEC,0x09
#define BR_S  0x89,0x40

const uint8_t brick_sprite[16u * 16u * 2u] = {
    /* row  0 – top mortar */
    BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M, BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,
    /* row  1 – top-half highlight */
    BR_M,BR_H,BR_H,BR_H,BR_H,BR_H,BR_H,BR_H, BR_M,BR_H,BR_H,BR_H,BR_H,BR_H,BR_H,BR_H,
    /* row  2 – brick face */
    BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F, BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,
    /* row  3 – brick face */
    BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F, BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,
    /* row  4 – brick face */
    BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F, BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,
    /* row  5 – brick face */
    BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F, BR_M,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,BR_F,
    /* row  6 – top-half shadow */
    BR_M,BR_S,BR_S,BR_S,BR_S,BR_S,BR_S,BR_S, BR_M,BR_S,BR_S,BR_S,BR_S,BR_S,BR_S,BR_S,
    /* row  7 – mid mortar */
    BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M, BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,
    /* row  8 – bottom-half highlight (bricks offset by 4 px) */
    BR_H,BR_H,BR_H,BR_H,BR_M,BR_H,BR_H,BR_H, BR_H,BR_H,BR_H,BR_H,BR_M,BR_H,BR_H,BR_H,
    /* row  9 – brick face */
    BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F, BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F,
    /* row 10 – brick face */
    BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F, BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F,
    /* row 11 – brick face */
    BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F, BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F,
    /* row 12 – brick face */
    BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F, BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F,
    /* row 13 – brick face */
    BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F, BR_F,BR_F,BR_F,BR_F,BR_M,BR_F,BR_F,BR_F,
    /* row 14 – bottom-half shadow */
    BR_S,BR_S,BR_S,BR_S,BR_M,BR_S,BR_S,BR_S, BR_S,BR_S,BR_S,BR_S,BR_M,BR_S,BR_S,BR_S,
    /* row 15 – bottom mortar */
    BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M, BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,BR_M,
};

#undef BR_M
#undef BR_F
#undef BR_H
#undef BR_S

/* ==========================================================================
 *  Sky / Empty tile — solid sky blue with a subtle cloud-hint gradient.
 *  SK_T  top sky     0x5CBF  (~#5C94FC)
 *  SK_B  bottom sky  0x4C9F  (~#4880F0)
 * ========================================================================== */
#define SK_T  0x5C,0xBF
#define SK_B  0x4C,0x9F

const uint8_t empty_sprite[16u * 16u * 2u] = {
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T, SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,SK_T,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
    SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B, SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,SK_B,
};
#undef SK_T
#undef SK_B

/* ==========================================================================
 *  Ground tile — brown dirt with darker cracks and a top grass strip.
 *  GR_G  grass top    0x03E0  (~#00C000  green)
 *  GR_F  dirt face    0x8AC5  (~#8B5A2B  warm brown)
 *  GR_D  dirt dark    0x6A02  (~#683010  darker brown crack)
 *  GR_H  dirt hilight 0xA4C7  (~#A06030  lighter patch)
 * ========================================================================== */
#define GR_G  0x03,0xE0
#define GR_F  0x8A,0xC5
#define GR_D  0x6A,0x02
#define GR_H  0xA4,0xC7

const uint8_t ground_sprite[16u * 16u * 2u] = {
    /* row  0 – top grass */
    GR_G,GR_G,GR_G,GR_G,GR_G,GR_G,GR_G,GR_G, GR_G,GR_G,GR_G,GR_G,GR_G,GR_G,GR_G,GR_G,
    /* row  1 – grass/dirt border */
    GR_G,GR_G,GR_D,GR_G,GR_G,GR_D,GR_G,GR_G, GR_D,GR_G,GR_G,GR_D,GR_G,GR_G,GR_D,GR_G,
    /* row  2 – dirt top */
    GR_H,GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F, GR_H,GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,
    /* row  3 */
    GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F, GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,
    /* row  4 */
    GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F, GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F,
    /* row  5 */
    GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H, GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H,
    /* row  6 */
    GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F, GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,
    /* row  7 */
    GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F,GR_F, GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F,GR_F,
    /* row  8 */
    GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H,GR_F, GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H,GR_F,
    /* row  9 */
    GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F, GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,
    /* row 10 */
    GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F, GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,GR_F,
    /* row 11 */
    GR_H,GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F, GR_H,GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,
    /* row 12 */
    GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F, GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,GR_F,
    /* row 13 */
    GR_F,GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F, GR_F,GR_F,GR_D,GR_F,GR_F,GR_F,GR_D,GR_F,
    /* row 14 */
    GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H, GR_F,GR_F,GR_F,GR_H,GR_F,GR_F,GR_F,GR_H,
    /* row 15 */
    GR_D,GR_D,GR_D,GR_D,GR_D,GR_D,GR_D,GR_D, GR_D,GR_D,GR_D,GR_D,GR_D,GR_D,GR_D,GR_D,
};
#undef GR_G
#undef GR_F
#undef GR_D
#undef GR_H

/* ==========================================================================
 *  Question block tile — gold box with "?" symbol.
 *  QB_F  gold face    0xFE45  (~#FFCA28)
 *  QB_H  highlight    0xFFE0  (~#FFFF00  bright yellow)
 *  QB_S  shadow       0xB220  (~#B0A000  dark gold)
 *  QB_O  outline      0x0000  (black)
 *  QB_W  white        0xFFFF
 * ========================================================================== */
#define QB_F  0xFE,0x45
#define QB_H  0xFF,0xE0
#define QB_S  0xB2,0x20
#define QB_O  0x00,0x00
#define QB_W  0xFF,0xFF

const uint8_t question_sprite[16u * 16u * 2u] = {
    /* row  0 – top outline */
    QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O, QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,
    /* row  1 – top highlight strip */
    QB_O,QB_H,QB_H,QB_H,QB_H,QB_H,QB_H,QB_H, QB_H,QB_H,QB_H,QB_H,QB_H,QB_H,QB_H,QB_O,
    /* row  2 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_O,QB_O,QB_F, QB_F,QB_O,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  3 – "?" top arch */
    QB_O,QB_H,QB_F,QB_O,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_O,QB_F,QB_F,QB_S,QB_O,
    /* row  4 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_O,QB_O,QB_F, QB_F,QB_O,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  5 – "?" descend */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_O, QB_O,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  6 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_O,QB_F,QB_F, QB_F,QB_O,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  7 – "?" dot gap */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  8 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row  9 – "?" dot */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_O,QB_O,QB_W, QB_W,QB_O,QB_O,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row 10 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_O,QB_O,QB_W, QB_W,QB_O,QB_O,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row 11 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row 12 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row 13 */
    QB_O,QB_H,QB_F,QB_F,QB_F,QB_F,QB_F,QB_F, QB_F,QB_F,QB_F,QB_F,QB_F,QB_F,QB_S,QB_O,
    /* row 14 – bottom shadow strip */
    QB_O,QB_S,QB_S,QB_S,QB_S,QB_S,QB_S,QB_S, QB_S,QB_S,QB_S,QB_S,QB_S,QB_S,QB_S,QB_O,
    /* row 15 – bottom outline */
    QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O, QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,QB_O,
};
#undef QB_F
#undef QB_H
#undef QB_S
#undef QB_O
#undef QB_W

/* ==========================================================================
 *  Block tile — solid grey stone with chiselled border.
 *  BK_F  face    0x8410  (~#808080  mid grey)
 *  BK_H  hilight 0xCE59  (~#C0C0B8  light grey)
 *  BK_S  shadow  0x4208  (~#404040  dark grey)
 *  BK_O  outline 0x2104  (~#202020  near-black)
 * ========================================================================== */
#define BK_F  0x84,0x10
#define BK_H  0xCE,0x59
#define BK_S  0x42,0x08
#define BK_O  0x21,0x04

const uint8_t block_sprite[16u * 16u * 2u] = {
    /* row  0 – top edge */
    BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O, BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,
    /* row  1 – top highlight */
    BK_O,BK_H,BK_H,BK_H,BK_H,BK_H,BK_H,BK_H, BK_H,BK_H,BK_H,BK_H,BK_H,BK_H,BK_H,BK_O,
    /* row  2 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  3 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  4 */
    BK_O,BK_H,BK_F,BK_F,BK_H,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_H,BK_F,BK_F,BK_S,BK_O,
    /* row  5 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  6 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_S,BK_F, BK_F,BK_S,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  7 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  8 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row  9 */
    BK_O,BK_H,BK_F,BK_F,BK_H,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_H,BK_F,BK_F,BK_S,BK_O,
    /* row 10 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row 11 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_S,BK_F, BK_F,BK_S,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row 12 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row 13 */
    BK_O,BK_H,BK_F,BK_F,BK_F,BK_F,BK_F,BK_F, BK_F,BK_F,BK_F,BK_F,BK_F,BK_F,BK_S,BK_O,
    /* row 14 – bottom shadow strip */
    BK_O,BK_S,BK_S,BK_S,BK_S,BK_S,BK_S,BK_S, BK_S,BK_S,BK_S,BK_S,BK_S,BK_S,BK_S,BK_O,
    /* row 15 – bottom outline */
    BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O, BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,BK_O,
};
#undef BK_F
#undef BK_H
#undef BK_S
#undef BK_O

/* ==========================================================================
 *  Pipe tiles — 4 quadrants share colour macros.
 *  PL_O  outline       0x0000  (black)
 *  PL_H  bright green  0x07E0  (~#00FF00  highlight edge)
 *  PL_F  mid green     0x0540  (~#00A800  main face)
 *  PL_D  dark green    0x0300  (~#006000  shadow / body)
 *  PL_R  rim lighter   0x0660  (~#00CC00  lip rim)
 * ========================================================================== */
#define PL_O  0x00,0x00
#define PL_H  0x07,0xE0
#define PL_F  0x05,0x40
#define PL_D  0x03,0x00
#define PL_R  0x06,0x60

/* -- TILE_PIPE_TL  (top-left lip) ---------------------------------------- */
const uint8_t pipe_tl_sprite[16u * 16u * 2u] = {
    /* row  0 – lip top outline */
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    /* row  1 – lip top highlight */
    PL_O,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H, PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_O,
    /* row  2 – lip face */
    PL_O,PL_H,PL_R,PL_R,PL_R,PL_R,PL_R,PL_R, PL_R,PL_R,PL_R,PL_R,PL_R,PL_R,PL_D,PL_O,
    /* row  3 – lip face */
    PL_O,PL_H,PL_R,PL_R,PL_R,PL_R,PL_R,PL_R, PL_R,PL_R,PL_R,PL_R,PL_R,PL_R,PL_D,PL_O,
    /* row  4 – lip bottom outline */
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    /* rows 5-15 – left half of body */
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
};

/* -- TILE_PIPE_TR  (top-right lip) --------------------------------------- */
const uint8_t pipe_tr_sprite[16u * 16u * 2u] = {
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H, PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_H,PL_O,
    PL_O,PL_H,PL_R,PL_R,PL_R,PL_R,PL_R,PL_R, PL_R,PL_R,PL_R,PL_R,PL_R,PL_R,PL_D,PL_O,
    PL_O,PL_H,PL_R,PL_R,PL_R,PL_R,PL_R,PL_R, PL_R,PL_R,PL_R,PL_R,PL_R,PL_R,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    /* rows 5-15 – right half of body */
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
};

/* -- TILE_PIPE_BL  (body left) ------------------------------------------- */
const uint8_t pipe_bl_sprite[16u * 16u * 2u] = {
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
    PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_F,PL_D, PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,
};

/* -- TILE_PIPE_BR  (body right) ------------------------------------------ */
const uint8_t pipe_br_sprite[16u * 16u * 2u] = {
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
    PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O,PL_O, PL_O,PL_H,PL_F,PL_F,PL_F,PL_F,PL_D,PL_O,
};
#undef PL_O
#undef PL_H
#undef PL_F
#undef PL_D
#undef PL_R

/* Sprite lookup table — NULL means use solid colour fallback.
 * Indexed by TileID_t value (0..TILE_ID_COUNT-1).               */
const uint8_t * const tile_sprites[TILE_ID_COUNT] = {
    /* 0 TILE_EMPTY    */ empty_sprite,
    /* 1 TILE_GROUND   */ ground_sprite,
    /* 2 TILE_BRICK    */ brick_sprite,
    /* 3 TILE_QUESTION */ question_sprite,
    /* 4 TILE_BLOCK    */ block_sprite,
    /* 5 TILE_PIPE_TL  */ pipe_tl_sprite,
    /* 6 TILE_PIPE_TR  */ pipe_tr_sprite,
    /* 7 TILE_PIPE_BL  */ pipe_bl_sprite,
    /* 8 TILE_PIPE_BR  */ pipe_br_sprite,
};
