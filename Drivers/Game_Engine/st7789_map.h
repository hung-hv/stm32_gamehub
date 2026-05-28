/*
 * st7789_map.h
 *
 *  Part of: Drivers/Game_Engine — Tilemap renderer for ST7789 LCD.
 *
 *  Provides the public API, geometry constants, tile ID enum, and the
 *  RGB565 colour palette for the 2D side-scrolling Mario tilemap engine.
 *
 *  Target display : ST7789 320x240, Landscape mode
 *  Tile size      : 16 x 16 pixels
 *  Map size       : 60 columns x 15 rows  (960 x 240 px)
 *  Screen grid    : 20 columns x 15 rows  (320 x 240 px)
 */

#ifndef GAME_ENGINE_ST7789_MAP_H_
#define GAME_ENGINE_ST7789_MAP_H_

#include "st7789.h"   /* ST7789_HandleTypeDef, ST7789_DrawRectangle()  */
#include <stdint.h>

/* =========================================================================
 *  Map & screen geometry
 * ========================================================================= */
#define TILE_SIZE        ((uint16_t)16u)   /* pixels per tile edge            */
#define MAP_ROWS         ((uint16_t)15u)   /* tile rows  in map (15*16=240px) */
#define MAP_COLS         ((uint16_t)90u)   /* tile cols  in map (90*16=1440px) */
#define SCREEN_COLS      ((uint16_t)20u)   /* visible tile columns on screen  */
#define SCREEN_ROWS      ((uint16_t)15u)   /* visible tile rows  on screen    */
#define LCD_WIDTH        ((uint16_t)320u)  /* ST7789 landscape width  (px)    */
#define LCD_HEIGHT       ((uint16_t)240u)  /* ST7789 landscape height (px)    */
#define MAP_PIXEL_WIDTH  ((uint32_t)(MAP_COLS * TILE_SIZE))  /* 1440 px       */

/* =========================================================================
 *  Tile ID definitions
 * ========================================================================= */
typedef enum {
    TILE_EMPTY    = 0,   /* Sky / Air                             */
    TILE_GROUND   = 1,   /* Dirt / Soil                           */
    TILE_BRICK    = 2,   /* Breakable brown brick                 */
    TILE_QUESTION = 3,   /* Mystery item box (gold)               */
    TILE_BLOCK    = 4,   /* Solid grey stone block                */
    TILE_PIPE_TL  = 5,   /* Green pipe – top-left  (lip)          */
    TILE_PIPE_TR  = 6,   /* Green pipe – top-right (lip)          */
    TILE_PIPE_BL  = 7,   /* Green pipe – body left                */
    TILE_PIPE_BR  = 8,   /* Green pipe – body right               */
    TILE_ID_COUNT = 9    /* Sentinel: total number of tile types  */
} TileID_t;

/* =========================================================================
 *  RGB565 colour palette
 *
 *  Conversion formula (24-bit → 16-bit):
 *      rgb565 = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3)
 * ========================================================================= */
#define COLOR_SKY       ((uint16_t)0x5CBFu)   /* ~#5C94FC  sky blue           */
#define COLOR_GROUND    ((uint16_t)0x8AC5u)   /* ~#8B5A2B  warm brown dirt    */
#define COLOR_BRICK     ((uint16_t)0xB281u)   /* ~#B45309  brick red-brown    */
#define COLOR_QUESTION  ((uint16_t)0xFE45u)   /* ~#FFCA28  gold               */
#define COLOR_BLOCK     ((uint16_t)0x8410u)   /* ~#808080  mid grey stone     */
#define COLOR_PIPE_LT   ((uint16_t)0x0540u)   /* ~#00AA00  pipe top / lip     */
#define COLOR_PIPE_DK   ((uint16_t)0x0320u)   /* ~#006400  pipe body          */

/* =========================================================================
 *  Public API
 * ========================================================================= */

/**
 * @brief  Render the full visible tilemap frame at the given camera offset.
 *
 * Covers every pixel on the 320x240 LCD each call; no prior FillScreen
 * is required between frames.
 *
 * @param  dev       Pointer to the initialised ST7789 device handle.
 * @param  camera_x  Horizontal pixel offset into the map (0 = leftmost).
 *                   Clamp caller-side to [0 .. MAP_PIXEL_WIDTH - LCD_WIDTH]
 *                   i.e. [0 .. 640] before passing.
 */
void ST7789_RenderMap(ST7789_HandleTypeDef *dev, uint32_t camera_x);

#endif /* GAME_ENGINE_ST7789_MAP_H_ */
