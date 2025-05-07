/* Includes ------------------------------------------------------------------*/
#include "manual_lcd.h" // Bao gồm file header tương ứng

/* Private variables ---------------------------------------------------------*/

// !! QUAN TRỌNG: Đảm bảo tên handle này khớp với tên được tạo bởi CubeMX trong main.c !!
extern SPI_HandleTypeDef hspi1; // Giả sử bạn dùng SPI1
#define LCD_SPI_HANDLE &hspi1 // Con trỏ tới SPI handle đang sử dụng

/* Dữ liệu Font -------------------------------------------------------------*/
// --- Dữ liệu Font8 (5x8) - Đã copy từ file font8.c của bạn ---
static const uint8_t Font8_Table[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFA,0x00,0x00,0x00,0xC0,0x00,0xC0,0x00,0x28,
	0xFE,0x28,0xFE,0x28,0x24,0xD4,0x54,0x54,0x18,0x48,0x54,0x2A,0x14,0x12,0x44,0xAC,
	0x54,0x2A,0x54,0x00,0x40,0x40,0x00,0x00,0x00,0x7C,0x82,0x00,0x00,0x00,0x00,0x82,
	0x7C,0x00,0x14,0x08,0x3E,0x08,0x14,0x08,0x08,0x3E,0x08,0x08,0x00,0xA0,0x60,0x00,
	0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x60,0x60,0x00,0x00,0x04,0x0A,0x12,0x24,0x44,
	0x3E,0x4A,0x52,0x62,0x3E,0x00,0x44,0xFE,0x04,0x00,0x44,0x8A,0x92,0x92,0x64,0x44,
	0x92,0x92,0x92,0x6C,0x1C,0x24,0x44,0xFE,0x04,0xEC,0x92,0x92,0x92,0x8C,0x1C,0x24,
	0x92,0x92,0x8C,0x80,0x8A,0x92,0xA2,0xC0,0x6C,0x92,0x92,0x92,0x6C,0x60,0x92,0x92,
	0x52,0x3C,0x00,0x60,0x00,0x60,0x00,0x00,0xA0,0x60,0x00,0x00,0x10,0x28,0x44,0x82,
	0x00,0x28,0x28,0x28,0x28,0x28,0x00,0x82,0x44,0x28,0x10,0x40,0x82,0x92,0x92,0x60,
	0x7C,0x82,0xBA,0xAA,0x78,0x7E,0x88,0x88,0x88,0x7E,0xFE,0x92,0x92,0x92,0x6C,0x3C,
	0x42,0x82,0x82,0x44,0xFE,0x82,0x82,0x82,0x7C,0xFE,0x92,0x92,0x92,0x82,0xFE,0x90,
	0x90,0x88,0x80,0x3C,0x42,0x82,0x92,0x5C,0xFE,0x10,0x10,0x10,0xFE,0x00,0x82,0xFE,
	0x82,0x00,0x0C,0x02,0x82,0xFC,0x80,0xFE,0x10,0x28,0x44,0x82,0xFE,0x02,0x02,0x02,
	0x02,0xFE,0x40,0x30,0x40,0xFE,0xFE,0x40,0x20,0x10,0xFE,0x7C,0x82,0x82,0x82,0x7C,
	0xFE,0x90,0x90,0x90,0x60,0x7C,0x82,0x8A,0x84,0x7A,0xFE,0x90,0x98,0x94,0x62,0x64,
	0x92,0x92,0x92,0x4C,0x80,0x80,0xFE,0x80,0x80,0xFC,0x02,0x02,0x02,0xFC,0xF8,0x04,
	0x02,0x04,0xF8,0xFC,0x02,0x1C,0x02,0xFC,0xC6,0x28,0x10,0x28,0xC6,0xE0,0x10,0x0E,
	0x10,0xE0,0x86,0x8A,0x92,0xA2,0xC2,0x00,0xFE,0x82,0x82,0x00,0x40,0x20,0x10,0x08,
	0x04,0x00,0x82,0x82,0xFE,0x00,0x00,0x10,0x28,0x44,0x00,0x02,0x02,0x02,0x02,0x02,
	0x00,0x80,0x40,0x00,0x00,0x04,0x2A,0x2A,0x2A,0x3E,0xFE,0x12,0x22,0x22,0x1C,0x1C,
	0x22,0x22,0x22,0x04,0x1C,0x22,0x22,0x12,0xFE,0x3C,0x5A,0x52,0x52,0x18,0x08,0x7E,
	0x88,0x80,0x80,0x70,0x92,0x92,0x52,0x3E,0xFE,0x10,0x10,0x10,0x0E,0x00,0x22,0xBE,
	0x02,0x00,0x04,0x02,0x22,0xBC,0x00,0x00,0xFE,0x10,0x28,0x44,0x00,0x82,0xFE,0x02,
	0x00,0x3E,0x20,0x18,0x20,0x1E,0x3E,0x10,0x10,0x10,0x0E,0x1C,0x22,0x22,0x22,0x1C,
	0xFE,0x22,0x22,0x22,0x1C,0x1C,0x22,0x22,0x12,0xFE,0x3E,0x10,0x10,0x08,0x08,0x48,
	0x52,0x52,0x52,0x24,0x00,0x08,0x7C,0x88,0x00,0x3C,0x02,0x02,0x04,0x3E,0x38,0x04,
	0x02,0x04,0x38,0x3C,0x02,0x0C,0x02,0x3C,0x66,0x18,0x18,0x18,0x66,0x78,0x04,0x02,
	0x42,0x3C,0x46,0x4A,0x52,0x62,0x02,0x00,0x10,0x6C,0x82,0x00,0x00,0x00,0xFE,0x00,
	0x00,0x00,0x82,0x6C,0x10,0x00,0x20,0x10,0x08,0x10,0x20
};

