/*
 * mario_map.h
 *
 *  Part of: Drivers/Game_Engine — World map data declarations.
 *
 *  Provides the external declaration for the Mario World 1-1 tilemap
 *  array. Include this header wherever map data is read.
 *
 *  Map dimensions: 15 rows x 90 columns (1440 x 240 pixels at 16 px/tile).
 */

#ifndef GAME_ENGINE_MARIO_MAP_H_
#define GAME_ENGINE_MARIO_MAP_H_

#include <stdint.h>

/* World 1-1 map data (extended to 90 columns).
 * Defined in mario_map.c as a 2-D array of tile IDs (see TileID_t in
 * st7789_map.h for ID meanings).
 * NOTE: Stored in SRAM. For ROM placement use:
 *           const uint8_t Mario_World_1_1[15][90] __attribute__((section(".rodata")))
 *       and update this declaration to `extern const uint8_t ...`.
 */
extern uint8_t Mario_World_1_1[15][90];

#endif /* GAME_ENGINE_MARIO_MAP_H_ */
