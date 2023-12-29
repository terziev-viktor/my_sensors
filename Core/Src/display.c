#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "display.h"

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

#ifndef SSD1306_I2C_CMD_ADDR
#define SSD1306_I2C_CMD_ADDR        (0x00)
#endif

#ifndef SSD1306_I2C_DATA_ADDR
#define SSD1306_I2C_DATA_ADDR        (0x40)
#endif


// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8
#endif

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

static const uint16_t Font_data_11x18 [] = {
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // sp
        0x0000, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // !
        0x0000, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // "
        0x0000, 0x1980, 0x1980, 0x1980, 0x1980, 0x7FC0, 0x7FC0, 0x1980, 0x3300, 0x7FC0, 0x7FC0, 0x3300, 0x3300, 0x3300, 0x3300, 0x0000, 0x0000, 0x0000,   // #
        0x0000, 0x1E00, 0x3F00, 0x7580, 0x6580, 0x7400, 0x3C00, 0x1E00, 0x0700, 0x0580, 0x6580, 0x6580, 0x7580, 0x3F00, 0x1E00, 0x0400, 0x0400, 0x0000,   // $
        0x0000, 0x7000, 0xD800, 0xD840, 0xD8C0, 0xD980, 0x7300, 0x0600, 0x0C00, 0x1B80, 0x36C0, 0x66C0, 0x46C0, 0x06C0, 0x0380, 0x0000, 0x0000, 0x0000,   // %
        0x0000, 0x1E00, 0x3F00, 0x3300, 0x3300, 0x3300, 0x1E00, 0x0C00, 0x3CC0, 0x66C0, 0x6380, 0x6180, 0x6380, 0x3EC0, 0x1C80, 0x0000, 0x0000, 0x0000,   // &
        0x0000, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // '
        0x0080, 0x0100, 0x0300, 0x0600, 0x0600, 0x0400, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0400, 0x0600, 0x0600, 0x0300, 0x0100, 0x0080,   // (
        0x2000, 0x1000, 0x1800, 0x0C00, 0x0C00, 0x0400, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0400, 0x0C00, 0x0C00, 0x1800, 0x1000, 0x2000,   // )
        0x0000, 0x0C00, 0x2D00, 0x3F00, 0x1E00, 0x3300, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // *
        0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0xFFC0, 0xFFC0, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // +
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0400, 0x0400, 0x0800,   // ,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E00, 0x1E00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // -
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // .
        0x0000, 0x0300, 0x0300, 0x0300, 0x0600, 0x0600, 0x0600, 0x0600, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x1800, 0x1800, 0x1800, 0x0000, 0x0000, 0x0000,   // /
        0x0000, 0x1E00, 0x3F00, 0x3300, 0x6180, 0x6180, 0x6180, 0x6D80, 0x6D80, 0x6180, 0x6180, 0x6180, 0x3300, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 0
        0x0000, 0x0600, 0x0E00, 0x1E00, 0x3600, 0x2600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0000, 0x0000, 0x0000,   // 1
        0x0000, 0x1E00, 0x3F00, 0x7380, 0x6180, 0x6180, 0x0180, 0x0300, 0x0600, 0x0C00, 0x1800, 0x3000, 0x6000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x0000,   // 2
        0x0000, 0x1C00, 0x3E00, 0x6300, 0x6300, 0x0300, 0x0E00, 0x0E00, 0x0300, 0x0180, 0x0180, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 3
        0x0000, 0x0600, 0x0E00, 0x0E00, 0x1E00, 0x1E00, 0x1600, 0x3600, 0x3600, 0x6600, 0x7F80, 0x7F80, 0x0600, 0x0600, 0x0600, 0x0000, 0x0000, 0x0000,   // 4
        0x0000, 0x7F00, 0x7F00, 0x6000, 0x6000, 0x6000, 0x6E00, 0x7F00, 0x6380, 0x0180, 0x0180, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 5
        0x0000, 0x1E00, 0x3F00, 0x3380, 0x6180, 0x6000, 0x6E00, 0x7F00, 0x7380, 0x6180, 0x6180, 0x6180, 0x3380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 6
        0x0000, 0x7F80, 0x7F80, 0x0180, 0x0300, 0x0300, 0x0600, 0x0600, 0x0C00, 0x0C00, 0x0C00, 0x0800, 0x1800, 0x1800, 0x1800, 0x0000, 0x0000, 0x0000,   // 7
        0x0000, 0x1E00, 0x3F00, 0x6380, 0x6180, 0x6180, 0x2100, 0x1E00, 0x3F00, 0x6180, 0x6180, 0x6180, 0x6180, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 8
        0x0000, 0x1E00, 0x3F00, 0x7300, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F80, 0x1D80, 0x0180, 0x6180, 0x7300, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // 9
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // :
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C00, 0x0C00, 0x0400, 0x0400, 0x0800,   // ;
        0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x0380, 0x0E00, 0x3800, 0x6000, 0x3800, 0x0E00, 0x0380, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // <
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // =
        0x0000, 0x0000, 0x0000, 0x0000, 0x4000, 0x7000, 0x1C00, 0x0700, 0x0180, 0x0700, 0x1C00, 0x7000, 0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // >
        0x0000, 0x1F00, 0x3F80, 0x71C0, 0x60C0, 0x00C0, 0x01C0, 0x0380, 0x0700, 0x0E00, 0x0C00, 0x0C00, 0x0000, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // ?
        0x0000, 0x1E00, 0x3F00, 0x3180, 0x7180, 0x6380, 0x6F80, 0x6D80, 0x6D80, 0x6F80, 0x6780, 0x6000, 0x3200, 0x3E00, 0x1C00, 0x0000, 0x0000, 0x0000,   // @
        0x0000, 0x0E00, 0x0E00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x3180, 0x3180, 0x3F80, 0x3F80, 0x3180, 0x60C0, 0x60C0, 0x60C0, 0x0000, 0x0000, 0x0000,   // A
        0x0000, 0x7C00, 0x7E00, 0x6300, 0x6300, 0x6300, 0x6300, 0x7E00, 0x7E00, 0x6300, 0x6180, 0x6180, 0x6380, 0x7F00, 0x7E00, 0x0000, 0x0000, 0x0000,   // B
        0x0000, 0x1E00, 0x3F00, 0x3180, 0x6180, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6180, 0x3180, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // C
        0x0000, 0x7C00, 0x7F00, 0x6300, 0x6380, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6300, 0x6300, 0x7E00, 0x7C00, 0x0000, 0x0000, 0x0000,   // D
        0x0000, 0x7F80, 0x7F80, 0x6000, 0x6000, 0x6000, 0x6000, 0x7F00, 0x7F00, 0x6000, 0x6000, 0x6000, 0x6000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x0000,   // E
        0x0000, 0x7F80, 0x7F80, 0x6000, 0x6000, 0x6000, 0x6000, 0x7F00, 0x7F00, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x0000, 0x0000, 0x0000,   // F
        0x0000, 0x1E00, 0x3F00, 0x3180, 0x6180, 0x6000, 0x6000, 0x6000, 0x6380, 0x6380, 0x6180, 0x6180, 0x3180, 0x3F80, 0x1E00, 0x0000, 0x0000, 0x0000,   // G
        0x0000, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x7F80, 0x7F80, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x0000, 0x0000, 0x0000,   // H
        0x0000, 0x3F00, 0x3F00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x3F00, 0x3F00, 0x0000, 0x0000, 0x0000,   // I
        0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x6180, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // J
        0x0000, 0x60C0, 0x6180, 0x6300, 0x6600, 0x6600, 0x6C00, 0x7800, 0x7C00, 0x6600, 0x6600, 0x6300, 0x6180, 0x6180, 0x60C0, 0x0000, 0x0000, 0x0000,   // K
        0x0000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x0000,   // L
        0x0000, 0x71C0, 0x71C0, 0x7BC0, 0x7AC0, 0x6AC0, 0x6AC0, 0x6EC0, 0x64C0, 0x60C0, 0x60C0, 0x60C0, 0x60C0, 0x60C0, 0x60C0, 0x0000, 0x0000, 0x0000,   // M
        0x0000, 0x7180, 0x7180, 0x7980, 0x7980, 0x7980, 0x6D80, 0x6D80, 0x6D80, 0x6580, 0x6780, 0x6780, 0x6780, 0x6380, 0x6380, 0x0000, 0x0000, 0x0000,   // N
        0x0000, 0x1E00, 0x3F00, 0x3300, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x3300, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // O
        0x0000, 0x7E00, 0x7F00, 0x6380, 0x6180, 0x6180, 0x6180, 0x6380, 0x7F00, 0x7E00, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x0000, 0x0000, 0x0000,   // P
        0x0000, 0x1E00, 0x3F00, 0x3300, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6580, 0x6780, 0x3300, 0x3F80, 0x1E40, 0x0000, 0x0000, 0x0000,   // Q
        0x0000, 0x7E00, 0x7F00, 0x6380, 0x6180, 0x6180, 0x6380, 0x7F00, 0x7E00, 0x6600, 0x6300, 0x6300, 0x6180, 0x6180, 0x60C0, 0x0000, 0x0000, 0x0000,   // R
        0x0000, 0x0E00, 0x1F00, 0x3180, 0x3180, 0x3000, 0x3800, 0x1E00, 0x0700, 0x0380, 0x6180, 0x6180, 0x3180, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // S
        0x0000, 0xFFC0, 0xFFC0, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // T
        0x0000, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // U
        0x0000, 0x60C0, 0x60C0, 0x60C0, 0x3180, 0x3180, 0x3180, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x0E00, 0x0E00, 0x0E00, 0x0400, 0x0000, 0x0000, 0x0000,   // V
        0x0000, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xCCC0, 0x4C80, 0x4C80, 0x5E80, 0x5280, 0x5280, 0x7380, 0x6180, 0x6180, 0x0000, 0x0000, 0x0000,   // W
        0x0000, 0xC0C0, 0x6080, 0x6180, 0x3300, 0x3B00, 0x1E00, 0x0C00, 0x0C00, 0x1E00, 0x1F00, 0x3B00, 0x7180, 0x6180, 0xC0C0, 0x0000, 0x0000, 0x0000,   // X
        0x0000, 0xC0C0, 0x6180, 0x6180, 0x3300, 0x3300, 0x1E00, 0x1E00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // Y
        0x0000, 0x3F80, 0x3F80, 0x0180, 0x0300, 0x0300, 0x0600, 0x0C00, 0x0C00, 0x1800, 0x1800, 0x3000, 0x6000, 0x7F80, 0x7F80, 0x0000, 0x0000, 0x0000,   // Z
        0x0F00, 0x0F00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0F00, 0x0F00,   // [
        0x0000, 0x1800, 0x1800, 0x1800, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0300, 0x0300, 0x0300, 0x0000, 0x0000, 0x0000,   /* \ */
        0x1E00, 0x1E00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x1E00, 0x1E00,   // ]
        0x0000, 0x0C00, 0x0C00, 0x1E00, 0x1200, 0x3300, 0x3300, 0x6180, 0x6180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // ^
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0x0000,   // _
        0x0000, 0x3800, 0x1800, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // `
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F00, 0x3F80, 0x6180, 0x0180, 0x1F80, 0x3F80, 0x6180, 0x6380, 0x7F80, 0x38C0, 0x0000, 0x0000, 0x0000,   // a
        0x0000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6E00, 0x7F00, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x7F00, 0x6E00, 0x0000, 0x0000, 0x0000,   // b
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E00, 0x3F00, 0x7380, 0x6180, 0x6000, 0x6000, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // c
        0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x1D80, 0x3F80, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F80, 0x1D80, 0x0000, 0x0000, 0x0000,   // d
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E00, 0x3F00, 0x7300, 0x6180, 0x7F80, 0x7F80, 0x6000, 0x7180, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // e
        0x0000, 0x07C0, 0x0FC0, 0x0C00, 0x0C00, 0x7F80, 0x7F80, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000,   // f
        0x0000, 0x0000, 0x0000, 0x0000, 0x1D80, 0x3F80, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F80, 0x1D80, 0x0180, 0x6380, 0x7F00, 0x3E00,   // g
        0x0000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6F00, 0x7F80, 0x7180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x0000, 0x0000, 0x0000,   // h
        0x0000, 0x0600, 0x0600, 0x0000, 0x0000, 0x3E00, 0x3E00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0000, 0x0000, 0x0000,   // i
        0x0600, 0x0600, 0x0000, 0x0000, 0x3E00, 0x3E00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x4600, 0x7E00, 0x3C00,   // j
        0x0000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6180, 0x6300, 0x6600, 0x6C00, 0x7C00, 0x7600, 0x6300, 0x6300, 0x6180, 0x60C0, 0x0000, 0x0000, 0x0000,   // k
        0x0000, 0x3E00, 0x3E00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0000, 0x0000, 0x0000,   // l
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xDD80, 0xFFC0, 0xCEC0, 0xCCC0, 0xCCC0, 0xCCC0, 0xCCC0, 0xCCC0, 0xCCC0, 0xCCC0, 0x0000, 0x0000, 0x0000,   // m
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6F00, 0x7F80, 0x7180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x0000, 0x0000, 0x0000,   // n
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E00, 0x3F00, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // o
        0x0000, 0x0000, 0x0000, 0x0000, 0x6E00, 0x7F00, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x7F00, 0x6E00, 0x6000, 0x6000, 0x6000, 0x6000,   // p
        0x0000, 0x0000, 0x0000, 0x0000, 0x1D80, 0x3F80, 0x7380, 0x6180, 0x6180, 0x6180, 0x6180, 0x7380, 0x3F80, 0x1D80, 0x0180, 0x0180, 0x0180, 0x0180,   // q
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6700, 0x3F80, 0x3900, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x0000, 0x0000, 0x0000,   // r
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E00, 0x3F80, 0x6180, 0x6000, 0x7F00, 0x3F80, 0x0180, 0x6180, 0x7F00, 0x1E00, 0x0000, 0x0000, 0x0000,   // s
        0x0000, 0x0000, 0x0800, 0x1800, 0x1800, 0x7F00, 0x7F00, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1F80, 0x0F80, 0x0000, 0x0000, 0x0000,   // t
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6180, 0x6380, 0x7F80, 0x3D80, 0x0000, 0x0000, 0x0000,   // u
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x60C0, 0x3180, 0x3180, 0x3180, 0x1B00, 0x1B00, 0x1B00, 0x0E00, 0x0E00, 0x0600, 0x0000, 0x0000, 0x0000,   // v
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xDD80, 0xDD80, 0xDD80, 0x5500, 0x5500, 0x5500, 0x7700, 0x7700, 0x2200, 0x2200, 0x0000, 0x0000, 0x0000,   // w
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6180, 0x3300, 0x3300, 0x1E00, 0x0C00, 0x0C00, 0x1E00, 0x3300, 0x3300, 0x6180, 0x0000, 0x0000, 0x0000,   // x
        0x0000, 0x0000, 0x0000, 0x0000, 0x6180, 0x6180, 0x3180, 0x3300, 0x3300, 0x1B00, 0x1B00, 0x1B00, 0x0E00, 0x0E00, 0x0E00, 0x1C00, 0x7C00, 0x7000,   // y
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FC0, 0x7FC0, 0x0180, 0x0300, 0x0600, 0x0C00, 0x1800, 0x3000, 0x7FC0, 0x7FC0, 0x0000, 0x0000, 0x0000,   // z
        0x0380, 0x0780, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0E00, 0x1C00, 0x1C00, 0x0E00, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0780, 0x0380,   // {
        0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600,   // |
        0x3800, 0x3C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0E00, 0x0700, 0x0700, 0x0E00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x0C00, 0x3C00, 0x3800,   // }
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3880, 0x7F80, 0x4700, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // ~
};

