/*
 * st7789_map.c
 *
 *  Part of: Drivers/Game_Engine — Tilemap renderer for ST7789 LCD.
 *
 *  Renders a horizontally scrolling 2D Mario tilemap onto a 320x240
 *  ST7789 display using solid-colour 16x16 tile blocks.
 *
 *  Implementation notes
 *  --------------------
 *  No dynamic memory. All variables are stack-local or file-static const.
 *
 *  The inner column loop runs col = 0 .. SCREEN_COLS (21 iterations).
 *  The 21st iteration (col == 20) draws a partial "buffer" tile at the
 *  right screen edge, preventing a pixel-wide gap during sub-tile scroll.
 *
 *  screen_x is kept as int16_t throughout the clipping math because when
 *  col == 0 and offset_x > 0 the left tile is partially off-screen to the
 *  left (screen_x = -offset_x, range -15..0). Only after clipping ensures
 *  draw_x >= 0 is the value cast to uint16_t for ST7789_DrawRectangle.
 */

#include "st7789_map.h"
#include "mario_map.h"   /* Mario_World_1_1[] */

uint16_t FrameBuffer[LCD_WIDTH * LCD_HEIGHT];

/* -------------------------------------------------------------------------
 *  Colour lookup table — indexed directly by TileID_t value (0..8).
 *  Pipe top sections get the brighter green; body sections get darker green
 *  to create a subtle 3-D pipe appearance.
 * ---------------------------------------------------------------------- */
static const uint16_t tile_colors[TILE_ID_COUNT] = {
    COLOR_SKY,       /* 0 : TILE_EMPTY    */
    COLOR_GROUND,    /* 1 : TILE_GROUND   */
    COLOR_BRICK,     /* 2 : TILE_BRICK    */
    COLOR_QUESTION,  /* 3 : TILE_QUESTION */
    COLOR_BLOCK,     /* 4 : TILE_BLOCK    */
    COLOR_PIPE_LT,   /* 5 : TILE_PIPE_TL  – bright green lip */
    COLOR_PIPE_LT,   /* 6 : TILE_PIPE_TR  – bright green lip */
    COLOR_PIPE_DK,   /* 7 : TILE_PIPE_BL  – dark  green body */
    COLOR_PIPE_DK,   /* 8 : TILE_PIPE_BR  – dark  green body */
};

/* -------------------------------------------------------------------------
 *  ST7789_RenderMap
 * ---------------------------------------------------------------------- */
