#include "manual_touch.h"
#include <stdio.h>  // For sprintf
#include <string.h> // For strlen

// SPI handle - to be passed during init
static SPI_HandleTypeDef *g_hspi_touch_ptr;

Matrix touchMatrix = {0}; // Initialize to ensure Divider is 0 if not calibrated

// Default calibration points on LCD (can be adjusted)
Coordinate DefaultCalDisplaySample[3] = {
    {40, 40},                         // Top-left
    {LCD_WIDTH - 40, LCD_HEIGHT / 2}, // Mid-right (e.g., 200, 160 for 240x320)
    {LCD_WIDTH / 2, LCD_HEIGHT - 40}  // Bottom-mid (e.g., 120, 280 for 240x320)
};
Coordinate CalScreenSample[3]; // Raw ADC values from touch panel for calibration points

// Low-level SPI communication for touch
// Sends 1 byte command, receives 2 bytes data (12-bit ADC value)
static uint16_t TP_Read_ADC_Raw(uint8_t cmd)
{
    uint8_t tx_byte = cmd;
    uint8_t rx_buffer[2] = {0, 0};
    uint16_t adc_val = 0;

    HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_RESET); // Select touch controller

    // Send command byte
    HAL_SPI_Transmit(g_hspi_touch_ptr, &tx_byte, 1, HAL_MAX_DELAY);

    // Receive 2 data bytes. Some controllers might need dummy clocks before data is valid.
    // For XPT2046/ADS7846, data usually follows directly.
    HAL_SPI_Receive(g_hspi_touch_ptr, rx_buffer, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET); // Deselect

    adc_val = (rx_buffer[0] << 8) | rx_buffer[1];
    adc_val >>= 3; // Keep the most significant 12 bits (XPT2046 specific, D0 is at bit 3 of second byte)
                   // Original code used >>4, which might be for a different ADC alignment or resolution.
                   // If raw values are consistently too low or high, this shift might need adjustment.
    return adc_val;
}

void Manual_Touch_Init(SPI_HandleTypeDef *hspi_touch)
{
    g_hspi_touch_ptr = hspi_touch;
    // GPIOs for TP_CS (PA9) and TP_IRQ (PB4) are assumed to be initialized
    // in main.c's MX_GPIO_Init() as Output PP and Input Pull-up respectively.
    HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET); // Deselect touch controller initially
}

uint8_t Manual_Touch_Pressed(void)
{
    // TP_IRQ_Pin is low when pressed (assuming pull-up resistor is enabled)
    return (HAL_GPIO_ReadPin(TP_IRQ_GPIO_Port, TP_IRQ_Pin) == GPIO_PIN_RESET);
}

uint8_t Manual_Touch_GetRawPoint(Coordinate *rawPoint)
{
    // Đọc một mẫu đơn giản để debug
    rawPoint->x = TP_Read_ADC_Raw(TP_CMD_READ_X);
    rawPoint->y = TP_Read_ADC_Raw(TP_CMD_READ_Y);

    // Trả về thành công mà không kiểm tra
    return 1;
}

// uint8_t Manual_Touch_GetRawPoint(Coordinate *rawPoint)
// {
//     // if (!Manual_Touch_Pressed())
//     // {
//     //     return 0; // Not pressed
//     // }

//     uint16_t x_samples[TOUCH_AVG_SAMPLES];
//     uint16_t y_samples[TOUCH_AVG_SAMPLES];
//     uint8_t i;

//     // Read all X samples first
//     for (i = 0; i < TOUCH_AVG_SAMPLES; i++)
//     {
//         // if (!Manual_Touch_Pressed() && i >= 1)
//         if (i >= 1)
//         { // Check if pen lifted after at least one sample
//             // If pen lifted early, try to use what we have if enough samples, or fail
//             if (i < 3)
//                 return 0; // Not enough samples for even one average
//             // Fill remaining samples with the last good one to avoid distorting average too much
//             for (uint8_t j = i; j < TOUCH_AVG_SAMPLES; j++)
//             {
//                 x_samples[j] = x_samples[i - 1];
//             }
//             break; // Exit X sampling loop
//         }
//         x_samples[i] = TP_Read_ADC_Raw(TP_CMD_READ_X);
//         // HAL_Delay(1); // Increased delay slightly
//     }