// Định nghĩa biến font (khớp với extern trong .h)
const Manual_FontDef Font8 = {Font8_Table, 5, 8}; // Rộng 5, Cao 8

/* Private function prototypes -----------------------------------------------*/
static void LCD_Select(void);
static void LCD_Deselect(void);
static void LCD_SetDataMode(void);
static void LCD_SetCommandMode(void);
static void LCD_Reset(void);
static HAL_StatusTypeDef LCD_SPI_Transmit(uint8_t *pData, uint16_t Size);
static void LCD_SendCommand(uint8_t cmd);
static void LCD_SendData(uint8_t data);
static void LCD_SendDataBuffer(uint8_t *pData, uint16_t Size);
static void LCD_WriteData16(uint16_t data);

/* Exported functions ---------------------------------------------------------*/

// --- Các hàm public giữ nguyên nội dung như phiên bản trước ---
// Init, BacklightOn/Off, SetWindow, DrawPixel, FillScreen,
// FillRectangle, DrawRectangle, DrawChar, WriteString, DrawLayout

/** @brief Khởi tạo LCD ILI9341. */
void Manual_LCD_Init(void) {
    LCD_Reset();
    LCD_SendCommand(ILI9341_SWRESET);
    HAL_Delay(150);
    LCD_SendCommand(ILI9341_SLPOUT);
    HAL_Delay(120); // Delay quan trọng
    LCD_SendCommand(ILI9341_PIXFMT);
    LCD_SendData(0x55); // 16 bpp
    LCD_SendCommand(ILI9341_MADCTL);
    LCD_SendData(0x48); // Hướng quét mặc định
    LCD_SendCommand(ILI9341_DISPON);
    HAL_Delay(10);
    Manual_LCD_BacklightOn();
}

/** @brief Bật đèn nền. */
void Manual_LCD_BacklightOn(void) {
#ifdef LCD_BL_PORT
    HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_SET);
#endif
}

/** @brief Tắt đèn nền. */
void Manual_LCD_BacklightOff(void) {
#ifdef LCD_BL_PORT
    HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_RESET);
#endif
}

/** @brief Đặt cửa sổ vẽ. */
void Manual_LCD_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    if (x1 >= ILI9341_WIDTH) x1 = ILI9341_WIDTH - 1;
    if (x2 >= ILI9341_WIDTH) x2 = ILI9341_WIDTH - 1;
    if (y1 >= ILI9341_HEIGHT) y1 = ILI9341_HEIGHT - 1;
    if (y2 >= ILI9341_HEIGHT) y2 = ILI9341_HEIGHT - 1;
    if (x1 > x2) { uint16_t t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { uint16_t t = y1; y1 = y2; y2 = t; }

    LCD_SendCommand(ILI9341_CASET); // Column Address Set
    {
        uint8_t data[] = {(x1 >> 8) & 0xFF, x1 & 0xFF, (x2 >> 8) & 0xFF, x2 & 0xFF};
        LCD_SendDataBuffer(data, 4);
    }
    LCD_SendCommand(ILI9341_PASET); // Page Address Set
    {
        uint8_t data[] = {(y1 >> 8) & 0xFF, y1 & 0xFF, (y2 >> 8) & 0xFF, y2 & 0xFF};
        LCD_SendDataBuffer(data, 4);
    }
    LCD_SendCommand(ILI9341_RAMWR); // Chuẩn bị ghi vào GRAM
}