void ST7789_RenderMap(ST7789_HandleTypeDef *dev, uint32_t camera_x)
{
    /* --- Scrolling parameters ----------------------------------------- */
    uint16_t start_col = camera_x / TILE_SIZE;   /* first visible map col (tile-aligned)  */
    uint16_t offset_x  = camera_x % TILE_SIZE;   /* sub-tile pixel shift: 0..TILE_SIZE-1  */

    uint8_t  row, col;

    for (row = 0u; row < (uint8_t)MAP_ROWS; row++)
    {
        uint16_t screen_y = (uint16_t)row * TILE_SIZE;

        /* col runs 0 .. SCREEN_COLS inclusive (21 total).
         * The extra col == 20 covers the right-edge partial tile so that
         * no undrawn strip appears after sub-tile scrolling.             */
        for (col = 0u; col <= (uint8_t)SCREEN_COLS; col++)
        {
            uint16_t current_col = start_col + (uint16_t)col;

            /* ----- Safety boundary: past end of map → treat as sky ---- */
            uint8_t tile_id;
            if (current_col >= MAP_COLS)
            {
                tile_id = (uint8_t)TILE_EMPTY;
            }
            else
            {
                tile_id = Mario_World_1_1[row][current_col];

                /* Guard: corrupt map data with an out-of-range ID falls
                 * back to sky instead of indexing tile_colors out-of-bounds. */
                if (tile_id >= (uint8_t)TILE_ID_COUNT)
                {
                    tile_id = (uint8_t)TILE_EMPTY;
                }
            }

            /* ----- Screen X of this tile's left edge (may be negative) - */
            /* screen_x = col*16 - offset_x
             * When col==0 and offset_x>0: screen_x is -offset_x (range -15..-1).
             * This is intentional; we clip it below.                     */
            int16_t screen_x = (int16_t)((uint16_t)col * TILE_SIZE)
                              - (int16_t)offset_x;

            /* ----- Cull tiles fully outside the visible area ----------- */
            if (screen_x >= (int16_t)LCD_WIDTH)
            {
                continue;   /* tile starts at or beyond right screen edge  */
            }
            if (screen_x + (int16_t)TILE_SIZE <= 0)
            {
                continue;   /* tile ends at or before left screen edge     */
            }

            /* ----- Left-edge clip --------------------------------------- */
            int16_t draw_x = screen_x;
            int16_t draw_w = (int16_t)TILE_SIZE;

            if (draw_x < 0)
            {
                /* Tile straddles the left edge.
                 * draw_x is negative, so adding it to draw_w shrinks width
                 * by the number of pixels that are off-screen.           */
                draw_w += draw_x;
                draw_x  = 0;
            }

            /* ----- Right-edge clip -------------------------------------- */
            if (draw_x + draw_w > (int16_t)LCD_WIDTH)
            {
                draw_w = (int16_t)LCD_WIDTH - draw_x;
            }

            /* ----- Degenerate tile (zero/negative width after clip) ---- */
            if (draw_w <= 0)
            {
                continue;
            }

            /* ----- Draw the tile --------------------------------------- */
            if ((uint16_t)draw_w == TILE_SIZE)
            {
                const uint8_t *sprite = tile_sprites[tile_id];

                if (sprite != NULL)
                {
                    /* Flash sprite — DMA reads directly from .rodata.
                     * No CPU fill needed; ST7789_DrawTile_DMA calls
                     * ST7789_WaitDMA internally before setting the window. */
                    ST7789_DrawTile_DMA(dev,
                                        (uint16_t)draw_x,
                                        screen_y,
                                        TILE_SIZE,
                                        TILE_SIZE,
                                        (uint8_t *)sprite);
                }
                else
                {
                    /* Solid colour — double-buffer DMA (ping-pong).
                     *
                     * Two static buffers alternate each tile:
                     *   CPU fills buf[next]  while  DMA sends buf[current]
                     * ST7789_WaitBuf() only stalls if the CPU lapped the DMA,
                     * which almost never happens at 16x16 solid-colour tiles.
                     */
                    static uint8_t tile_buf[2u][TILE_SIZE * TILE_SIZE * 2u];
                    static uint8_t buf_idx = 0u;

                    uint8_t *buf = tile_buf[buf_idx];

                    /* Wait only if this specific buffer is still in DMA. */
                    ST7789_WaitBuf(buf);

                    /* Fill the idle buffer with the tile colour. */
                    const uint8_t hi = (tile_colors[tile_id] >> 8u) & 0xFFu;
                    const uint8_t lo =  tile_colors[tile_id]        & 0xFFu;
                    for (uint16_t p = 0u; p < (uint16_t)(TILE_SIZE * TILE_SIZE); p++) {
                        buf[p * 2u]      = hi;
                        buf[p * 2u + 1u] = lo;
                    }

                    /* Fire DMA (returns immediately). */
                    ST7789_DrawTile_DMA(dev,
                                        (uint16_t)draw_x,
                                        screen_y,
                                        TILE_SIZE,
                                        TILE_SIZE,
                                        buf);

                    /* Alternate to the other buffer for the next tile. */
                    buf_idx ^= 1u;
                }
            }
            else
            {
                /* Partial tile (clipped edge) — wait for DMA then blocking fill. */
                ST7789_WaitDMA(dev);
                ST7789_DrawRectangle(dev,
                                     (uint16_t)draw_x,
                                     screen_y,
                                     (uint16_t)draw_w,
                                     TILE_SIZE,
                                     tile_colors[tile_id]);
            }
        }
    }
    /* Ensure the very last DMA burst is complete before returning. */
    ST7789_WaitDMA(dev);
}

void ST7789_RenderTile16x16(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint8_t tile_id)
{
    const uint8_t *sprite = tile_sprites[tile_id];
    ST7789_DrawTile_DMA(dev, x, y, TILE_SIZE, TILE_SIZE, (uint8_t *)sprite);
}

void ST7789_RenderScreen(ST7789_HandleTypeDef *dev)
{
    int map_row = 0;
    int map_col = 0;
    int lcd_pixel_x = 0;
    int lcd_pixel_y = 0;
    uint8_t tile_id = 0;
    // const uint8_t *sprite = tile_sprites[tile_id];
    for (map_row = 0u; map_row < (uint8_t)MAP_ROWS; map_row++) {
        lcd_pixel_y = map_row*TILE_SIZE;
        for (map_col = 0u; map_col < (uint8_t)SCREEN_COLS; map_col++) {
            lcd_pixel_x = map_col*TILE_SIZE;
            /* get tile ID by check in global map_buffer */
            tile_id = Mario_World_1_1[map_row][map_col];
            ST7789_DrawTile_DMA(dev, lcd_pixel_x, lcd_pixel_y, TILE_SIZE, TILE_SIZE, tile_sprites[tile_id]);
        }
    }
}

