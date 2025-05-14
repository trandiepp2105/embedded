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

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  // Task Box Definitions
  uint16_t start_y_tasks_row1 = INFO_BOX_Y + INFO_BOX_H + INFO_TO_TASK_SPACING;
  uint16_t start_y_tasks_row2 = start_y_tasks_row1 + TASK_BOX_H + SPACING_BETWEEN_BOXES;

  TaskBox_t task_boxes[] = {
      {MARGIN, start_y_tasks_row1, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-1"},
      {(uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES), start_y_tasks_row1, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-2"},
      {MARGIN, start_y_tasks_row2, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-3"},
      {(uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES), start_y_tasks_row2, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-4"}};
  const int num_tasks = sizeof(task_boxes) / sizeof(task_boxes[0]);
  // int highlighted_task_index = -1; // Not currently used
  char info_text_buffer[50];
  char current_task_text[20];
  Coordinate rawPoint, displayPoint;

  // Define the Back Button Box (ADJUST x, y, w, h AS PER YOUR Manual_LCD_DrawLayout)
  TaskBox_t back_button_box = {
      (uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES), // x: same margin as tasks
      (uint16_t)(start_y_tasks_row2 + TASK_BOX_H + TASK_TO_BACK_BUTTON_SPACING),        // y: towards the bottom
      BACK_BUTTON_W,                                           // w: span most of the width
      BACK_BUTTON_H,                                           // h: defined height
      "Back"                                                   // name (used for internal logic if needed, not displayed by default)
  };
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

  Manual_LCD_Clear(COLOR_BLACK);
  Manual_LCD_DrawLayout(); // This function should draw the tasks and the back button

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (Manual_Touch_Pressed())
    {
      if (Manual_Touch_GetRawPoint(&rawPoint))
      {
        Manual_Touch_ApplyCalibration(&displayPoint, &rawPoint);

        int task_hit_index = -1;
        for (int i = 0; i < num_tasks; i++)
        {
          if (displayPoint.x >= task_boxes[i].x && displayPoint.x < (task_boxes[i].x + task_boxes[i].w) &&
              displayPoint.y >= task_boxes[i].y && displayPoint.y < (task_boxes[i].y + task_boxes[i].h))
          {
            task_hit_index = i;
            break;
          }
        }

        bool back_button_pressed = false;
        if (displayPoint.x >= back_button_box.x && displayPoint.x < (back_button_box.x + back_button_box.w) &&
            displayPoint.y >= back_button_box.y && displayPoint.y < (back_button_box.y + back_button_box.h))
        {
          back_button_pressed = true;
        }

        if (task_hit_index != -1)
        {
          // reset fill color of previous task box(using current_task_text)
          if (strlen(current_task_text) > 0)
          {
            if (strcmp(current_task_text, task_boxes[task_hit_index].name) != 0)
            {
              Manual_LCD_RefillTaskBox(current_task_text, COLOR_BLACK);                 // Refill the task box with red color
              Manual_LCD_RefillTaskBox(task_boxes[task_hit_index].name, COLOR_MAGENTA); // Refill the task box with red color
            }
          }
          else
          {
            Manual_LCD_RefillTaskBox(task_boxes[task_hit_index].name, COLOR_MAGENTA); // Refill the task box with red color
          }
          strcpy(current_task_text, task_boxes[task_hit_index].name); // Update current task text

          // sprintf(info_text_buffer, "Task: %s", task_boxes[task_hit_index].name);
        }
        else if (back_button_pressed)
        {
          sprintf(info_text_buffer, "Back"); // Display "Back" when back button is touched
        }
        else
        {
          sprintf(info_text_buffer, "Touch: X=%03u Y=%03u", displayPoint.x, displayPoint.y);
        }
        Manual_LCD_UpdateInfoText(info_text_buffer);

        while (Manual_Touch_Pressed())
        {
          HAL_Delay(20);
        }
      }
      else
      {
        sprintf(info_text_buffer, "Pressed, GetPoint Fail");
        Manual_LCD_UpdateInfoText(info_text_buffer);
        while (Manual_Touch_Pressed())
        {
          HAL_Delay(20);
        }
      }
    }
    HAL_Delay(50);
    /* USER CODE END 3 */
  }
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
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
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
  HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PA1 (If not used, can be omitted) */
  // GPIO_InitStruct.Pin = GPIO_PIN_1;
  // GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  // GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RST_Pin LCD_BL_Pin LCD_CS_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin | LCD_BL_Pin | LCD_CS_Pin | LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_CS_Pin */
  GPIO_InitStruct.Pin = TP_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TP_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_IRQ_Pin (Configured for polling) */
  GPIO_InitStruct.Pin = TP_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TP_IRQ_GPIO_Port, &GPIO_InitStruct);

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
