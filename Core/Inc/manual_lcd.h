/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_MANUAL_LCD_H_
#define INC_MANUAL_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"     // Để sử dụng các định nghĩa HAL
#include <stdint.h>  // Để sử dụng uint8_t, uint16_t, etc.

/* Exported constants --------------------------------------------------------*/

// --- Định nghĩa Chân GPIO ---
// !! XÁC MINH LẠI CÁC CHÂN NÀY KHỚP VỚI KẾT NỐI VÀ CUBEMX CỦA BẠN !!
#define LCD_CS_PORT     GPIOB
#define LCD_CS_PIN      GPIO_PIN_7
#define LCD_DC_PORT     GPIOB
#define LCD_DC_PIN      GPIO_PIN_8
#define LCD_RST_PORT    GPIOB
#define LCD_RST_PIN     GPIO_PIN_2
#define LCD_BL_PORT     GPIOB
#define LCD_BL_PIN      GPIO_PIN_6

// --- Lệnh Cơ bản của ILI9341 ---
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_RDDST       0x09
#define ILI9341_SLPIN       0x10
#define ILI9341_SLPOUT      0x11
#define ILI9341_PTLON       0x12
#define ILI9341_NORON       0x13
#define ILI9341_INVOFF      0x20
#define ILI9341_INVON       0x21
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A // Column Address Set
#define ILI9341_PASET       0x2B // Page Address Set
#define ILI9341_RAMWR       0x2C // Memory Write
#define ILI9341_RAMRD       0x2E // Memory Read
#define ILI9341_MADCTL      0x36 // Memory Access Control (Định hướng)
#define ILI9341_PIXFMT      0x3A // Pixel Format Set

// --- Định nghĩa Màu (16-bit RGB565) ---
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xD69A
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xF81F

// --- Kích thước Màn hình ---
#define ILI9341_WIDTH       240
#define ILI9341_HEIGHT      320

// --- Định nghĩa Màu Sắc Giao diện ---
#define COLOR_BACKGROUND    ILI9341_BLUE     // Màu nền xanh dương
#define COLOR_RECT_FILL     ILI9341_WHITE    // Màu nền trắng cho các khung
#define COLOR_RECT_BORDER   ILI9341_RED      // Màu viền đỏ cho các khung
#define COLOR_TEXT          ILI9341_BLACK    // Màu chữ đen

// --- Định nghĩa Layout Giao diện ---
#define SCREEN_W            ILI9341_WIDTH
#define SCREEN_H            ILI9341_HEIGHT
#define PADDING             10
#define TOP_RECT_X          PADDING
#define TOP_RECT_Y          PADDING
#define TOP_RECT_W          (SCREEN_W - 2 * PADDING)
#define TOP_RECT_H          35
#define MID_RECT_W          ((SCREEN_W - 3 * PADDING) / 2)
#define MID_RECT_H          80
#define MID_RECT_Y1         (TOP_RECT_Y + TOP_RECT_H + PADDING)
#define MID_RECT_Y2         (MID_RECT_Y1 + MID_RECT_H + PADDING)
#define MID_RECT_X1         PADDING
#define MID_RECT_X2         (MID_RECT_X1 + MID_RECT_W + PADDING)
#define BACK_RECT_W         70
#define BACK_RECT_H         30
#define BACK_RECT_X         (SCREEN_W - PADDING - BACK_RECT_W)
#define BACK_RECT_Y         (SCREEN_H - PADDING - BACK_RECT_H)

// --- Định nghĩa Cấu trúc Font ---
typedef struct {
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
} Manual_FontDef;

/* Exported functions prototypes ---------------------------------------------*/

// --- Khai báo Font Sử dụng ---
extern const Manual_FontDef Font8; // Định nghĩa trong .c

// --- Khai báo các hàm public ---
void Manual_LCD_Init(void);
void Manual_LCD_BacklightOn(void);
void Manual_LCD_BacklightOff(void);
void Manual_LCD_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void Manual_LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void Manual_LCD_DrawChar(uint16_t x, uint16_t y, char character, Manual_FontDef font, uint16_t color, uint16_t bgcolor);
void Manual_LCD_WriteString(uint16_t x, uint16_t y, const char *str, Manual_FontDef font, uint16_t color, uint16_t bgcolor);
void Manual_LCD_FillScreen(uint16_t color);
void Manual_LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Manual_LCD_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Manual_LCD_DrawLayout(void);

#endif /* INC_MANUAL_LCD_H_ */