DISPLAY_FONT Font_11x18 = {11,18, Font_data_11x18};

struct Display {
    I2C_HandleTypeDef *p_hi2c1;
    uint8_t screen[SSD1306_BUFFER_SIZE];
    uint16_t x;
    uint16_t y;
    bool is_on;
    bool initialized;
};

static Display self;

void Display_WriteCommand(uint8_t cmd) {
    assert(self.initialized == true);
    assert(HAL_I2C_Mem_Write(self.p_hi2c1, SSD1306_I2C_ADDR, SSD1306_I2C_CMD_ADDR, 1, &cmd, 1, HAL_MAX_DELAY) == HAL_OK);
}

void Display_WriteData(uint8_t *buffer, size_t buff_size) {
    assert(self.initialized == true);
    HAL_I2C_Mem_Write(self.p_hi2c1, SSD1306_I2C_ADDR, SSD1306_I2C_DATA_ADDR, 1, buffer, buff_size, HAL_MAX_DELAY);
}

bool Display_FillBuffer(uint8_t const *const buf, const uint32_t len) {
    if (len <= SSD1306_BUFFER_SIZE) {
        memcpy(self.screen, buf, len);
        return true;
    }
    return false;
}

void Display_SetOn(const bool on) {
    const uint8_t cmd = on ? 0xAF : 0xAE;
    Display_WriteCommand(cmd);
}