//     // Read all Y samples
//     for (i = 0; i < TOUCH_AVG_SAMPLES; i++)
//     {
//         // if (!Manual_Touch_Pressed() && i >= 1)
//         if (i >= 1)
//         {
//             if (i < 3)
//                 return 0;
//             for (uint8_t j = i; j < TOUCH_AVG_SAMPLES; j++)
//             {
//                 y_samples[j] = y_samples[i - 1];
//             }
//             break; // Exit Y sampling loop
//         }
//         y_samples[i] = TP_Read_ADC_Raw(TP_CMD_READ_Y);
//         // HAL_Delay(2); // Increased delay slightly
//     }

//     // Averaging and filtering logic from original code
//     uint16_t avg_x[3], avg_y[3];
//     avg_x[0] = (x_samples[0] + x_samples[1] + x_samples[2]) / 3;
//     avg_x[1] = (x_samples[3] + x_samples[4] + x_samples[5]) / 3;
//     avg_x[2] = (x_samples[6] + x_samples[7] + x_samples[8]) / 3;

//     avg_y[0] = (y_samples[0] + y_samples[1] + y_samples[2]) / 3;
//     avg_y[1] = (y_samples[3] + y_samples[4] + y_samples[5]) / 3;
//     avg_y[2] = (y_samples[6] + y_samples[7] + y_samples[8]) / 3;

//     int16_t m0x = avg_x[0] > avg_x[1] ? avg_x[0] - avg_x[1] : avg_x[1] - avg_x[0];
//     int16_t m1x = avg_x[1] > avg_x[2] ? avg_x[1] - avg_x[2] : avg_x[2] - avg_x[1];
//     int16_t m2x = avg_x[2] > avg_x[0] ? avg_x[2] - avg_x[0] : avg_x[0] - avg_x[2];

//     if (m0x > TOUCH_RAW_THRESHOLD && m1x > TOUCH_RAW_THRESHOLD && m2x > TOUCH_RAW_THRESHOLD)
//         return 0; // Unstable X

//     if (m0x < m1x)
//         rawPoint->x = (m2x < m0x) ? (avg_x[0] + avg_x[2]) / 2 : (avg_x[0] + avg_x[1]) / 2;
//     else
//         rawPoint->x = (m2x < m1x) ? (avg_x[0] + avg_x[2]) / 2 : (avg_x[1] + avg_x[2]) / 2;

//     int16_t m0y = avg_y[0] > avg_y[1] ? avg_y[0] - avg_y[1] : avg_y[1] - avg_y[0];
//     int16_t m1y = avg_y[1] > avg_y[2] ? avg_y[1] - avg_y[2] : avg_y[2] - avg_y[1];
//     int16_t m2y = avg_y[2] > avg_y[0] ? avg_y[2] - avg_y[0] : avg_y[0] - avg_y[2];

//     if (m0y > TOUCH_RAW_THRESHOLD && m1y > TOUCH_RAW_THRESHOLD && m2y > TOUCH_RAW_THRESHOLD)
//         return 0; // Unstable Y

//     if (m0y < m1y)
//         rawPoint->y = (m2y < m0y) ? (avg_y[0] + avg_y[2]) / 2 : (avg_y[0] + avg_y[1]) / 2;
//     else
//         rawPoint->y = (m2y < m1y) ? (avg_y[0] + avg_y[2]) / 2 : (avg_y[1] + avg_y[2]) / 2;

//     return 1; // Valid point obtained
// }

// Calibration Matrix Calculation (from original code)
static uint8_t CalculateCalibrationMatrix(const Coordinate *displayPtr, const Coordinate *screenPtr, Matrix *matrixPtr)
{
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
    if (matrixPtr->Divider == 0)
    {
        return 0; // Error, division by zero
    }
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y;
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y));
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x));
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
    return 1; // Success
}

