#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <sys/types.h>
#include <stdbool.h>
#include "stm32f3xx_hal.h"

struct Display;
typedef struct Display Display;

typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} DISPLAY_COLOR;

typedef struct {
    const uint8_t FontWidth;    /*!< Font width in pixels */
    uint8_t FontHeight;   /*!< Font height in pixels */
    const uint16_t *data; /*!< Pointer to data font data array */
} DISPLAY_FONT;

extern DISPLAY_FONT Font_11x18;

void Display_Init(I2C_HandleTypeDef * p_hi2c1);
void Display_Fill(DISPLAY_COLOR color);
void Display_UpdateScreen(void);
void Display_DrawPixel(uint8_t x, uint8_t y, DISPLAY_COLOR color);
char Display_WriteChar(char ch, DISPLAY_FONT Font, DISPLAY_COLOR color);
char Display_WriteString(char* str, DISPLAY_FONT Font, DISPLAY_COLOR color);
void Display_SetCursor(uint8_t x, uint8_t y);
void Display_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color);
void Display_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color);
void Display_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DISPLAY_COLOR color);
void Display_SetContrast(const uint8_t value);
void Display_SetOn(const bool on);
bool Display_IsOn();
void Display_Print(const char *format, ...);

#endif /* INC_DISPLAY_H_ */