/** @brief Vẽ một pixel. */
void Manual_LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    Manual_LCD_SetWindow(x, y, x, y);
    // Chuẩn bị gửi màu
    LCD_Select();
    LCD_SetDataMode();
    LCD_WriteData16(color);
    LCD_Deselect();
}

/** @brief Tô toàn bộ màn hình. */
void Manual_LCD_FillScreen(uint16_t color) {
    Manual_LCD_SetWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
    LCD_Select();
    LCD_SetDataMode();
    uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};
    for(uint32_t i = 0; i < (uint32_t)ILI9341_WIDTH * ILI9341_HEIGHT; i++) {
       LCD_SPI_Transmit(color_bytes, 2);
    }
    LCD_Deselect();
}

/** @brief Tô một vùng hình chữ nhật. */
void Manual_LCD_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if ((x + width) > ILI9341_WIDTH) width = ILI9341_WIDTH - x;
    if ((y + height) > ILI9341_HEIGHT) height = ILI9341_HEIGHT - y;
    if (width == 0 || height == 0) return;

    Manual_LCD_SetWindow(x, y, x + width - 1, y + height - 1);
    LCD_Select();
    LCD_SetDataMode();
    uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};
    for (uint32_t i = 0; i < (uint32_t)width * height; i++) {
        LCD_SPI_Transmit(color_bytes, 2);
    }
    LCD_Deselect();
}

/** @brief Vẽ đường viền hình chữ nhật. */
void Manual_LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT) || width == 0 || height == 0) return;
    if ((x + width) > ILI9341_WIDTH) width = ILI9341_WIDTH - x;
    if ((y + height) > ILI9341_HEIGHT) height = ILI9341_HEIGHT - y;

    Manual_LCD_FillRectangle(x, y, width, 1, color); // Top
    Manual_LCD_FillRectangle(x, y + height - 1, width, 1, color); // Bottom
    if (height > 2) {
        Manual_LCD_FillRectangle(x, y + 1, 1, height - 2, color); // Left
        Manual_LCD_FillRectangle(x + width - 1, y + 1, 1, height - 2, color); // Right
    } else if (height == 2) {}
}

/** @brief Vẽ một ký tự (sử dụng font lưu theo cột dọc, uint8_t). */
void Manual_LCD_DrawChar(uint16_t x, uint16_t y, char character, Manual_FontDef font, uint16_t color, uint16_t bgcolor) {
    uint16_t i, j;
    const uint8_t *font_char_ptr;

    if (character < ' ' || character > '~') character = '?';

    font_char_ptr = &font.table[(character - ' ') * font.Width];

    if ((x + font.Width > ILI9341_WIDTH) || (y + font.Height > ILI9341_HEIGHT)) return;

    Manual_LCD_SetWindow(x, y, x + font.Width - 1, y + font.Height - 1);

    LCD_Select();
    LCD_SetDataMode();

    for (j = 0; j < font.Width; j++) { // Duyệt qua từng cột
        uint8_t column_bitmap = font_char_ptr[j];
        for (i = 0; i < font.Height; i++) { // Duyệt qua từng pixel trong cột
            if ((column_bitmap >> i) & 0x01) { // Kiểm tra bit (bit 0 là trên cùng)
                LCD_WriteData16(color);
            } else {
                LCD_WriteData16(bgcolor);
            }
        }
    }

    LCD_Deselect();
}

/** @brief Vẽ một chuỗi ký tự. */
void Manual_LCD_WriteString(uint16_t x, uint16_t y, const char *str, Manual_FontDef font, uint16_t color, uint16_t bgcolor) {
    while (*str != '\0') {
        if (x > (ILI9341_WIDTH - font.Width)) {
            x = 0;
            y += font.Height;
            if (y > (ILI9341_HEIGHT - font.Height)) break;
        }
        Manual_LCD_DrawChar(x, y, *str, font, color, bgcolor);
        x += font.Width;
        str++;
    }
}

