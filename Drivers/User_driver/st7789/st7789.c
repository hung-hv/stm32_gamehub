/*
 * st7789.c
 *
 *  Created on: 17 May 2026
 *      Author: vieth
 */

#include "st7789.h"
#include "fonts.h"

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

    // 5. Lệnh INVON (0x21): Bật đảo màu (Rất quan trọng với màn hình IPS)
    // Màn hình ST7789 IPS nếu không có lệnh này sẽ bị hiện tượng màu sắc âm bản (đen thành trắng)
    WriteCommand(dev, 0x21);

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

    uint32_t total_pixels = 320 * 240; // Phải là uint32_t để không bị tràn số
    uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    for (uint32_t i = 0; i < total_pixels; i++) {
        HAL_SPI_Transmit(dev->spi, color_bytes, 2, HAL_MAX_DELAY);
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

    // 2. Lệnh chuẩn bị ghi vào RAM
    WriteCommand(dev, 0x2C);

    // 3. Đổ màu liên tục vào cửa sổ đã mở
    uint32_t total_pixels = w * h;
    uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};

    // Bật CS một lần duy nhất và chuyển sang Data mode để đẩy data cho nhanh
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->dc_port, dev->dc_pin, GPIO_PIN_SET);

    for (uint32_t i = 0; i < total_pixels; i++) {
        // Thay vì gọi hàm WriteData rườm rà, ta gọi thẳng SPI của HAL ở đây để tối ưu tốc độ
        HAL_SPI_Transmit(dev->spi, color_bytes, 2, HAL_MAX_DELAY);
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