void Display_Init(I2C_HandleTypeDef * p_hi2c1) {
    assert(self.initialized == false);
    self.p_hi2c1 = p_hi2c1;
    self.initialized = true;
    Display_SetOn(false);
    Display_WriteCommand(0x20); // Set Memory Addressing Mode
    Display_WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
    Display_WriteCommand(0xB0); // Set Page Start Address for Page Addressing Mode,0-7
#ifdef SSD1306_MIRROR_VERT
    Display_WriteCommand(0xC0); // Mirror vertically
#else
    Display_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

    Display_WriteCommand(0x00); //---set low column address
    Display_WriteCommand(0x10); //---set high column address
    Display_WriteCommand(0x40); //--set start line address
    Display_SetContrast(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    Display_WriteCommand(0xA0); // Mirror horizontally
#else
    Display_WriteCommand(0xA1); //--set segment re-map 0 to 127
#endif
    Display_WriteCommand(0xA6); //--set normal color
    Display_WriteCommand(0x3F); // set hight to 64 pixels
    Display_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    Display_WriteCommand(0xD3); //-set display offset - CHECK
    Display_WriteCommand(0x00); //-not offset
    Display_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    Display_WriteCommand(0xF0); //--set divide ratio
    Display_WriteCommand(0xD9); //--set pre-charge period
    Display_WriteCommand(0x22); //
    Display_WriteCommand(0xDA); //--set com pins hardware configuration
    Display_WriteCommand(0x12);

    Display_WriteCommand(0xDB); //--set vcomh
    Display_WriteCommand(0x20); //0x20,0.77xVcc

    Display_WriteCommand(0x8D); //--set DC-DC enable
    Display_WriteCommand(0x14); //
    Display_SetOn(true); //--turn on SSD1306 panel

    // Clear screen
    Display_Fill(Black);

    // Flush buffer to screen
    Display_UpdateScreen();

    // Set default values for screen object
    self.x = 0;
    self.y = 0;

    self.initialized = true;
}

void Display_Fill(DISPLAY_COLOR color) {
    assert(self.initialized == true);
    memset(self.screen, (color == Black) ? 0x00 : 0xFF, sizeof self.screen);
}

void Display_UpdateScreen(void) {
    assert(self.initialized == true);
    for (uint8_t i = 0; i < SSD1306_HEIGHT / 8; i++) {
        Display_WriteCommand(0xB0 + i); // Set the current RAM page address.
        Display_WriteCommand(0x00 + SSD1306_X_OFFSET_LOWER);
        Display_WriteCommand(0x10 + SSD1306_X_OFFSET_UPPER);
        Display_WriteData(&self.screen[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

void Display_DrawPixel(uint8_t x, uint8_t y, DISPLAY_COLOR color) {
    assert(self.initialized == true);
    assert(x < SSD1306_WIDTH && y < SSD1306_HEIGHT);

    // Draw in the right color
    if (color == White) {
        self.screen[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        self.screen[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

char Display_WriteChar(char ch, DISPLAY_FONT Font, DISPLAY_COLOR color) {
    assert(self.initialized == true);
    if (ch < 32 || ch > 126) {
        return 0;
    }

    if (SSD1306_WIDTH < (self.x + Font.FontWidth)) {
        // try to start writing on a new line
        if (SSD1306_HEIGHT < (self.y + Font.FontHeight)) {
            // no space on the display
            return 0;
        }
        Display_SetCursor(0, self.y + Font.FontHeight);
    }

    for (uint8_t i = 0; i < Font.FontHeight; i++) {
        int b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (uint8_t j = 0; j < Font.FontWidth; j++) {
            if ((b << j) & 0x8000) {
                Display_DrawPixel(self.x + j, (self.y + i), (DISPLAY_COLOR) color);
            } else {
                Display_DrawPixel(self.x + j, (self.y + i), (DISPLAY_COLOR) !color);
            }
        }
    }
    // The current space is now taken
    self.x += Font.FontWidth;
    // Return written char for validation
    return ch;
}

char Display_WriteString(char *str, DISPLAY_FONT Font, DISPLAY_COLOR color) {
    assert(self.initialized == true);
    while (*str) {
        if (Display_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }

    return *str;
}

void Display_SetCursor(uint8_t x, uint8_t y) {
    assert(self.initialized == true);
    self.x = x;
    self.y = y;
}

void Display_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color) {
    assert(self.initialized == true);
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    Display_DrawPixel(x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        Display_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }

        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
}

void Display_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color) {
    assert(self.initialized == true);

    Display_DrawLine(x1, y1, x2, y1, color);
    Display_DrawLine(x2, y1, x2, y2, color);
    Display_DrawLine(x2, y2, x1, y2, color);
    Display_DrawLine(x1, y2, x1, y1, color);
}

void Display_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color) {
    assert(self.initialized == true);
    uint8_t x_start = ((x1<=x2) ? x1 : x2);
    uint8_t x_end   = ((x1<=x2) ? x2 : x1);
    uint8_t y_start = ((y1<=y2) ? y1 : y2);
    uint8_t y_end   = ((y1<=y2) ? y2 : y1);

    for (uint8_t y= y_start; (y<= y_end)&&(y<SSD1306_HEIGHT); y++) {
        for (uint8_t x= x_start; (x<= x_end)&&(x<SSD1306_WIDTH); x++) {
            Display_DrawPixel(x, y, color);
        }
    }
}

void Display_SetContrast(const uint8_t value) {
    assert(self.initialized == true);
    static const uint8_t setContrastControlRegister = 0x81;
    Display_WriteCommand(setContrastControlRegister);
    Display_WriteCommand(value);
}

bool Display_IsOn() {
    assert(self.initialized == true);
    return self.is_on;
}

void Display_Print(const char *format, ...) {
    assert(self.initialized == true);

    Display_Fill(Black);
    Display_SetCursor(0, 0);

    char message[SSD1306_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(message, SSD1306_BUFFER_SIZE, format, args);
    Display_WriteString(message, Font_11x18, White);

    Display_UpdateScreen();
    va_end(args);
}
