/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

/* USER CODE BEGIN PV */
const unsigned char stringtosend[] = "DMA\r\n";
unsigned char stringtoreceive[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Configure_GPIO_LEDs(void);
void Configure_GPIO_USART2(void);
void Configure_DMA1(void);
void Configure_USART2(void);
void Configure_GPIO_Button(void);
void Configure_EXTI(void);
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
  /* USER CODE BEGIN 2 */

  Configure_GPIO_LEDs();
  Configure_GPIO_USART2();
  Configure_DMA1();
  Configure_USART2();
  Configure_GPIO_Button();
  Configure_EXTI();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Configure_GPIO_LEDs(void){
	// Setup of LED pins as output for PC0, PC1, PC2
	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC -> MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2);
	GPIOC -> MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0;
}

void Configure_GPIO_USART2(void){
	// USART2 configuration for pins PA2-TX PA3-RX
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA -> MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOA -> MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;

	GPIOA -> AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
	GPIOA -> AFR[0] |= 0b0111 << (2 * 4) | 0b0111 << (3 * 4);
}

void Configure_DMA1(void){
	// Configure DMA, peripheral adress, memory adress, memory increment for TX

	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;

	DMA1_Channel7 -> CPAR = (unsigned long)&(USART2 -> TDR);
	DMA1_Channel7 -> CMAR = (unsigned long)stringtosend;
	DMA1_Channel7 -> CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;

	// Configure DMA, peripheral adress. memory adress, data size, increment for RX

	DMA1_Channel6 -> CPAR = (unsigned int)&(USART2 -> RDR);
	DMA1_Channel6 -> CMAR = (unsigned int)stringtoreceive;
	DMA1_Channel6 -> CNDTR = sizeof(stringtoreceive);
	DMA1_Channel6 -> CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN;

	// Configure DMA interrupt for channel

	NVIC_SetPriority(DMA1_Channel7_IRQn, 0);
	NVIC_SetPriority(DMA1_Channel6_IRQn, 0);

	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

void Configure_USART2(void){
	/*
	* Configure USART2 peripheral clock, set Baud Rate 38400
	* Enable DMA reception and transmission
	* 8 data bit, 1 start bit, 1 stop bit, no parity, reception and transmission enabled
	* */
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;

	USART2 -> BRR = 8000000 / 38400;
	USART2 -> CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;
	USART2 -> CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	while((USART2 -> ISR & USART_ISR_TC) != USART_ISR_TC){
	  // Add time out here for a robust application
	}

	USART2 -> ICR |= USART_ICR_TCCF;
	USART2 -> CR1 |= USART_CR1_TCIE;
}

void Configure_GPIO_Button(void){
	// Configure BUTTON PA10 as input
	GPIOA -> MODER &= ~(GPIO_MODER_MODER10);
}
void Configure_EXTI(void){
	//EXTI with FALLING EDGE trigger

	SYSCFG -> EXTICR[10] &= ~(SYSCFG_EXTICR3_EXTI10);
	SYSCFG -> EXTICR[10] |= SYSCFG_EXTICR3_EXTI10_PA;

	EXTI -> IMR |= EXTI_IMR_MR10;
	EXTI -> FTSR |= EXTI_FTSR_FT10;

	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void){

	DMA1_Channel7 -> CCR &= ~(DMA_CCR_EN);
	DMA1_Channel7 -> CNDTR = sizeof(stringtosend);
	DMA1_Channel7 -> CCR |= DMA_CCR_EN;

	EXTI -> PR |= 1;

}

void DMA1_Channel7_IRQHandler(void){
// IF TRANSFER COMPLEET FLAG IS 1 DO THIS ...
	if((DMA1 -> ISR & DMA_ISR_TCIF7) == DMA_ISR_TCIF7){

		DMA1 -> IFCR |= DMA_IFCR_CTCIF7;//CLEAR FLAG

		GPIOC -> ODR ^= GPIO_ODR_0;// TOGGLE LED
	}

	else{
		NVIC_DisableIRQ(DMA1_Channel7_IRQn);
	}
}

void DMA1_Channel6_IRQHandler(void){

	if((DMA1 -> ISR & DMA_ISR_TCIF6) == DMA_ISR_TCIF6){

		DMA1 -> IFCR |= DMA_IFCR_CTCIF6;

		if(strcmp("OK",(const char *)stringtoreceive) == 0){
			GPIOC -> ODR ^= GPIO_ODR_1;
			GPIOC -> ODR ^= GPIO_ODR_2;
		}

		else if(strcmp("Ok",(const char *)stringtoreceive) == 0){
			GPIOC -> ODR ^= GPIO_ODR_2;
		}

		else if(strcmp("ok",(const char *)stringtoreceive) == 0){
			GPIOC -> ODR ^= GPIO_ODR_1;
		}

		DMA1_Channel6 -> CCR &= ~(DMA_CCR_EN);
		DMA1_Channel6 -> CNDTR = sizeof(stringtoreceive);
		DMA1_Channel6 -> CCR |= DMA_CCR_EN;
	}

	else{
		NVIC_DisableIRQ(DMA1_Channel6_IRQn);
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
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