void Engine_Draw_Sprite_To_Buffer(int dest_x, int dest_y, const uint16_t *sprite_data) {
    if (dest_x <= -TILE_SIZE || dest_x >= LCD_WIDTH || dest_y <= -TILE_SIZE || dest_y >= LCD_HEIGHT) return;

    int x_start = 0, x_end = TILE_SIZE;
    int y_start = 0, y_end = TILE_SIZE;

    if (dest_x < 0)                  x_start = -dest_x;
    if (dest_x + TILE_SIZE > LCD_WIDTH) x_end = LCD_WIDTH - dest_x;
    if (dest_y < 0)                  y_start = -dest_y;
    if (dest_y + TILE_SIZE > LCD_HEIGHT) y_end = LCD_HEIGHT - dest_y;

    for (int y = y_start; y < y_end; y++) {
        int buffer_row = (dest_y + y) * LCD_WIDTH + dest_x;
        int tile_row   = y * TILE_SIZE;

        for (int x = x_start; x < x_end; x++) {
            uint16_t color = sprite_data[tile_row + x];
            
            // Mẹo: Giả sử màu Đen (0x0000) là màu nền trong suốt của nhân vật Mario
            if (color != 0x0000) { 
                FrameBuffer[buffer_row + x] = color;
            }
        }
    }
}

/* Renders the tilemap to a frame buffer, accounting for camera scroll position.
* Calculates which map columns are visible, applies horizontal offset clipping,
* and prepares tiles for display on the 320x240 LCD.
*
* camera_x = 20 (pixels scrolled into the map)
*
* MAP TILES:
*  col:     0          1          2          3
*        |<-16px->|<-16px->|<-16px->|<-16px->| ...
*        +--------+--------+--------+--------+
*        |  tile0 |  tile1 |  tile2 |  tile3 |
*        +--------+--------+--------+--------+
*                  ^
*                  | camera_x=20
*
* map_start_col = 20 / 16 = 1   (first tile column that is visible)
* map_offset_x  = 20 % 16 = 4   (pixels into tile1 that are off-screen to the left)
*
* SCREEN:
*  screen_x:  0        16        32 ...
*             +--------+--------+--
*             | tile1  | tile2  |
*             +--------+--------+--
*              <-4px->|
*              clipped  ^
*                       first visible pixel of tile1
*
*  tile1 is drawn starting at screen_x = -map_offset_x = -4
*  then clipped so only pixels [4..15] of tile1 appear at screen [0..11]
* @param camera_x Horizontal pixel offset into the map (0 = leftmost).
*/
void Engine_Draw_Map_To_Buffer(int camera_x){
    /*get the index of the starting column in the map based on the camera position */
    int map_tile_start_col = camera_x / TILE_SIZE; 
    /*get the pixel offset within the starting column */
    int map_px_offset_x  = camera_x % TILE_SIZE;
    int map_tile_row, screen_tile_col;
    int map_tile_current_col;
    uint8_t tile_id;
    const uint8_t *sprite;
    int screen_px_x = 0;
    int screen_px_y = 0;
    for (map_tile_row = 0u; map_tile_row < (uint8_t)MAP_ROWS; map_tile_row++) {
        for (screen_tile_col = 0u; screen_tile_col <= (uint8_t)MAX_SCREEN_COLS; screen_tile_col++) {
            map_tile_current_col = map_tile_start_col + (uint16_t)screen_tile_col;
            /*get current tile ID from the map */
            tile_id = Mario_World_1_1[map_tile_row][map_tile_current_col];
            /*get sprite for the current tile */
            sprite = tile_sprites[tile_id];
            /*calculate the screen pixel position for this tile */
            screen_px_x = ((int16_t)screen_tile_col * TILE_SIZE) - map_px_offset_x;
            screen_px_y = (int16_t)map_tile_row * TILE_SIZE;
            Engine_Draw_Sprite_To_Buffer(screen_px_x, screen_px_y, (const uint16_t *)sprite);
        }
    }
}

