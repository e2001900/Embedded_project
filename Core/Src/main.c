/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbusSlave.h"
#include <stdio.h>
#include <string.h>
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
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define SelectPinsPort GPIOA
const int SelectPins[3] = {S1_Pin, S2_Pin, S3_Pin};

uint8_t RxData[256];
uint8_t TxData[256];

float Holding_Registers_Database[5]={
		0.0,  0.0,  0.0,  0.0,  0.0,   // 40-49 40041-40050
};

float adc_temp = 0;
float last_cell = 0;
float raw_adc[5] = {0,0,0,0,0};
float Voltages[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
float current = 0;
float temperature = 0;
float temp = 0;
char buffer[100];

void selectMuxPin(int pin)
{
	for(int i = 0; i < 3; i++)
	{
		if(pin & (1 << i))
			HAL_GPIO_WritePin(SelectPinsPort, SelectPins[i], GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SelectPinsPort, SelectPins[i], GPIO_PIN_RESET);

	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (RxData[0] == SLAVE_ID)
	{
		switch (RxData[1]){
		case 0x03:
			readHoldingRegs();
			break;
		case 0x04:
			readInputRegs();
			break;
		default:
			break;
		}
	}

	HAL_UARTEx_ReceiveToIdle_IT(&huart2, RxData, 256);
}
/* USER CODE END 0 */
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
	return len;
}

void current_check(float current) {
	if (current > 1000){
		HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_SET);
	}
}

void temperature_check(float temp) {
	if ((temp < -20) || (temp > 40)) {
		HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_SET);
	}
}

void voltage_check(float voltage[]) {
	for(int i = 0; i < 5; i++) {
		if ((voltage[i] > 3) || voltage[i] < 3) {
			HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_RESET);
		}
		else {
			HAL_GPIO_WritePin(GPIOA, Charge_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, Discharge_Pin, GPIO_PIN_SET);
		}
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USART2_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, RxData, 256);

  for(int i = 0; i < 3; i++)
     	  {
   	  	  HAL_GPIO_WritePin(SelectPinsPort, SelectPins[i], GPIO_PIN_SET);
     	  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  for(int i = 0; i < 6; i++)
	  	  		  	  {
	  	  		  		  selectMuxPin(i);
	  	  		  		  HAL_ADC_Start(&hadc);
	  	  		  		  HAL_ADC_PollForConversion(&hadc, 100);
	  	  		  		  raw_adc[i] = HAL_ADC_GetValue(&hadc);

	  	  		  		  /*********************************************/
	  	  		  		  //raw_adc[0] = channel0
	  	  		  		  //raw_adc[1] = channel1
	  	  		  		  //raw_adc[2] = channel2
	  	  		  		  //raw_adc[3] = channel3, not connected atm
	  	  		  		  //raw_adc[4] = channel4 = temp sensor
	  	  		  		  //raw_adc[5] = channel5 = current sensor
	  	  		  		  /* ***************************************************************************************************/

	  	  		  		  temp = (raw_adc[4]*3.3)/4095;
	  	  		  		  temperature = temp*100;
	  	  		  		  current = ((raw_adc[5] / 9300) / 0.01) * 1000;


	  	  		  		  // should add this modified temperature value to the "database" aswell

	  	  		  		  Voltages[i] = (raw_adc[i] / 4095) * 3.3;
	  	  		  		  last_cell = Voltages[2] - Voltages[3];
	  	  		  		  Holding_Registers_Database[i] = Voltages[i];
	  	  		  		  //Holding_Registers_Database[4] = temperature;

	  	  		  		//snprintf(buffer,sizeof(buffer), "Voltage at channel(1): %.2f\r\n",temperature);
	  	  		  		//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	  	  		  		//HAL_Delay(1000);

	  	  		  		  void current_check(current);
	  	  		  		  void temperature_check(temperature);
	  	  		  		  void voltage_check( Voltages[i]);

	  	  		  		  HAL_ADC_Stop(&hadc);
	  	  		  		  HAL_Delay(500);
	  	  		  	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|Charge_Pin|Discharge_Pin, S1_Pin|S2_Pin|S3_Pin|TX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : S1_Pin S2_Pin S3_Pin TX_EN_Pin */
  GPIO_InitStruct.Pin = S1_Pin|S2_Pin|S3_Pin|TX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin|Charge_Pin|Discharge_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
