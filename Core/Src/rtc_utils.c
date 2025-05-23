/* Includes ------------------------------------------------------------------*/
#include "rtc_utils.h"
#include <stdio.h> // Cho printf (nếu bạn dùng để debug trong các hàm này)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Biến handle RTC thường được khai báo là extern từ main.c hoặc được truyền vào
// extern RTC_HandleTypeDef hrtc; // Nếu bạn muốn truy cập trực tiếp hrtc từ main.c
// Nhưng tốt hơn là truyền con trỏ vào hàm.

/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

/**
 * @brief Chuyển đổi số thập phân sang BCD.
 */
uint8_t My_RTC_DecToBcd(uint8_t val)
{
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}

/**
 * @brief Chuyển đổi số BCD sang thập phân.
 */
uint8_t My_RTC_BcdToDec(uint8_t val)
{
    return (uint8_t)(((val >> 4) * 10) + (val & 0x0F));
}

/**
 * @brief Cài đặt thời gian và ngày tháng cho RTC.
 */
HAL_StatusTypeDef My_RTC_SetDateTime(RTC_HandleTypeDef *hrtc_ptr,
                                     uint8_t hours_bcd, uint8_t minutes_bcd, uint8_t seconds_bcd,
                                     uint8_t day_bcd, uint8_t month_bcd, uint8_t year_bcd, uint8_t weekday)
{
    RTC_TimeTypeDef sTimeSet = {0};
    RTC_DateTypeDef sDateSet = {0};
    HAL_StatusTypeDef time_status, date_status;

    sTimeSet.Hours = hours_bcd;
    sTimeSet.Minutes = minutes_bcd;
    sTimeSet.Seconds = seconds_bcd;
    sTimeSet.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTimeSet.StoreOperation = RTC_STOREOPERATION_RESET;
    time_status = HAL_RTC_SetTime(hrtc_ptr, &sTimeSet, RTC_FORMAT_BCD);
    if (time_status != HAL_OK)
    {
        return time_status;
    }

    sDateSet.WeekDay = weekday;
    sDateSet.Month = month_bcd;
    sDateSet.Date = day_bcd;
    sDateSet.Year = year_bcd;
    date_status = HAL_RTC_SetDate(hrtc_ptr, &sDateSet, RTC_FORMAT_BCD);

    return date_status;
}

/**
 * @brief Lấy thời gian và ngày tháng hiện tại từ RTC.
 */
HAL_StatusTypeDef My_RTC_GetDateTime(RTC_HandleTypeDef *hrtc_ptr, My_RTC_DateTime_t *dateTimeDest)
{
    HAL_StatusTypeDef status;
    // Quan trọng: Phải gọi GetTime trước, sau đó mới gọi GetDate
    status = HAL_RTC_GetTime(hrtc_ptr, &(dateTimeDest->time), RTC_FORMAT_BCD);
    if (status != HAL_OK)
    {
        return status;
    }
    status = HAL_RTC_GetDate(hrtc_ptr, &(dateTimeDest->date), RTC_FORMAT_BCD);
    return status;
}

/**
 * @brief Khởi tạo và cài đặt RTC.
 */
void My_RTC_InitAndSet(RTC_HandleTypeDef *hrtc_ptr)
{
    // Giả sử MX_RTC_Init() đã được gọi trong main.c trước khi gọi hàm này.

    // Kiểm tra xem RTC đã được cấu hình trước đó chưa (chỉ hoạt động đúng nếu có Vbat)
    // Nếu không có Vbat, cờ này sẽ mất mỗi khi reset, nên thời gian sẽ luôn được đặt lại.
    if (HAL_RTCEx_BKUPRead(hrtc_ptr, RTC_BKP_DR0) != RTC_INIT_MAGIC_NUMBER)
    {
        printf("RTC: First time setup or Vbat lost. Setting default time...\r\n");

        // Đặt thời gian mặc định khi khởi tạo lần đầu
        // Ví dụ: 12:00:00, Thứ Hai, ngày 01 tháng 01 năm 2024
        uint8_t default_hours = My_RTC_DecToBcd(12);
        uint8_t default_minutes = My_RTC_DecToBcd(0);
        uint8_t default_seconds = My_RTC_DecToBcd(0);
        uint8_t default_day = My_RTC_DecToBcd(1);
        uint8_t default_month = RTC_MONTH_JANUARY; // Hoặc My_RTC_DecToBcd(1)
        uint8_t default_year = My_RTC_DecToBcd(24);
        uint8_t default_weekday = RTC_WEEKDAY_MONDAY;

        if (My_RTC_SetDateTime(hrtc_ptr, default_hours, default_minutes, default_seconds,
                               default_day, default_month, default_year, default_weekday) == HAL_OK)
        {
            printf("RTC: Default time set successfully.\r\n");
            // Ghi magic number vào Backup Register (chỉ có ý nghĩa nếu có Vbat)
            HAL_PWR_EnableBkUpAccess();
            HAL_RTCEx_BKUPWrite(hrtc_ptr, RTC_BKP_DR0, RTC_INIT_MAGIC_NUMBER);
            HAL_PWR_DisableBkUpAccess();
        }
        else
        {
            printf("RTC: Error setting default time.\r\n");
            // Error_Handler(); // Cân nhắc xử lý lỗi ở đây
        }
    }
    else
    {
        printf("RTC: Already configured.\r\n");
    }
}