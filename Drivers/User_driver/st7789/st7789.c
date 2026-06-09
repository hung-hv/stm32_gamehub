/*
 * st7789.c
 *
 *  Created on: 17 May 2026
 *      Author: vieth
 */

#include "st7789.h"
#include "fonts.h"

/* Tracks the active device during an async DMA transfer so the
 * HAL_SPI_TxCpltCallback can release CS without a global handle. */
static ST7789_HandleTypeDef *s_dma_dev = NULL;
static const uint8_t        *s_dma_buf = NULL;  /* which buffer DMA is currently reading */
volatile uint8_t dma_tx_complete = 0; /* Flag set in DMA complete callback, cleared before new transfer */

// Hàm nội bộ (Private) chỉ dùng trong file này
static void WriteCommand(ST7789_HandleTypeDef *dev, uint8_t cmd) {
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_RESET); // Command mode
    HAL_SPI_Transmit(dev->spi, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

static void WriteData(ST7789_HandleTypeDef *dev, uint8_t data) {
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET); // Data mode
    HAL_SPI_Transmit(dev->spi, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

/*  @brief Opens the pixel-write window on the display (column then row address). 
* @param dev: Pointer to the display handle.
* @param x0, y0: Top-left corner of the window (inclusive).
* @param x1, y1: Bottom-right corner of the window (inclusive).
*/
static void SetWindow(ST7789_HandleTypeDef *dev,
                      uint16_t x0, uint16_t y0,
                      uint16_t x1, uint16_t y1)
{
    WriteCommand(dev, 0x2A);
    WriteData(dev, (x0 >> 8) & 0xFF); WriteData(dev, x0 & 0xFF);
    WriteData(dev, (x1 >> 8) & 0xFF); WriteData(dev, x1 & 0xFF);

    WriteCommand(dev, 0x2B);
    WriteData(dev, (y0 >> 8) & 0xFF); WriteData(dev, y0 & 0xFF);
    WriteData(dev, (y1 >> 8) & 0xFF); WriteData(dev, y1 & 0xFF);

    WriteCommand(dev, 0x2C); /* Memory Write — ready for pixel data */
}

void ST7789_Init(ST7789_HandleTypeDef *dev) {
    // 1. Hardware Reset
    HAL_GPIO_WritePin(dev->rst_port, dev->rst_pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(dev->rst_port, dev->rst_pin, GPIO_PIN_SET);
    HAL_Delay(50);

    // 2. Lệnh SLPOUT: Thoát khỏi chế độ ngủ
    WriteCommand(dev, 0x11);
    HAL_Delay(120); // Bắt buộc phải chờ 120ms

    // 3. Lệnh COLMOD (0x3A): Cấu hình định dạng màu sắc
    // Nếu thiếu lệnh này, màn hình sẽ không biết bạn gửi màu 16-bit hay 18-bit
    WriteCommand(dev, 0x3A);
    WriteData(dev, 0x55);    // 0x55 = RGB565 (16-bit/pixel)

    // 4. Lệnh MADCTL (0x36): Điều hướng hiển thị (Landscape 320x240)
    // Đây chính là lệnh sửa lỗi cắt 2/3 dọc của bạn!
    WriteCommand(dev, 0x36);
//    WriteData(dev, 0x70);    // 0x70 cấu hình màn hình nằm ngang (MX=1, MV=1)
    WriteData(dev, 0xB0);
    // Mẹo: Nếu màn hình bị ngược chữ hoặc ngược hướng di chuyển,
    // bạn hãy thử thay 0x70 bằng một trong các mã: 0xAC, 0xA0, hoặc 0x60.

    // 5. INVON (0x21) — commented out: this panel is NOT a negative-image IPS.
    // Enabling INVON caused all RGB565 colours to appear as their bitwise-NOT
    // complement (Red→Cyan, Green→Magenta, Blue→Yellow, Black→White).
    // Use INVOFF (0x20) explicitly so the display stays in normal mode.
    WriteCommand(dev, 0x20);   /* INVOFF – normal display, colours correct */
    // WriteCommand(dev, 0x21); /* INVON  – only enable for negative-image IPS panels */

    // 6. Lệnh DISPON (0x29): Bật hiển thị màn hình
    WriteCommand(dev, 0x29);
    HAL_Delay(50);
}
// Triển khai hàm vẽ
void ST7789_DrawPixel(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t color) {
    // 1. Gửi lệnh Set Column Address (0x2A)
    // 2. Gửi lệnh Set Row Address (0x2B)
    // 3. Gửi lệnh Write RAM (0x2C)
    // 4. Gửi Data (color)
}

void ST7789_FillScreen(ST7789_HandleTypeDef *dev, uint16_t color) {
    // Ép chặt giới hạn cửa sổ theo chiều ngang (320x240)
    uint16_t x_end = 320 - 1; // 319
    uint16_t y_end = 240 - 1; // 239

    WriteCommand(dev, 0x2A); // Column Set
    WriteData(dev, 0x00); WriteData(dev, 0x00);
    WriteData(dev, (x_end >> 8) & 0xFF); WriteData(dev, x_end & 0xFF);

    WriteCommand(dev, 0x2B); // Row Set
    WriteData(dev, 0x00); WriteData(dev, 0x00);
    WriteData(dev, (y_end >> 8) & 0xFF); WriteData(dev, y_end & 0xFF);

    WriteCommand(dev, 0x2C); // Write RAM

    /* Optimization: fill one 320-pixel scanline buffer once, send 240 rows.
     * Reduces SPI calls from 76,800 to 240 (320x fewer transactions).   */
    static uint8_t line_buf[320u * 2u];   /* 640 bytes — one full LCD row */
    const uint8_t hi = (color >> 8) & 0xFF;
    const uint8_t lo =  color       & 0xFF;
    for (uint16_t i = 0u; i < 320u; i++) {
        line_buf[i * 2u]      = hi;
        line_buf[i * 2u + 1u] = lo;
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    for (uint16_t row = 0u; row < 240u; row++) {
        HAL_SPI_Transmit(dev->spi, line_buf, 320u * 2u, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

void ST7789_DrawRectangle(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x >= 320) || (y >= 240)) return;
    if ((x + w > 320)) w = 320 - x;
    if ((y + h > 240)) h = 240 - y;

    // 1. Mở một cửa sổ rộng đúng bằng kích thước hình chữ nhật
    uint16_t x_end = x + w - 1;
    uint16_t y_end = y + h - 1;

    // Set trục X
    WriteCommand(dev, 0x2A);
    WriteData(dev, (x >> 8) & 0xFF);  WriteData(dev, x & 0xFF);
    WriteData(dev, (x_end >> 8) & 0xFF); WriteData(dev, x_end & 0xFF);

    // Set trục Y
    WriteCommand(dev, 0x2B);
    WriteData(dev, (y >> 8) & 0xFF);  WriteData(dev, y & 0xFF);
    WriteData(dev, (y_end >> 8) & 0xFF); WriteData(dev, y_end & 0xFF);

    WriteCommand(dev, 0x2C);

    /* Optimization: pre-fill one scanline, send row-by-row.
     * For a 16x16 tile : 16 HAL_SPI_Transmit calls  (was 256)  → 16x fewer.
     * For a 320x16 row : 16 HAL_SPI_Transmit calls  (was 5120) → 320x fewer.
     * The static buffer is shared with FillScreen — 640 bytes total.    */
    static uint8_t line_buf[320u * 2u];   /* one full LCD row = 640 bytes */
    const uint8_t hi = (color >> 8) & 0xFF;
    const uint8_t lo =  color       & 0xFF;
    for (uint16_t i = 0u; i < w; i++) {
        line_buf[i * 2u]      = hi;
        line_buf[i * 2u + 1u] = lo;
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    for (uint16_t row = 0u; row < h; row++) {
        HAL_SPI_Transmit(dev->spi, line_buf, (uint16_t)(w * 2u), HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

/**
 * @brief  Draw a tile (or any rectangle) from a pre-built pixel buffer using
 *         non-blocking DMA.  Returns immediately; CS is released by
 *         HAL_SPI_TxCpltCallback when the DMA burst completes.
 *
 * @param  dev        ST7789 device handle.
 * @param  x, y       Top-left corner of the destination rectangle (pixels).
 * @param  w, h       Width / height of the tile in pixels.
 * @param  pTileData  Pointer to w*h RGB565 pixels in big-endian byte order.
 *                    MUST remain valid until the DMA callback fires.
 */
void ST7789_DrawTile_DMA(ST7789_HandleTypeDef *dev,
                          uint16_t x, uint16_t y,
                          uint16_t w, uint16_t h,
                          uint8_t *pTileData)
{
    /* 1. Wait for any previous DMA to finish — SPI must be idle before
     *    SetWindow sends blocking commands on the same bus.             */
    ST7789_WaitDMA(dev);

    /* 2. Program the display window using blocking SPI (command bytes only). */
    SetWindow(dev, x, y, (uint16_t)(x + w - 1u), (uint16_t)(y + h - 1u));

    /* 2. Store device pointer so the TX callback can release CS. */
    s_dma_dev = dev;
    s_dma_buf = pTileData;

    /* 3. Assert CS and switch to DATA mode, then fire DMA. */
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    /* Total bytes = w * h * 2 (RGB565).  Fits uint16_t for tiles ≤ 128x128. */
    HAL_SPI_Transmit_DMA(dev->spi, pTileData, (uint16_t)(w * h * 2u));
    /* Returns immediately.  HAL_SPI_TxCpltCallback releases CS when done.  */
}

void ST7789_RenderMap(ST7789_HandleTypeDef *dev, uint8_t *frame_buf, uint32_t buf_size) {
    SetWindow(dev, 0, 0, 319, 239);

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    /* Send the entire frame buffer in one blocking call. */
    HAL_SPI_Transmit(dev->spi, frame_buf, 320u * 240u * 2u, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

/**
 * @brief  SPI TX-complete callback — releases CS after a DMA tile transfer.
 *         Defined here (weak override) so it lives next to DrawTile_DMA.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if ((s_dma_dev != NULL) && (hspi->Instance == s_dma_dev->spi->Instance))
    {
        HAL_GPIO_WritePin(s_dma_dev->cs_port, s_dma_dev->cs_pin, GPIO_PIN_SET);
        s_dma_buf = NULL;
        s_dma_dev = NULL;
        dma_tx_complete = 1; /* Set flag to indicate DMA transfer is complete */
    }
}

uint8_t isTxComplete() {
    return dma_tx_complete;
}

/**
 * @brief  Block until the specified buffer is no longer being read by DMA.
 *         Returns immediately if DMA is using a different buffer or is idle.
 *         Use with double-buffering: wait only on the buffer you are about
 *         to refill, not on the one currently being transmitted.
 */
void ST7789_WaitBuf(const uint8_t *buf)
{
    while (s_dma_buf == buf) {}
}

/**
 * @brief  Block until any in-progress DMA tile transfer completes.
 *         Call at the end of a frame to ensure the last burst finished.
 */
void ST7789_WaitDMA(ST7789_HandleTypeDef *dev)
{
    while (s_dma_dev == dev) {}
}

void ST7789_DrawRectangle_DMA(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ((x >= 320u) || (y >= 240u)) return;
    if ((x + w) > 320u) w = 320u - x;
    if ((y + h) > 240u) h = 240u - y;

    uint16_t x_end = x + w - 1u;
    uint16_t y_end = y + h - 1u;

    WriteCommand(dev, 0x2A);
    WriteData(dev, (x >> 8) & 0xFF);     WriteData(dev, x & 0xFF);
    WriteData(dev, (x_end >> 8) & 0xFF); WriteData(dev, x_end & 0xFF);

    WriteCommand(dev, 0x2B);
    WriteData(dev, (y >> 8) & 0xFF);     WriteData(dev, y & 0xFF);
    WriteData(dev, (y_end >> 8) & 0xFF); WriteData(dev, y_end & 0xFF);

    WriteCommand(dev, 0x2C);

    /* Pre-fill one scanline with the solid colour (RGB565, big-endian). */
    static uint8_t line_buf[320u * 2u];
    const uint8_t hi = (color >> 8) & 0xFF;
    const uint8_t lo =  color       & 0xFF;
    for (uint16_t i = 0u; i < w; i++) {
        line_buf[i * 2u]      = hi;
        line_buf[i * 2u + 1u] = lo;
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    for (uint16_t row = 0u; row < h; row++) {
        HAL_SPI_Transmit_DMA(dev->spi, line_buf, (uint16_t)(w * 2u));
        /* Spin-wait for current DMA burst to finish before sending next row.
         * Keeps CS asserted and the window open throughout the transfer.  */
        while (HAL_SPI_GetState(dev->spi) != HAL_SPI_STATE_READY) {}
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

void ST7789_WriteChar(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    // 1. Kiểm tra giới hạn ký tự (Mảng bắt đầu từ ký tự Space = 32)
    if (ch < 32 || ch > 126) return;

    // 2. Tính toán vị trí phần tử đầu tiên của ký tự trong mảng Font7x10
    // Mỗi ký tự chiếm đúng 10 dòng (font.height)
    uint32_t char_index = (ch - 32) * font.height;

    // 3. Thiết lập cửa sổ hiển thị trên ST7789 đúng bằng kích thước chữ
    uint16_t x_end = x + font.width - 1;
    uint16_t y_end = y + font.height - 1;

    if (x_end >= 320 || y_end >= 240) return; // An toàn biên

    WriteCommand(dev, 0x2A); // Column Set
    WriteData(dev, (x >> 8) & 0xFF);  WriteData(dev, x & 0xFF);
    WriteData(dev, (x_end >> 8) & 0xFF); WriteData(dev, x_end & 0xFF);

    WriteCommand(dev, 0x2B); // Row Set
    WriteData(dev, (y >> 8) & 0xFF);  WriteData(dev, y & 0xFF);
    WriteData(dev, (y_end >> 8) & 0xFF); WriteData(dev, y_end & 0xFF);

    WriteCommand(dev, 0x2C); // Memory Write

    // Tách sẵn mã màu
    uint8_t color_msg[2] = {(color >> 8) & 0xFF, color & 0xFF};
    uint8_t bg_msg[2]    = {(bgcolor >> 8) & 0xFF, bgcolor & 0xFF};

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    // Vòng lặp quét từng dòng của chữ
    for (uint32_t i = 0; i < font.height; i++) {
        // ĐỌC DỮ LIỆU 16-BIT: Ép kiểu chuẩn xác từ mảng dữ liệu của bạn
        uint16_t b = font.data[char_index + i];

        // Vòng lặp quét từng bit từ trái sang phải theo chiều rộng (7 pixel)
        for (uint32_t j = 0; j < font.width; j++) {
            // SỬ DỤNG MẶT NẠ 16-BIT (0x8000) vì font của bạn căn lề trái ở hàng 16-bit
            if (b & (0x8000 >> j)) {
                // Có nét chữ -> Gửi màu chữ
                HAL_SPI_Transmit(dev->spi, color_msg, 2, HAL_MAX_DELAY);
            } else {
                // Vùng trống -> Gửi màu nền
                HAL_SPI_Transmit(dev->spi, bg_msg, 2, HAL_MAX_DELAY);
            }
        }
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

void ST7789_WriteString(ST7789_HandleTypeDef *dev, uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor) {
    // Vòng lặp duyệt qua từng ký tự của chuỗi cho đến khi gặp ký tự kết thúc '\0'
    while (*str) {
        // Nếu ký tự tiếp theo vượt quá chiều rộng màn hình (320), tự động xuống dòng
        if (x + font.width >= 320) {
            x = 0;
            y += font.height;
            // Nếu vượt quá chiều cao màn hình (240), ngừng in
            if (y + font.height >= 240) {
                break;
            }
        }

        // Gọi hàm vẽ 1 ký tự
        ST7789_WriteChar(dev, x, y, *str, font, color, bgcolor);

        // Dịch tọa độ X sang phải để chuẩn bị cho ký tự kế tiếp
        x += font.width;
        str++;
    }
}
