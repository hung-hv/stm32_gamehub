/*
 * st7789.h
 *
 *  Created on: 17 May 2026
 *      Author: vieth
 */

#ifndef ST7789_H_
#define ST7789_H_

#include "stm32h5xx_hal.h" // Chỉ include HAL của dòng chip đang dùng
#include "fonts.h"
#include <stdint.h>

/*TODO: stub fixed screen size*/
#define LCD_WIDTH  ((uint16_t)320u)
#define LCD_HEIGHT ((uint16_t)240u)

// 1. Cấu trúc cấu hình phần cứng (Hardware Abstraction)
typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef      *cs_port;
    uint16_t          cs_pin;
    GPIO_TypeDef      *dc_port;
    uint16_t          dc_pin;
    GPIO_TypeDef      *rst_port;
    uint16_t          rst_pin;
} ST7789_HandleTypeDef;

// 2. Các API Công khai (Public Functions)
void ST7789_Init(ST7789_HandleTypeDef *dev);
void ST7789_FillScreen(ST7789_HandleTypeDef *dev, uint16_t color);
void ST7789_DrawPixel(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t color);
void ST7789_DrawRectangle(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawRectangle_DMA(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawTile_DMA(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *pTileData);
void ST7789_WaitBuf(const uint8_t *buf);
void ST7789_WaitDMA(ST7789_HandleTypeDef *dev);

void ST7789_RenderMap_DMA(ST7789_HandleTypeDef *dev, uint16_t *frame_buf);
uint8_t isTxComplete();

/* Write char and string */
void ST7789_WriteChar(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7789_WriteString(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);

#endif /* ST7789_H_ */
