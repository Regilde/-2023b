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
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "fun.h"
#include "capture.h"
#include "xfft.h"
#include "stm32h7xx_hal_gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ALIGN_32BYTES (uint16_t adc1_data[FFT_LENGTH]) 	__attribute__((section(".ARM.__at_0x30000000")));
ALIGN_32BYTES (uint16_t adc2_data[FFT_LENGTH]) 	__attribute__((section(".ARM.__at_0x30020000")));
ALIGN_32BYTES (uint16_t adc3_data[FFT_LENGTH]) 	__attribute__((section(".ARM.__at_0x30040000")));
__IO uint8_t AdcConvEnd = 0;//��ʾadc�ɼ����
ALIGN_32BYTES (uint16_t Dat[200]) 	__attribute__((section(".ARM.__at_0x38000000")));
uint8_t rec_data;

void adc1_init(void)
{
	//printf("start_adc1\n");
	MX_ADC1_Init();	//��ʼ�����÷�����, ȷ����MX_DMA_Init()��ʼ������  	  
	HAL_Delay(10);	//�еط�˵������Եȵȵ�ѹ�ȶ�����У׼

	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, FFT_LENGTH) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc1);
	HAL_TIM_Base_Stop(&htim15);
	/*for(int i=0;i<FFT_LENGTH;i++)
		{
			printf("%f\r\n", (adc1_data[i]*3.3/65535));
		}*/
}

void adc2_init(void)
{
	//printf("start_adc2\n");
	MX_ADC2_Init();	//��ʼ�����÷�����, ȷ����MX_DMA_Init()��ʼ������  	  
	HAL_Delay(10);	//�еط�˵������Եȵȵ�ѹ�ȶ�����У׼

	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc2 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc2_data, FFT_LENGTH) != HAL_OK)
	{
			printf("hadc2 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc2);
	HAL_TIM_Base_Stop(&htim15);
}

void adc3_init(void)
{
	//printf("start_adc3\n");
	MX_ADC3_Init();	//��ʼ�����÷�����, ȷ����MX_DMA_Init()��ʼ������    
	HAL_Delay(10);	//�еط�˵������Եȵȵ�ѹ�ȶ�����У׼

	if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc3_data, FFT_LENGTH) != HAL_OK)
	{
			printf("hadc3 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc3);
	HAL_TIM_Base_Stop(&htim15);
	/*for(int i=0;i<FFT_LENGTH;i++)
		{
			printf("%f\r\n", (adc3_data[i]*3.3/65535));
		}*/
}

void double_adc(void)
{ 
	MX_ADC1_Init();
	MX_ADC3_Init();	//��ʼ�����÷�����, ȷ����MX_DMA_Init()��ʼ������    
	HAL_Delay(100);	//�еط�˵������Եȵȵ�ѹ�ȶ�����У׼
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, FFT_LENGTH) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}
	if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	{
			printf("hadc1 error with HAL_ADCEx_Calibration_Start\r\n");
			Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc3_data, FFT_LENGTH) != HAL_OK)
	{
			printf("hadc3 error with HAL_ADC_Start_DMA\r\n");
			Error_Handler();
	}

	HAL_TIM_Base_Start(&htim15);
	while (!AdcConvEnd);
	AdcConvEnd = 0;
	HAL_ADC_DeInit(&hadc1);
	HAL_ADC_DeInit(&hadc3);
	HAL_TIM_Base_Stop(&htim15);
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void SineWave()
{
    uint16_t i;
    for( i=0;i<200;i++)
    {
  		Dat[i]=(uint16_t)((int16_t)(2048*sin(i*2*3.1415926/200))+2048);
//			Dat[i] = 4095;
    }
//	Dat[0] = 0;
//	Dat[1] = 4095;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t 	cnt_value[5] 	= {0};
uint32_t 	flag_f;
uint32_t 	frequency = 0;
uint32_t cnt_flag = 0;
int mode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
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

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM7_Init();
  MX_DAC1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_TIM15_Init();
  MX_ADC3_Init();
  MX_ADC2_Init();
  MX_UART7_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	printf("strat\r\n");
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start(&htim2);

	HAL_UART_Receive_IT(&huart1,&rec_data,1);
	uint8_t k=1;

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	/*HAL_GPIO_WritePin (GPIOD,GPIO_PIN_8,GPIO_PIN_SET);
*/
	
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
	//���Դ���
	adc3_init();
	FFT_Init(adc3_data);//Ri_v1
	//FFT_DIS();
	for (int j=0;j<5;j++)
		{
			printf("v%d=%.2f\r\n",j,v1[j]);
			printf("f%d=%.2f\r\n",j,f2[j]);
		}
	//ChangeFreq();
	
	//Debug();
  while (1)
  {		
		if (mode == 0)
		{
			if (rec_data == '1') mode = 1;
			else if (rec_data =='2') mode = 2;
		}
		if(mode =='1')
		{
			length_cmp();
			mode = 0;
		}
		else if(mode == '2')
		{
			load_type();
			mode = 0;
		}
//	for(int i=0;i<FFT_LENGTH;i++)
//	{
//		printf("%d\r\n",adc3_data[i]);
//	}



		
		
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 360;
  PeriphClkInitStruct.PLL2.PLL2P = 5;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if(hadc->Instance == ADC1) 
	{
      //SCB_InvalidateDCache_by_Addr((uint32_t *) &adc1_data[0], ADC1_BUFFER_SIZE);
   }
	 AdcConvEnd = 1;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim4.Instance)
	{
			cnt_value[flag_f]= __HAL_TIM_GetCounter(&htim2);//��ȡ�ⲿ��������ֵ
			__HAL_TIM_SetCounter(&htim2,0);
			flag_f++;
			if(flag_f == 5)
			{
				flag_f = 0;
				frequency = (cnt_value[2]+cnt_value[3]+cnt_value[4])/3*1000;
				HAL_TIM_Base_Stop_IT(&htim4);
				cnt_flag = 1;
			}
	}
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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