uint8_t Manual_Touch_ApplyCalibration(Coordinate *displayPoint, const Coordinate *screenPoint)
{
    if (touchMatrix.Divider == 0)
    { // Not calibrated or calibration failed
        // Fallback: direct scaling (highly inaccurate without calibration)
        displayPoint->x = (uint16_t)(((long)screenPoint->x * LCD_WIDTH) / 4095);
        displayPoint->y = (uint16_t)(((long)screenPoint->y * LCD_HEIGHT) / 4095);
        return 0; // Indicated that calibration matrix was not used
    }

    // Apply calibration matrix
    displayPoint->x = (uint16_t)(((touchMatrix.An * screenPoint->x) +
                                  (touchMatrix.Bn * screenPoint->y) +
                                  touchMatrix.Cn) /
                                 touchMatrix.Divider);
    displayPoint->y = (uint16_t)(((touchMatrix.Dn * screenPoint->x) +
                                  (touchMatrix.En * screenPoint->y) +
                                  touchMatrix.Fn) /
                                 touchMatrix.Divider);

    // Constrain to LCD boundaries
    if (displayPoint->x >= LCD_WIDTH)
        displayPoint->x = LCD_WIDTH - 1;
    if (displayPoint->y >= LCD_HEIGHT)
        displayPoint->y = LCD_HEIGHT - 1;
    // Note: long double might produce negative intermediate results if raw values are way off.
    // Casting to uint16_t will wrap. Consider clamping if signed intermediates are possible.
    // For typical touch values, this should be fine.

    return 1; // Calibration matrix used
}

static void DrawCalibrationCross(uint16_t x, uint16_t y, uint16_t color)
{
    Manual_LCD_DrawLine(x - 12, y, x + 12, y, color);
    Manual_LCD_DrawLine(x, y - 12, x, y + 12, color);
    Manual_LCD_DrawCircle(x, y, 5, color); // Small circle at center
}

void Manual_Touch_Calibrate(uint16_t charColor, uint16_t bkColor)
{
    Coordinate rawPoint;
    char msg_buffer[50];
    uint8_t i;

    Manual_LCD_Clear(bkColor);
    sprintf(msg_buffer, "Touch Calibration");
    Manual_LCD_DrawString((LCD_WIDTH - strlen(msg_buffer) * 6) / 2, LCD_HEIGHT / 2 - 30, msg_buffer, charColor, bkColor, 1);
    sprintf(msg_buffer, "Touch crosshairs");
    Manual_LCD_DrawString((LCD_WIDTH - strlen(msg_buffer) * 6) / 2, LCD_HEIGHT / 2 - 15, msg_buffer, charColor, bkColor, 1);
    HAL_Delay(1000);

    for (i = 0; i < 3; i++)
    {
        DrawCalibrationCross(DefaultCalDisplaySample[i].x, DefaultCalDisplaySample[i].y, charColor);
        // Wait for touch
        while (1)
        {
            if (Manual_Touch_GetRawPoint(&rawPoint))
            {
                CalScreenSample[i].x = rawPoint.x;
                CalScreenSample[i].y = rawPoint.y;
                break; // Got a point
            }
            HAL_Delay(50); // Polling delay
        }
        DrawCalibrationCross(DefaultCalDisplaySample[i].x, DefaultCalDisplaySample[i].y, bkColor); // Erase crosshair
        HAL_Delay(300);                                                                            // Debounce/wait for finger lift
    }

    if (!CalculateCalibrationMatrix(&DefaultCalDisplaySample[0], &CalScreenSample[0], &touchMatrix))
    {
        Manual_LCD_Clear(bkColor);
        Manual_LCD_DrawString(10, LCD_HEIGHT / 2, "Calibration FAILED!", COLOR_RED, bkColor, 2);
        // You might want to loop here or use default (bad) calibration
        touchMatrix.Divider = 0; // Mark as failed
        HAL_Delay(3000);
    }
    else
    {
        Manual_LCD_Clear(bkColor);
        Manual_LCD_DrawString(10, LCD_HEIGHT / 2, "Calibration OK!", COLOR_GREEN, bkColor, 2);
        HAL_Delay(1500);
    }
    // The main layout will be redrawn by the main loop after this.
}

void Manual_Touch_SetFixedCalibration(const Coordinate CalDisplay[], const Coordinate CalScreen[])
{
    if (!CalculateCalibrationMatrix(&CalDisplay[0], &CalScreen[0], &touchMatrix))
    {
        touchMatrix.Divider = 0; // Mark as failed if fixed values are bad
    }
}
