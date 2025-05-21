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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "manual_lcd.h" // Include the manual LCD driver
#include "manual_touch.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h> // Cho hàm strlen() và strcmp()
#include "stm32f4xx_hal.h"
#include "task2.h"
#include "sd_card_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

uint8_t read_data[50], byte_read;
SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypeDef SDCardInfo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

I2C_HandleTypeDef hi2c2;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader1;
CAN_RxHeaderTypeDef RxHeader2;

uint8_t TxData1[2]; // Sẽ chứa [số nhóm (1 byte)] + [nhiệt độ (1 bytes)]
uint8_t RxData2[2];

uint32_t TxMailbox1;

volatile uint8_t can2_received_temperature;
volatile uint8_t can2_received_group;
volatile uint8_t can2_received_data_flag = 0;

volatile int current_task = -1;
volatile uint8_t touch_detected = 0; // Flag to indicate touch interrupt has occurred
GPIO_PinState current_button_state = GPIO_PIN_RESET;
// Global variables for touch interface
TaskBox_t task_boxes[4];
TaskBox_t back_button_box;
int num_tasks;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

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

  // Initialize task boxes (now global)
  task_boxes[0] = (TaskBox_t){MARGIN, start_y_tasks_row1, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-1"};
  task_boxes[1] = (TaskBox_t){(uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES), start_y_tasks_row1, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-2"};
  task_boxes[2] = (TaskBox_t){MARGIN, start_y_tasks_row2, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-3"};
  task_boxes[3] = (TaskBox_t){(uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES), start_y_tasks_row2, TASK_BOX_W_CALC, TASK_BOX_H, "Task 02-4"};

  num_tasks = 4; // Set global count

  char info_text_buffer[50];
  char current_task_text[20];
  Coordinate rawPoint, displayPoint;

  // Define the Back Button Box (now global)
  back_button_box = (TaskBox_t){
      (uint16_t)(MARGIN + TASK_BOX_W_CALC + SPACING_BETWEEN_BOXES),              // x: same margin as tasks
      (uint16_t)(start_y_tasks_row2 + TASK_BOX_H + TASK_TO_BACK_BUTTON_SPACING), // y: towards the bottom
      BACK_BUTTON_W,                                                             // w: span most of the width
      BACK_BUTTON_H,                                                             // h: defined height
      "Back"                                                                     // name (used for internal logic if needed, not displayed by default)
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
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  /* USER CODE BEGIN 2 */
  // Bật backlight trước khi khởi tạo LCD
  HAL_GPIO_WritePin(GPIOB, LCD_BL_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  // Cấu hình filter cho CAN1
  // CAN_FilterTypeDef sFilterConfig1;
  // sFilterConfig1.FilterBank = 0; // CAN1 sử dụng bank 0-13
  // sFilterConfig1.FilterMode = CAN_FILTERMODE_IDMASK;
  // sFilterConfig1.FilterScale = CAN_FILTERSCALE_32BIT;
  // sFilterConfig1.FilterIdHigh = 0x0000;
  // sFilterConfig1.FilterIdLow = 0x0000;
  // sFilterConfig1.FilterMaskIdHigh = 0x0000;
  // sFilterConfig1.FilterMaskIdLow = 0x0000;
  // sFilterConfig1.FilterFIFOAssignment = CAN_RX_FIFO1;
  // sFilterConfig1.FilterActivation = ENABLE;
  // sFilterConfig1.SlaveStartFilterBank = 14; // Cấu hình một lần duy nhất

  // if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig1) != HAL_OK)
  // {
  //   Error_Handler();
  // }

  // Cấu hình filter cho CAN2
  CAN_FilterTypeDef sFilterConfig2;
  sFilterConfig2.FilterBank = 14; // CAN2 sử dụng từ bank 14-27
  sFilterConfig2.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig2.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig2.FilterIdHigh = 0x0000;
  sFilterConfig2.FilterIdLow = 0x0000;
  sFilterConfig2.FilterMaskIdHigh = 0x0000;
  sFilterConfig2.FilterMaskIdLow = 0x0000;
  sFilterConfig2.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig2.FilterActivation = ENABLE;
  // Không cần đặt SlaveStartFilterBank ở đây

  if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig2) != HAL_OK)
  {
    Error_Handler();
  }

  // Khởi động CAN1 và CAN2
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }

  // Kích hoạt thông báo ngắt cho CAN2 RX FIFO0
  if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

  // Cấu hình thông điệp gửi từ CAN1
  TxHeader1.StdId = 0x124;
  TxHeader1.ExtId = 0x00;
  TxHeader1.RTR = CAN_RTR_DATA;
  TxHeader1.IDE = CAN_ID_STD;
  TxHeader1.DLC = 2;
  TxHeader1.TransmitGlobalTime = DISABLE;

  // Khởi tạo LCD và Touch
  // disable touch interrupt
  HAL_NVIC_DisableIRQ(EXTI4_IRQn); // Tắt ngắt trước khi khởi tạo

  Manual_LCD_Init();
  Manual_Touch_Init(&hspi1);
  Manual_LCD_DrawLayout(); // This function should draw the tasks and the back button

  if (SD_Init())
  {
	 sprintf(info_text_buffer, "SD OK");

    // Tạo file team9.txt
//    if (SD_CreateTeamFile())
//    {
//      sprintf(info_text_buffer, "SD OK, File created");
//    }
//    else
//    {
//      sprintf(info_text_buffer, "SD OK, File error");
//    }
  }
  else
  {
    sprintf(info_text_buffer, "SD Init Failed");
  }

  Manual_LCD_UpdateInfoText(info_text_buffer);

  __HAL_GPIO_EXTI_CLEAR_IT(TP_IRQ_Pin);

  HAL_NVIC_EnableIRQ(EXTI4_IRQn); // Enable touch interrupt

  //   /* USER CODE END 4 */
  // }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* USER CODE BEGIN WHILE */
  int can2_received_temperature_int;
  while (1)
    {
      /* USER CODE END WHILE */
      if (current_task == 0)
      {
        // Task 02-1: Blink LED
        Task2_LedBlink(GPIOB, GPIO_PIN_1, BLINK_SPEED_1000_MS);
      }
      else if (current_task == 1)
      {
        char temp_display_str[50];
        uint8_t current_temp_c = (uint8_t)Read_Internal_Temperature();
        uint8_t temp_to_send = current_temp_c;
        TxData1[0] = GROUP_NUMBER;
        TxData1[1] = temp_to_send;
        HAL_CAN_AddTxMessage(&hcan1, &TxHeader1, TxData1, &TxMailbox1);
        while (!can2_received_data_flag)
        {
          HAL_Delay(10);
        }
        can2_received_data_flag = 0; // Reset the flag for the next message
	    can2_received_temperature_int = (int)can2_received_temperature;

        sprintf(temp_display_str, "Group: %d, temp: %d *C", can2_received_group, can2_received_temperature_int);
        // sprintf(temp_display_str, "Group: %d, temp: %d *C", GROUP_NUMBER, temp_to_send);
        strcpy(info_text_buffer, temp_display_str);
        Manual_LCD_UpdateInfoText(info_text_buffer);
        HAL_Delay(500); // Delay to avoid flooding the CAN bus
      }
      else if (current_task == 2)
      {
        byte_read = SD_ReadTeamFile(read_data, sizeof(read_data));

        if (byte_read > 0)
        {
          // Đọc thành công, hiển thị nội dung
          sprintf(info_text_buffer, "SD content: %s", read_data);
          Manual_LCD_UpdateInfoText(info_text_buffer);
        }
        else
        {
          // Đọc thất bại
          sprintf(info_text_buffer, "Read error (%d)", byte_read);
          Manual_LCD_UpdateInfoText(info_text_buffer);

          // Thử khởi tạo lại SD trong trường hợp bị ngắt kết nối
          if (SD_Init())
          {
            sprintf(info_text_buffer, "SD reinitialized");
            Manual_LCD_UpdateInfoText(info_text_buffer);
            HAL_Delay(1000);

            // Thử đọc lại
            byte_read = SD_ReadTeamFile(read_data, sizeof(read_data));
            if (byte_read > 0)
            {
              sprintf(info_text_buffer, "Content: %s", read_data);
              Manual_LCD_UpdateInfoText(info_text_buffer);
            }
            else
            {
              sprintf(info_text_buffer, "Still error (%d)", byte_read);
              Manual_LCD_UpdateInfoText(info_text_buffer);
            }
          }
        }

        HAL_Delay(500); // Delay to avoid flooding the LCD
      }
      else if (current_task == 3)
      {
        // Task 02-4: Read temperature from FRAM
        uint8_t read_value = 0;
        // Đọc nhiệt độ từ FRAM
        GPIO_PinState prev_btn_state = current_button_state;

        CheckUserButtonAndSaveTemp(&current_button_state);
        // Kiểm tra trạng thái nút và ghi nhiệt độ vào FRAM
        if (current_button_state == GPIO_PIN_SET && prev_btn_state == GPIO_PIN_RESET)
        {
          // Nút vừa được nhấn xuống
          if (FRAM_ReadBytes(&hi2c2, USER_TEMP_ADDR, &read_value, 1) == HAL_OK)
          {
            sprintf(info_text_buffer, "FRAM Read OK: %d", read_value);
          }
          else
          {
            sprintf(info_text_buffer, "FRAM Read ERROR");
          }
          Manual_LCD_UpdateInfoText(info_text_buffer);
        }

        HAL_Delay(10); // Delay to avoid flooding the LCD



      }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 72;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 72;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE END CAN2_Init 2 */

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
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|LCD_RST_Pin|LCD_BL_Pin|LCD_CS_Pin
                          |LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 LCD_RST_Pin LCD_BL_Pin LCD_CS_Pin
                           LCD_DC_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LCD_RST_Pin|LCD_BL_Pin|LCD_CS_Pin
                          |LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_CS_Pin */
  GPIO_InitStruct.Pin = TP_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TP_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_IRQ_Pin */
  GPIO_InitStruct.Pin = TP_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TP_IRQ_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == TP_IRQ_Pin)
  {

    // Kiểm tra có phải là chạm không
    if (HAL_GPIO_ReadPin(TP_IRQ_GPIO_Port, TP_IRQ_Pin) == GPIO_PIN_RESET)
    {
      Coordinate rawPoint, displayPoint;
      uint8_t old_task = current_task; // Lưu task cũ

      if (Manual_Touch_GetRawPoint(&rawPoint))
      {
        Manual_Touch_ApplyCalibration(&displayPoint, &rawPoint);

        // Kiểm tra xem có chạm vào task box nào không
        for (int i = 0; i < num_tasks; i++)
        {
          if (displayPoint.x >= task_boxes[i].x &&
              displayPoint.x < (task_boxes[i].x + task_boxes[i].w) &&
              displayPoint.y >= task_boxes[i].y &&
              displayPoint.y < (task_boxes[i].y + task_boxes[i].h))
          {
            current_task = i; // Đặt current_task thành index của task box
            break;
          }
        }

        // Kiểm tra xem có chạm vào nút Back không
        if (displayPoint.x >= back_button_box.x &&
            displayPoint.x < (back_button_box.x + back_button_box.w) &&
            displayPoint.y >= back_button_box.y &&
            displayPoint.y < (back_button_box.y + back_button_box.h))
        {
          current_task = 4; // Nút Back
        }

        // Nếu task thay đổi, vẽ lại màn hình ngay
        if (old_task != current_task)
        {
          // Tắt ngắt trước khi vẽ
          HAL_NVIC_DisableIRQ(EXTI4_IRQn);

          // Xử lý riêng cho các task thông thường và nút back
          if (current_task >= 0 && current_task < num_tasks)
          {
            // Khôi phục màu của task cũ nếu có
            if (old_task >= 0 && old_task < num_tasks)
            {
              Manual_LCD_RefillTaskBox(task_boxes[old_task].name, COLOR_BLACK);
            }

            // Tô màu task mới được chọn
            Manual_LCD_RefillTaskBox(task_boxes[current_task].name, COLOR_MAGENTA);
          }
          else if (current_task == 4 && old_task != -1) // Back button
          {
            // Handle back button action here
            Manual_LCD_RefillTaskBox(task_boxes[old_task].name, COLOR_BLACK);
            current_task = -1; // Reset current task value
          }

          // // Reset CS pins và cấu hình SPI cho Touch
          // HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); // Deselect LCD
          // HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET);   // Deselect Touch

          // // Reset cấu hình SPI cho Touch
          // hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
          // hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
          // hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
          // HAL_SPI_Init(&hspi1);

          // Xóa cờ ngắt và bật lại
          __HAL_GPIO_EXTI_CLEAR_IT(TP_IRQ_Pin);
          HAL_NVIC_EnableIRQ(EXTI4_IRQn);
        }
      }
    }
    // __HAL_GPIO_EXTI_CLEAR_IT(TP_IRQ_Pin);
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if (hcan->Instance == CAN2)
  {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader2, RxData2) == HAL_OK)
    {
      // Giải mã dữ liệu nhận được (Tùy chọn, để debug)
      if (RxHeader2.DLC >= 2) // Đảm bảo có đủ dữ liệu
      {
    	can2_received_group = RxData2[0];
        can2_received_temperature = RxData2[1];

        can2_received_data_flag = 1; // Đánh dấu đã nhận dữ liệu
}
    }
  }
}

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

#ifdef  USE_FULL_ASSERT
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
