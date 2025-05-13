#ifndef __MANUAL_LCD_H
#define __MANUAL_LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"
#include "main.h" // To access GPIO definitions and SPI handle

// Define LCD screen dimensions
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

// Define some basic colors (RGB565 format)
#define COLOR_BLACK 0x0000
#define COLOR_BLUE 0x001F
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW 0xFFE0
#define COLOR_WHITE 0xFFFF

    // Exported functions
    void Manual_LCD_Init(void);
    void Manual_LCD_Clear(uint16_t color);
    void Manual_LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
    void Manual_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    void Manual_LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
    void Manual_LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void Manual_LCD_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void Manual_LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t charColor, uint16_t bgColor, uint8_t size);
    void Manual_LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t charColor, uint16_t bgColor, uint8_t size);
    void Manual_LCD_DrawLayout(void); // As used in your main.c

    // New functions for updating layout elements
    void Manual_LCD_UpdateInfoText(const char *new_text);
    void Manual_LCD_RefillTaskBox(const char *task_name, uint16_t new_fill_color);

#ifdef __cplusplus
}
#endif

#endif /* __MANUAL_LCD_H */
