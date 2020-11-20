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
#include "GPIO.h"
#include "EXTIFallingEdge.h"
#include "DelayFunction.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MCU_CLOCK 48000000

#define LED1_PIN 9
#define LED1_PORT GPIOC

#define LED2_PIN 8
#define LED2_PORT GPIOA

#define UART2_TX_PIN 2
#define UART2_TX_PORT GPIOA

#define UART2_RX_PIN 3
#define UART2_RX_PORT GPIOA

#define UART1_TX_PIN 9
#define UART1_TX_PORT GPIOA

#define UART1_RX_PIN 10
#define UART1_RX_PORT GPIOA

#define BUFFER_LENGHT 40
#define STRING_LENGHT 40
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char uart1_receive_buffer[BUFFER_LENGHT];
unsigned char uart2_receive_buffer[BUFFER_LENGHT];
unsigned char uart1_send_string[BUFFER_LENGHT];
unsigned char uart2_send_string[BUFFER_LENGHT];
volatile unsigned short inputuart1 = 0;
volatile unsigned short inputuart2 = 0;
volatile unsigned short senduart1 = 0;
volatile unsigned short senduart2 = 0;
volatile unsigned short messageuart1 = 0;
volatile unsigned short messageuart2 = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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

  	  SysTick_Config(MCU_CLOCK/1000);

	GPIOOutputSetup(LED1_PORT, LED1_PIN);
	GPIOOutputSetup(LED2_PORT, LED2_PIN);
	GPIOAlternateFunctionSetup(UART2_TX_PORT, UART2_TX_PIN);
	GPIOAlternateFunctionSetup(UART2_RX_PORT, UART2_RX_PIN);
	GPIOAlternateFunctionSetup(UART1_TX_PORT, UART1_TX_PIN);
	GPIOAlternateFunctionSetup(UART1_RX_PORT, UART1_RX_PIN);

	//UART2 SETUP START
	UART2_TX_PORT->AFR[0] &= ~(GPIO_AFRL_AFRL2);
	UART2_TX_PORT->AFR[0] |= 0b0111 << 8;

	UART2_RX_PORT->AFR[0] &= ~(GPIO_AFRL_AFRL3);
	UART2_RX_PORT->AFR[0] |= 0b0111 << 12;

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	USART2->BRR = 24000000 / 38400;
	USART2->CR1 |= USART_CR1_TE | USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE;

	while((USART2->ISR & USART_ISR_TC) != USART_ISR_TC){

	}

	USART2->ICR |= USART_ICR_TCCF;
	USART2->CR1 |= USART_CR1_TCIE;

	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
	//UART2 SETUP END

	//UART1 SETUP START

	UART1_TX_PORT->AFR[1] &= ~(GPIO_AFRL_AFRL1);
	UART1_TX_PORT->AFR[1] |= 0b0111 << 4;

	UART1_RX_PORT->AFR[1] &= ~(GPIO_AFRL_AFRL2);
	UART1_RX_PORT->AFR[1] |= 0b0111 << 8;

	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;

	USART1 -> BRR = 48000000 / 38400;
	USART1 -> CR1 |= USART_CR1_TE | USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE;

	while((USART1 -> ISR & USART_ISR_TC) != USART_ISR_TC){
	  // Add time out here for a robust application
	}

	USART1 -> ICR |= USART_ICR_TCCF;
	USART1 -> CR1 |= USART_CR1_TCIE;

	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

	  //UART1 SETUP END



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(messageuart2 == 1){
		GPIOPinToggle(LED2_PORT, LED2_PIN);//PA8
		inputuart2 = 0;
		strcpy((char *)uart1_send_string, (char *)uart2_receive_buffer);
		memset(uart2_receive_buffer, '\0', sizeof(uart2_receive_buffer));
		USART1 -> TDR = uart1_send_string[senduart1++];
		messageuart2 = 0;
		}

	  if(messageuart1 == 1){
		GPIOPinToggle(LED1_PORT, LED1_PIN);//PC9
		inputuart1 = 0;
		strcpy((char *)uart2_send_string, (char *)uart1_receive_buffer);
		memset(uart1_receive_buffer, '\0', sizeof(uart1_receive_buffer));
		USART2 -> TDR = uart2_send_string[senduart2++];
		messageuart1 = 0;
	}

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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void USART1_IRQHandler(void ){
	if((USART1 -> ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		USART1->CR1 |= USART_CR1_IDLEIE;
		uart1_receive_buffer[inputuart1] = (unsigned char)(USART1 -> RDR);
		inputuart1++;
	}

	else if((USART1 -> ISR & USART_ISR_IDLE) == USART_ISR_IDLE){
		USART1->CR1 &= ~USART_CR1_IDLEIE;
		messageuart1 = 1;
		USART1 -> ICR |= USART_ICR_IDLECF;

	}
	else if((USART1 -> ISR & USART_ISR_TC) == USART_ISR_TC){
		if(senduart1 == sizeof(uart1_send_string) || (uart1_send_string[senduart1] == '\0')){
			senduart1 = 0;
			USART1 -> ICR |= USART_ICR_TCCF;
		}
		else{
			USART1 -> TDR = uart1_send_string[senduart1++];
		}
	}
	else{
			NVIC_DisableIRQ(USART1_IRQn);
	}
}

void USART2_IRQHandler(){

	if((USART2 -> ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		USART2->CR1 |= USART_CR1_IDLEIE;
		uart2_receive_buffer[inputuart2] = (unsigned char)(USART2 -> RDR);
		inputuart2++;
		}

	else if((USART2 -> ISR & USART_ISR_IDLE) == USART_ISR_IDLE){
		USART2->CR1 &= ~USART_CR1_IDLEIE;
		messageuart2 = 1;
		USART2 -> ICR |= USART_ICR_IDLECF;
	}

	else if((USART2 -> ISR & USART_ISR_TC) == USART_ISR_TC){
		if(senduart2 == sizeof(uart2_send_string) || (uart2_send_string[senduart2] == '\0')){
			senduart2 = 0;
			USART2 -> ICR |= USART_ICR_TCCF;

		}
		else{
			USART2 -> TDR = uart2_send_string[senduart2++];
		}
	}

	else{
		NVIC_DisableIRQ(USART2_IRQn);
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
