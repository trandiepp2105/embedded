/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "manual_lcd.h" // Include the manual LCD driver
#include "manual_touch.h"
#include <stdio.h>
#include <string.h> // Cho hàm strlen() và strcmp()

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

int main(void)
{

  /* USER CODE BEGIN 1 */
  // Task Box Definitions
  uint16_t margin = 15;
  uint16_t spacing_between_boxes = 10;
  // Assuming LCD_WIDTH is 240 and LCD_HEIGHT is 320 from manual_lcd.h
  // If LCD_WIDTH is not 240, task_box_w calculation will be incorrect.
  uint16_t task_box_w = (LCD_WIDTH - 2 * margin - spacing_between_boxes) / 2;
  uint16_t task_box_h = 70;
  uint16_t info_box_y = 10;
  uint16_t info_box_h = 30;
  uint16_t start_y_tasks_row1 = info_box_y + info_box_h + 20;
  uint16_t start_y_tasks_row2 = start_y_tasks_row1 + task_box_h + spacing_between_boxes;

  TaskBox_t task_boxes[] = {
      {margin, start_y_tasks_row1, task_box_w, task_box_h, "Task 02-1"},
      {(uint16_t)(margin + task_box_w + spacing_between_boxes), start_y_tasks_row1, task_box_w, task_box_h, "Task 02-2"},
      {margin, start_y_tasks_row2, task_box_w, task_box_h, "Task 02-3"},
      {(uint16_t)(margin + task_box_w + spacing_between_boxes), start_y_tasks_row2, task_box_w, task_box_h, "Task 02-4"}};
  const int num_tasks = sizeof(task_boxes) / sizeof(task_boxes[0]);
  int highlighted_task_index = -1;
  char info_text_buffer[50];
  Coordinate rawPoint, displayPoint;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C2_Init();
  Manual_LCD_Init();
  Manual_Touch_Init(&hspi1);

  // Temporarily COMMENT OUT calibration for testing basic touch functionality
  /*
  Manual_LCD_Clear(COLOR_BLACK);                                                                        // Clear screen before calibration messages
  Manual_LCD_DrawString(10, LCD_HEIGHT / 2 - 20, "Calibrating Touch...", COLOR_WHITE, COLOR_BLACK, 1);  // Adjusted Y for better spacing
  Manual_LCD_DrawString(10, LCD_HEIGHT / 2, "Tap points as they appear.", COLOR_WHITE, COLOR_BLACK, 1); // Adjusted Y
  HAL_Delay(1500);                                                                                      // Give user a bit more time to read message
  Manual_Touch_Calibrate(COLOR_YELLOW, COLOR_BLACK); // This is where it might hang
  Manual_LCD_Clear(COLOR_BLACK); // Clear screen after calibration UI
  */

  Manual_LCD_Clear(COLOR_BLACK); // Clear the screen for the test
  Manual_LCD_DrawLayout();       // Draw the main layout (or a simpler test screen if preferred)

  // Initial Info Text for testing
  sprintf(info_text_buffer, "Touch Test Mode");
  Manual_LCD_UpdateInfoText(info_text_buffer);

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (Manual_Touch_Pressed())
    {
      if (Manual_Touch_GetRawPoint(&rawPoint))
      {
        // Display raw touch coordinates in the info box
        sprintf(info_text_buffer, "Raw: X=%03u Y=%03u", rawPoint.x, rawPoint.y);
        Manual_LCD_UpdateInfoText(info_text_buffer);

        // Wait for touch release to prevent continuous updates for a single press
        while (Manual_Touch_Pressed())
        {
          HAL_Delay(20); // Small delay
        }
        // Optionally, update info text after release
        sprintf(info_text_buffer, "Released. Tap again.");
        Manual_LCD_UpdateInfoText(info_text_buffer);
        HAL_Delay(500);                               // Show release message briefly
        sprintf(info_text_buffer, "Touch Test Mode"); // Revert to default test message
        Manual_LCD_UpdateInfoText(info_text_buffer);
      }
      else
      {
        // Manual_Touch_Pressed() was true, but GetRawPoint failed
        sprintf(info_text_buffer, "Pressed, GetPoint Fail");
        Manual_LCD_UpdateInfoText(info_text_buffer);
        while (Manual_Touch_Pressed())
        {
          HAL_Delay(20);
        }
      }
    }
    HAL_Delay(50); // Polling delay for touch checks
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_RST_Pin | LCD_BL_Pin | LCD_CS_Pin | LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level for TP_CS to be deselected initially */
  HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET); // TP_CS high (deselected)

  /*Configure GPIO pin : PA1 (Nếu bạn không dùng, có thể bỏ qua) */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RST_Pin LCD_BL_Pin LCD_CS_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin | LCD_BL_Pin | LCD_CS_Pin | LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // Tăng tốc độ cho các chân LCD SPI
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_CS_Pin */
  GPIO_InitStruct.Pin = TP_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // Tăng tốc độ cho TP_CS
  HAL_GPIO_Init(TP_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_IRQ_Pin (Cấu hình cho polling) */
  GPIO_InitStruct.Pin = TP_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // Input để đọc trạng thái
  GPIO_InitStruct.Pull = GPIO_PULLUP;     // Kéo lên vì TP_IRQ là active-low
  HAL_GPIO_Init(TP_IRQ_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init - Bỏ qua nếu dùng polling cho TP_IRQ */
  /*
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  */

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
