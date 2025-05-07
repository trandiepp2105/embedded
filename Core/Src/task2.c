/* Includes ------------------------------------------------------------------*/
#include "task2.h" // Include file header tương ứng là task2.h

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

/**
  * @brief Khởi tạo cho Task 2.
  */
void Task2_Init(void) // Sử dụng tên hàm mới (nếu đổi)
{
  // Hiện tại để trống.
}

/**
  * @brief Thực hiện một lần nháy (toggle) LED và delay cho Task 2.
  */
void Task2_LedBlink(GPIO_TypeDef* ledPort, uint16_t ledPin, uint32_t blinkSpeed_ms) // Sử dụng tên hàm mới (nếu đổi)
{
  HAL_GPIO_TogglePin(ledPort, ledPin);
  HAL_Delay(blinkSpeed_ms);
}

/**
  * @brief Bật sáng LED cho Task 2.
  */
void Task2_LedTurnOn(GPIO_TypeDef* ledPort, uint16_t ledPin) // Sử dụng tên hàm mới (nếu đổi)
{
  HAL_GPIO_WritePin(ledPort, ledPin, GPIO_PIN_SET);
}

/**
  * @brief Tắt LED cho Task 2.
  */
void Task2_LedTurnOff(GPIO_TypeDef* ledPort, uint16_t ledPin) // Sử dụng tên hàm mới (nếu đổi)
{
  HAL_GPIO_WritePin(ledPort, ledPin, GPIO_PIN_RESET);
}
