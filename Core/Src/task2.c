/* Includes ------------------------------------------------------------------*/
#include "task2.h"    // Include file header tương ứng là task2.h
#include "fram_i2c.h" // Include file cho FRAM
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c2;

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
void Task2_LedBlink(GPIO_TypeDef *ledPort, uint16_t ledPin, uint32_t blinkSpeed_ms) // Sử dụng tên hàm mới (nếu đổi)
{
  HAL_GPIO_TogglePin(ledPort, ledPin);
  HAL_Delay(blinkSpeed_ms);
}

float Read_Internal_Temperature(void)
{
  uint32_t adc_value = 0;
  float temperature_celsius = 0.0f;

  HAL_ADC_Start(&hadc1);                                // Khởi động ADC1
  if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) // Chờ chuyển đổi, timeout 100ms
  {
    adc_value = HAL_ADC_GetValue(&hadc1); // Đọc giá trị ADC
  }
  HAL_ADC_Stop(&hadc1); // Dừng ADC1

  // Công thức tính nhiệt độ từ datasheet (có thể cần điều chỉnh)
  // Temperature(in °C) = {(V SENSE – V 25 ) / Avg_Slope} + 25
  // V SENSE = adc_value * VREF_MV / ADC_MAX_VALUE
  float vsense_mv = ((float)adc_value * ADC_VREF_MV) / ADC_MAX_VALUE;
  temperature_celsius = (vsense_mv - TEMP_SENSOR_V25_MV) / TEMP_SENSOR_AVG_SLOPE_MV_PER_C + 25.0f;

  return temperature_celsius;
}

/**
 * @brief Kiểm tra nút User Button và ghi nhiệt độ vào FRAM ngay lập tức khi nút được nhấn
 * @param previous_state Con trỏ đến biến lưu trạng thái nút trước đó
 * @retval None
 */
void CheckUserButtonAndSaveTemp(GPIO_PinState *previous_state)
{
  // Đọc trạng thái hiện tại của nút
  GPIO_PinState current_button_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);

  // Kiểm tra xem nút vừa được nhấn xuống (phát hiện cạnh xuống)
  if (current_button_state == GPIO_PIN_SET && *previous_state == GPIO_PIN_RESET)
  {
    // Nút vừa được nhấn xuống
    // Đọc nhiệt độ từ cảm biến
    uint8_t current_temp = (uint8_t)Read_Internal_Temperature();

    // Ghi nhiệt độ vào FRAM tại địa chỉ cố định, ghi đè lên giá trị cũ
    HAL_StatusTypeDef status = FRAM_WriteBytes(&hi2c2, USER_TEMP_ADDR, &current_temp, 1);

  }

  // Cập nhật trạng thái nút trước đó
  *previous_state = current_button_state;
}
