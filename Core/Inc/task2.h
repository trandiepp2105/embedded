/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_TASK2_H_ // Đổi tên include guard thành INC_TASK2_H_
#define INC_TASK2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h" // Hoặc include cần thiết khác

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define BLINK_SPEED_200_MS    200
#define BLINK_SPEED_500_MS  500
#define BLINK_SPEED_1000_MS    1000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief Khởi tạo cho Task 2 (hiện tại liên quan đến LED).
  * @note Có thể để trống nếu cấu hình GPIO đã đủ trong MX_GPIO_Init().
  * @retval None
  */
void Task2_Init(void); // Đổi tên hàm Init (tùy chọn, nhưng rõ ràng hơn)

/**
  * @brief Thực hiện một lần nháy (toggle) LED và delay cho Task 2.
  * @note Để nhấp nháy liên tục, gọi hàm này trong vòng lặp.
  *       Yêu cầu chân GPIO đã được cấu hình là Output Push-Pull.
  * @param ledPort: Con trỏ tới port GPIO (ví dụ: GPIOB).
  * @param ledPin: Số hiệu pin (ví dụ: GPIO_PIN_2).
  * @param blinkSpeed_ms: Thời gian LED giữ một trạng thái (sáng hoặc tắt) tính bằng ms.
  * @retval None
  */
void Task2_LedBlink(GPIO_TypeDef* ledPort, uint16_t ledPin, uint32_t blinkSpeed_ms); // Đổi tên hàm (tùy chọn)

/**
  * @brief Bật sáng LED cho Task 2.
  * @note Yêu cầu chân GPIO đã được cấu hình là Output Push-Pull.
  * @param ledPort: Con trỏ tới port GPIO (ví dụ: GPIOB).
  * @param ledPin: Số hiệu pin (ví dụ: GPIO_PIN_2).
  * @retval None
  */
void Task2_LedTurnOn(GPIO_TypeDef* ledPort, uint16_t ledPin); // Đổi tên hàm (tùy chọn)

/**
  * @brief Tắt LED cho Task 2.
  * @note Yêu cầu chân GPIO đã được cấu hình là Output Push-Pull.
  * @param ledPort: Con trỏ tới port GPIO (ví dụ: GPIOB).
  * @param ledPin: Số hiệu pin (ví dụ: GPIO_PIN_2).
  * @retval None
  */
void Task2_LedTurnOff(GPIO_TypeDef* ledPort, uint16_t ledPin); // Đổi tên hàm (tùy chọn)


#endif /* INC_TASK2_H_ */