/** @brief Vẽ toàn bộ giao diện mẫu. */
void Manual_LCD_DrawLayout(void) {
    Manual_LCD_FillScreen(COLOR_BACKGROUND); // Nền xanh

    // Khung trên cùng
    Manual_LCD_FillRectangle(TOP_RECT_X, TOP_RECT_Y, TOP_RECT_W, TOP_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(TOP_RECT_X, TOP_RECT_Y, TOP_RECT_W, TOP_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(TOP_RECT_X + 10, TOP_RECT_Y + (TOP_RECT_H - Font8.Height)/2 + 2, "1111 1111", Font8, COLOR_TEXT, COLOR_RECT_FILL);

    // Khung Task 02-1
    Manual_LCD_FillRectangle(MID_RECT_X1, MID_RECT_Y1, MID_RECT_W, MID_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(MID_RECT_X1, MID_RECT_Y1, MID_RECT_W, MID_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(MID_RECT_X1 + 10, MID_RECT_Y1 + (MID_RECT_H - Font8.Height)/2 , "Task 02-1", Font8, COLOR_TEXT, COLOR_RECT_FILL);

    // Khung Task 02-2
    Manual_LCD_FillRectangle(MID_RECT_X2, MID_RECT_Y1, MID_RECT_W, MID_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(MID_RECT_X2, MID_RECT_Y1, MID_RECT_W, MID_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(MID_RECT_X2 + 10, MID_RECT_Y1 + (MID_RECT_H - Font8.Height)/2 , "Task 02-2", Font8, COLOR_TEXT, COLOR_RECT_FILL);

    // Khung Task 02-3
    Manual_LCD_FillRectangle(MID_RECT_X1, MID_RECT_Y2, MID_RECT_W, MID_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(MID_RECT_X1, MID_RECT_Y2, MID_RECT_W, MID_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(MID_RECT_X1 + 10, MID_RECT_Y2 + (MID_RECT_H - Font8.Height)/2 , "Task 02-3", Font8, COLOR_TEXT, COLOR_RECT_FILL);

    // Khung Task 02-4
    Manual_LCD_FillRectangle(MID_RECT_X2, MID_RECT_Y2, MID_RECT_W, MID_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(MID_RECT_X2, MID_RECT_Y2, MID_RECT_W, MID_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(MID_RECT_X2 + 10, MID_RECT_Y2 + (MID_RECT_H - Font8.Height)/2 , "Task 02-4", Font8, COLOR_TEXT, COLOR_RECT_FILL);

    // Khung Back
    Manual_LCD_FillRectangle(BACK_RECT_X, BACK_RECT_Y, BACK_RECT_W, BACK_RECT_H, COLOR_RECT_FILL);
    Manual_LCD_DrawRectangle(BACK_RECT_X, BACK_RECT_Y, BACK_RECT_W, BACK_RECT_H, COLOR_RECT_BORDER);
    Manual_LCD_WriteString(BACK_RECT_X + 5, BACK_RECT_Y + (BACK_RECT_H - Font8.Height)/2 + 1, "<<Back", Font8, COLOR_TEXT, COLOR_RECT_FILL);
}


/* Private functions ---------------------------------------------------------*/
// Định nghĩa các hàm static helper

/** @brief Kéo chân CS xuống thấp để chọn LCD. */
static void LCD_Select(void) {
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
}

/** @brief Kéo chân CS lên cao để bỏ chọn LCD. */
static void LCD_Deselect(void) {
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/** @brief Đặt chân DC lên cao (Chế độ gửi Dữ liệu). */
static void LCD_SetDataMode(void) {
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
}

/** @brief Đặt chân DC xuống thấp (Chế độ gửi Lệnh). */
static void LCD_SetCommandMode(void) {
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
}

/** @brief Thực hiện reset cứng cho LCD. */
static void LCD_Reset(void) {
    LCD_Deselect();
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(15);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
}

/** @brief Gửi dữ liệu qua SPI (hàm bao bọc HAL blocking). */
static HAL_StatusTypeDef LCD_SPI_Transmit(uint8_t *pData, uint16_t Size) {
    return HAL_SPI_Transmit(LCD_SPI_HANDLE, pData, Size, HAL_MAX_DELAY);
}

/** @brief Gửi một byte lệnh tới LCD. */
static void LCD_SendCommand(uint8_t cmd) {
    LCD_Select();
    LCD_SetCommandMode();
    LCD_SPI_Transmit(&cmd, 1);
    LCD_Deselect();
}

/** @brief Gửi một byte dữ liệu tới LCD. */
static void LCD_SendData(uint8_t data) {
    LCD_Select();
    LCD_SetDataMode();
    LCD_SPI_Transmit(&data, 1);
    LCD_Deselect();
}

/** @brief Gửi một buffer dữ liệu tới LCD. */
static void LCD_SendDataBuffer(uint8_t *pData, uint16_t Size) {
    LCD_Select();
    LCD_SetDataMode();
    LCD_SPI_Transmit(pData, Size);
    LCD_Deselect();
}

/** @brief Gửi một giá trị màu 16-bit (2 byte) qua SPI. (Nội bộ) */
static void LCD_WriteData16(uint16_t data) {
    uint8_t bytes[2];
    bytes[0] = (data >> 8) & 0xFF; // MSB
    bytes[1] = data & 0xFF;        // LSB
    LCD_SPI_Transmit(bytes, 2);
}
