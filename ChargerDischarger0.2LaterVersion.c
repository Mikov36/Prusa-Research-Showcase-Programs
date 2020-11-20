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
#include "LCD_STM32F031_4PIN_MODE.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAIN_MENU_SIZE 3
#define SUBMENU1_SIZE  4
#define SUBMENU2_SIZE  5
#define SUBMENU3_SIZE  4
#define MAX_MENU 3
#define BUTTON_PRESSED 1
#define BUTTON_NOT_PRESSED 0
#define DEFAULT_VOLTAGE 3600
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
char *p_mainmenu[MAIN_MENU_SIZE] = {"VOLTAGE", "DISCHARGE", "CAPACITY"};
char *p_submenu1[SUBMENU1_SIZE] = {"BACK", "CELL1", "CELL2", "CELL3"};
char *p_submenu2[SUBMENU2_SIZE] = {"BACK", "VOLTAGE SET", "CELL1", "CELL2", "CELL3"};
char *p_submenu3[SUBMENU3_SIZE] = {"BACK", "SET1", "SET2", "SET3"};
char ON[] = "ON ";
char OFF[] = "OFF";
char mV[] = "mV";
char mezera[] = " ";
int menu;
volatile short button = 0;
volatile short rotation = 0;
volatile int count = 0;
volatile short last = 0;
int progtime;
int line = 0;
int nahoru = 0;
int startdischarge1, startdischarge2, startdischarge3;
int relay1, relay2, relay3 = 0;
int voltageset1 = 3500;
int voltageset2 = 3400;
int voltageset3 = 3300;
int voltagesetvolba;
unsigned long adc_dma_values[3];
int ticker = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void RotationDirectionDetermination(){
/*
 * This function determines the rotation of the encoder, it increments or decrements according to the
 * conditionals in the interrupt c. file
 */
	if((rotation == 1) && (last == 1)){
		if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)) {
			HAL_Delay(3);
			if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)){
				count++;
				rotation = 0;
				last = 0;
			}
		}
	 }

	if((rotation == 2) && (last == 1)){
		if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)) {
			HAL_Delay(3);
			if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)){
				count--;
				rotation = 0;
				last = 0;
			}
		}
	 }
}

void CounterCheck(){
/* This function checks and corrects the count pulses depends on your MENU,
 * if you need more lines or more sub-menus, you can just add them by adding
 * more conditionals
 * */
	switch(menu){
	case 0:
		if(count > MAIN_MENU_SIZE - 1){
			count = MAIN_MENU_SIZE - 1;
		}
		if(count < 0){
			count = 0;
		}
		break;
	case 1:
		if(count > SUBMENU1_SIZE - 1){
			count = SUBMENU1_SIZE - 1;
		}
		if(count < 0){
			count = 0;
		}
		break;
	case 2:
		if(count > SUBMENU2_SIZE - 1){
			count = SUBMENU2_SIZE - 1;
		}
		if(count < 0){
			count = 0;
		}
		break;
	case 3:
		if(count > SUBMENU3_SIZE - 1){
			count = SUBMENU3_SIZE - 1;
		}
		if(count < 0){
			count = 0;
		}
		break;
	default:
		break;
	}
}

void DisplayDataMainMenu(int row){
	 LCD1602_setCursor(1, 2);
	 LCD1602_print(p_mainmenu[row]);
	 LCD1602_setCursor(2, 2);
	 LCD1602_print(p_mainmenu[row+1]);
}

void DisplayDataSubMenu1(int row){
	 LCD1602_setCursor(1, 2);
	 LCD1602_print(p_submenu1[row]);
	 LCD1602_setCursor(2, 2);
	 LCD1602_print(p_submenu1[row+1]);
}

void DisplayDataSubMenu2(int row){
	 LCD1602_setCursor(1, 2);
	 LCD1602_print(p_submenu2[row]);
	 LCD1602_setCursor(2, 2);
	 LCD1602_print(p_submenu2[row+1]);
}

void DisplayDataSubMenu3(int row){
	 LCD1602_setCursor(1, 2);
	 LCD1602_print(p_submenu3[row]);
	 LCD1602_setCursor(2, 2);
	 LCD1602_print(p_submenu3[row+1]);
}

void NewMenu(int row){
	 LCD1602_clear();
	 DisplayDataMainMenu(row);
}

void MainMenu_NewScreen(int row){
	LCD1602_clear();
	DisplayDataMainMenu(row);
}

void MainMenu_SameScreen(int row){
	 DisplayDataMainMenu(row);
}

void SubMenu1(int row){
	 LCD1602_clear();
	 DisplayDataSubMenu1(row);
}

void SubMenu1_NewScreen(int row){
	LCD1602_clear();
	DisplayDataSubMenu1(row);
}

void SubMenu1_SameScreen(int row){
	DisplayDataSubMenu1(row);
}

void SubMenu2(int row){
	 LCD1602_clear();
	 DisplayDataSubMenu2(row);
}

void SubMenu2_NewScreen(int row){
	LCD1602_clear();
	DisplayDataSubMenu2(row);
}

void SubMenu2_SameScreen(int row){
	DisplayDataSubMenu2(row);
}

void SubMenu3(int row){
	 LCD1602_clear();
	 DisplayDataSubMenu3(row);
}

void SubMenu3_NewScreen(int row){
	LCD1602_clear();
	DisplayDataSubMenu3(row);
}

void SubMenu3_SameScreen(int row){
	DisplayDataSubMenu3(row);
}

void CursorDown(){
	LCD1602_setCursor(2, 1);
	SendACharacter(0b00111110);
	LCD1602_setCursor(1, 1);
	SendACharacter(0b00100000);
}

void CursorUp(){
	LCD1602_setCursor(1, 1);
	SendACharacter(0b00111110);
	LCD1602_setCursor(2, 1);
	SendACharacter(0b00100000);
}

void DisplayADCValueOneRow(int row, unsigned long value){

	LCD1602_setCursor(row, 10);
	LCDSendAndInteger(value, 4);
	LCD1602_print(mezera);

}

void DisplayADCValueTwoRows(unsigned long value1, unsigned long value2){

	LCD1602_setCursor(1, 10);
	LCDSendAndInteger(value1, 4);
	LCD1602_print(mezera);

	LCD1602_setCursor(2, 10);
	LCDSendAndInteger(value2, 4);
	LCD1602_print(mezera);

}

void DisplayMiliVoltsOneRow(){
	LCD1602_setCursor(2, 15);
	LCD1602_print(mV);
}

void DisplayMiliVoltsTwoRows(){
	LCD1602_setCursor(1, 15);
	LCD1602_print(mV);
	LCD1602_setCursor(2, 15);
	LCD1602_print(mV);
}

int VoltageSetCalculation(int voltage){
	voltage = voltage + (count * 10);
	return voltage;
}

void VoltageSetFunction(int volba){

	switch(volba){
		case 1:
			if(nahoru == 1){
				if(count != 0){
					LCD1602_setCursor(1, 10);
					LCDSendAndInteger(voltageset1 = VoltageSetCalculation(voltageset1), 4);
					count = 0;
				}
				if(button == BUTTON_PRESSED){
					LCD1602_setCursor(1, 6);
					LCD1602_print(" ");
					voltagesetvolba = 0;
					count = 1;
					menu = 3;
				}
			}
			if(nahoru == 0){
				if(count != 0){
					LCD1602_setCursor(2, 10);
					LCDSendAndInteger(voltageset1 = VoltageSetCalculation(voltageset1), 4);
					count = 0;
				}
				if(button == BUTTON_PRESSED){
					LCD1602_setCursor(2, 6);
					LCD1602_print(" ");
					voltagesetvolba = 0;
					count = 1;
					menu = 3;
				}
			}
			button = BUTTON_NOT_PRESSED;
			break;
		case 2:
			if(nahoru == 1){
				if(count != 0){
					LCD1602_setCursor(1, 10);
					LCDSendAndInteger(voltageset2 = VoltageSetCalculation(voltageset2), 4);
					count = 0;
				}
				if(button == BUTTON_PRESSED){
					LCD1602_setCursor(1, 6);
					LCD1602_print(" ");
					voltagesetvolba = 0;
					count = 2;
					menu = 3;
				}
			}
			if(nahoru == 0){
				if(count != 0){
					LCD1602_setCursor(2, 10);
					LCDSendAndInteger(voltageset2 = VoltageSetCalculation(voltageset2), 4);
					count = 0;
				}
				if(button == BUTTON_PRESSED){
					LCD1602_setCursor(2, 6);
					LCD1602_print(" ");
					voltagesetvolba = 0;
					count = 2;
					menu = 3;
				}
			}
			button = BUTTON_NOT_PRESSED;
			break;
		case 3:
			if(nahoru == 0){
				if(count != 0){
					LCD1602_setCursor(2, 10);
					LCDSendAndInteger(voltageset3 = VoltageSetCalculation(voltageset3), 4);
					count = 0;
				}
				if(button == BUTTON_PRESSED){
					LCD1602_setCursor(2, 6);
					LCD1602_print(" ");
					voltagesetvolba = 0;
					count = 3;
					menu = 3;
				}
			}
			button = BUTTON_NOT_PRESSED;
			break;
	}
}

void DisplayMainMenu(void){

	switch(count){
	  case(0):
			if((line == 0) && (nahoru == 0)){
				NewMenu(count);
				CursorUp();
				line = 1;
			}
			if((line == 2) && (nahoru == 0)){
				MainMenu_SameScreen(count);
				CursorUp();
				line = 1;
			}
			if((line == 2) && (nahoru == 1)){
				MainMenu_NewScreen(count);
				CursorUp();
				nahoru = 0;
				line = 1;
			}

			if(button == BUTTON_PRESSED){
				menu = 1;
				line = 0;
				count = 0;
				nahoru = 0;
				button = BUTTON_NOT_PRESSED;
			}
		  break;
	  case(1):
			 if((line == 1) && (nahoru == 0)){
				MainMenu_SameScreen(count - 1);
				CursorDown();
				line = 2;
			 }
			 if((line == 3) && (nahoru == 0)){
				MainMenu_SameScreen(count);
				CursorUp();
				nahoru = 1;
				line = 2;
			 }
			 if(button == BUTTON_PRESSED){
				 menu = 2;
				 line = 0;
				 count = 0;
				 nahoru = 0;
				 button = BUTTON_NOT_PRESSED;
			 }
		  break;
	  case(2):
			if((line == 2) && (nahoru == 0)){
				MainMenu_NewScreen(count - 1);
				CursorDown();
				line = 3;
			}
			if((line == 2) && (nahoru == 1)){
				MainMenu_SameScreen(count - 1);
				CursorDown();
				nahoru = 0;
				line = 3;
			}
			button = BUTTON_NOT_PRESSED;
		  break;
	  }
}

void DisplaySubMenu1(void){
	switch(count){
	  case(0):
			if((line == 0) && (nahoru == 0)){
				SubMenu1(count);
				CursorUp();
				DisplayADCValueOneRow(2, adc_dma_values[0]);
				DisplayMiliVoltsOneRow();
				line = 1;
			}
			if((line == 2) && (nahoru == 0)){
				SubMenu1_SameScreen(count);
				CursorUp();
				DisplayADCValueOneRow(2, adc_dma_values[0]);
				DisplayMiliVoltsOneRow();
				line = 1;
			}
			if((line == 2) && (nahoru == 1)){
				SubMenu1_NewScreen(count);
				CursorUp();
				DisplayADCValueOneRow(2, adc_dma_values[0]);
				DisplayMiliVoltsOneRow();
				nahoru = 0;
				line = 1;
			}
			if(ticker == 1){
				DisplayADCValueOneRow(2, adc_dma_values[0]);
				ticker = 0;
			}
			if(button == BUTTON_PRESSED){
				menu = 0;
				line = 0;
				count = 0;
				nahoru = 0;
				button = BUTTON_NOT_PRESSED;
			}
		  break;
	  case(1):
			 if((line == 1) && (nahoru == 0)){
				SubMenu1_SameScreen(count - 1);
				CursorDown();
				DisplayADCValueOneRow(2, adc_dma_values[0]);
				DisplayMiliVoltsOneRow();

				line = 2;
			 }
			 if((line == 3) && (nahoru == 0)){
				SubMenu1_SameScreen(count);
				CursorUp();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);

				nahoru = 1;
				line = 2;
			 }
			 if((line == 3) && (nahoru == 1)){
				SubMenu1_NewScreen(count);
				CursorUp();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);

				nahoru = 1;
				line = 2;
			 }
			 if(ticker == 1){
				 if(nahoru == 0){
					 DisplayADCValueOneRow(2, adc_dma_values[0]);
				 }

				 if(nahoru == 1){
					 DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);
				 }
				 ticker = 0;
			 }
			 button = BUTTON_NOT_PRESSED;
		  break;
	  case(2):
			if((line == 2) && (nahoru == 0)){
				SubMenu1_NewScreen(count - 1);
				CursorDown();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);

				line = 3;
			}
		    if((line == 2) && (nahoru == 1)){
				SubMenu1_SameScreen(count - 1);
				CursorDown();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);

				nahoru = 0;
				line = 3;
			}
			if((line == 4) && (nahoru == 0)){
				SubMenu1_SameScreen(count);
				CursorUp();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[1], adc_dma_values[2]);

				nahoru = 1;
				line = 3;
			}
			if(ticker == 1){
				 if(nahoru == 0){
					 DisplayADCValueTwoRows(adc_dma_values[0], adc_dma_values[1]);
				 }

				 if(nahoru == 1){
					 DisplayADCValueTwoRows(adc_dma_values[1], adc_dma_values[2]);
				 }
				 ticker = 0;
			}
			button = BUTTON_NOT_PRESSED;
		  break;
	  case(3):
			if((line == 3) && (nahoru == 0)){
				SubMenu1_NewScreen(count - 1);
				CursorDown();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[1], adc_dma_values[2]);
				line = 4;
			}
			if((line == 3) && (nahoru == 1)){
				SubMenu1_SameScreen(count - 1);
				CursorDown();
				DisplayMiliVoltsTwoRows();
				DisplayADCValueTwoRows(adc_dma_values[1], adc_dma_values[2]);
				nahoru = 0;
				line = 4;
			}
			if(ticker == 1){
				 if(nahoru == 0){
					 DisplayADCValueTwoRows(adc_dma_values[1], adc_dma_values[2]);
				 }
				 ticker = 0;
			}

			button = BUTTON_NOT_PRESSED;
		  break;
	  }
}

void DisplaySubMenu2(void){

	switch(count){
	  case(0):
		if((line == 0) && (nahoru == 0)){
			SubMenu2(count);
			CursorUp();
			line = 1;
		}
		if((line == 2) && (nahoru == 0)){
			SubMenu2_SameScreen(count);
			CursorUp();
			line = 1;
		}
		if((line == 2) && (nahoru == 1)){
			SubMenu2_NewScreen(count);
			CursorUp();
			nahoru = 0;
			line = 1;
		}

		if(button == BUTTON_PRESSED){
			menu = 0;
			line = 0;
			count = 0;
			nahoru = 0;
			button = BUTTON_NOT_PRESSED;
		}
		break;
	  case(1):
		 if((line == 1) && (nahoru == 0)){
			SubMenu2_SameScreen(count - 1);
			CursorDown();
			line = 2;
		 }
		 if((line == 3) && (nahoru == 0)){
			SubMenu2_SameScreen(count);
			CursorUp();
			if(relay1 == 0){
			LCD1602_setCursor(2, 13);
			LCD1602_print(OFF);
			}
			else{
			LCD1602_setCursor(2, 13);
			LCD1602_print(ON);
			}
			nahoru = 1;
			line = 2;
		 }
		 if((line == 3) && (nahoru == 1)){
			SubMenu2_NewScreen(count);
			CursorUp();
			if(relay1 == 0){
			LCD1602_setCursor(2, 13);
			LCD1602_print(OFF);
			}
			else{
			LCD1602_setCursor(2, 13);
			LCD1602_print(ON);
			}
			nahoru = 1;
			line = 2;
		 }
		 if(button == BUTTON_PRESSED){
			menu = 3;
			line = 0;
			count = 0;
			nahoru = 0;
			button = BUTTON_NOT_PRESSED;
		}
		 break;
	  case(2):
		if((line == 2) && (nahoru == 0)){
			SubMenu2_NewScreen(count - 1);
			CursorDown();
			if(relay1 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			else{
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			line = 3;
		}
		if((line == 2) && (nahoru == 1)){
			SubMenu2_SameScreen(count - 1);
			CursorDown();
			if(relay1 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			else{
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 0;
			line = 3;
		}
		if((line == 4) && (nahoru == 0)){
			SubMenu2_SameScreen(count);
			CursorUp();
			if(relay1 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay1 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay2 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 1;
			line = 3;
		}
		if((line == 4) && (nahoru == 1)){
			SubMenu2_NewScreen(count);
			CursorUp();
			if(relay1 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay1 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay2 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 1;
			line = 3;
		}

		if(button == BUTTON_PRESSED){
				if(relay1 == 0){
					HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
					if(nahoru == 1){
						LCD1602_setCursor(1, 13);
						LCD1602_print(ON);
					}
					if(nahoru == 0){
						LCD1602_setCursor(2, 13);
						LCD1602_print(ON);
					}
				}
				relay1++;
				if(relay1 > 1){
					HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
					relay1 = 0;
					if(nahoru == 1){
						LCD1602_setCursor(1, 13);
						LCD1602_print(OFF);
					}
					if(nahoru == 0){
						LCD1602_setCursor(2, 13);
						LCD1602_print(OFF);
					}
				}
				button = BUTTON_NOT_PRESSED;
		}
		break;
	  case(3):
		if((line == 3) && (nahoru == 0)){
			SubMenu2_NewScreen(count - 1);
			CursorDown();
			if(relay1 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay1 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay2 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			line = 4;
		}
		if((line == 3) && (nahoru == 1)){
			SubMenu2_SameScreen(count - 1);
			CursorDown();
			if(relay1 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay1 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay2 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 0;
			line = 4;
		}
		if((line == 5) && (nahoru == 0)){
			SubMenu2_SameScreen(count);
			CursorUp();
			if(relay2 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay3 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay3 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 1;
			line = 4;
		}
		if(button == BUTTON_PRESSED){
			if(relay2 == 0){
				HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
				if(nahoru == 1){
					LCD1602_setCursor(1, 13);
					LCD1602_print(ON);
				}
				if(nahoru == 0){
					LCD1602_setCursor(2, 13);
					LCD1602_print(ON);
				}
			}
			relay2++;
			if(relay2 > 1){
				HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);
				relay2 = 0;
				if(nahoru == 1){
					LCD1602_setCursor(1, 13);
					LCD1602_print(OFF);
				}
				if(nahoru == 0){
					LCD1602_setCursor(2, 13);
					LCD1602_print(OFF);
				}
			}
			button = BUTTON_NOT_PRESSED;
		}
			  break;
	  case(4):
		if((line == 4) && (nahoru == 0)){
			SubMenu2_NewScreen(count - 1);
			CursorDown();
			if(relay2 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay3 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay3 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			line = 5;
		}
		if((line == 4) && (nahoru == 1)){
			SubMenu2_SameScreen(count - 1);
			CursorDown();
			if(relay2 == 0){
				LCD1602_setCursor(1, 13);
				LCD1602_print(OFF);
			}
			if(relay2 == 1){
				LCD1602_setCursor(1, 13);
				LCD1602_print(ON);
			}
			if(relay3 == 0){
				LCD1602_setCursor(2, 13);
				LCD1602_print(OFF);
			}
			if(relay3 == 1){
				LCD1602_setCursor(2, 13);
				LCD1602_print(ON);
			}
			nahoru = 0;
			line = 5;
		}
		if(button == BUTTON_PRESSED){
			if(relay3 == 0){
				HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);
				if(nahoru == 1){
					LCD1602_setCursor(1, 13);
					LCD1602_print(ON);
				}
				if(nahoru == 0){
					LCD1602_setCursor(2, 13);
					LCD1602_print(ON);
				}
			}
			relay3++;
			if(relay3 > 1){
				HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);
				relay3 = 0;
				if(nahoru == 1){
					LCD1602_setCursor(1, 13);
					LCD1602_print(OFF);
				}
				if(nahoru == 0){
					LCD1602_setCursor(2, 13);
					LCD1602_print(OFF);
				}
			}
			button = BUTTON_NOT_PRESSED;
		}
			  break;
  }
}

void DisplaySubMenu3(void){
	switch(count){
	  case(0):
			if((line == 0) && (nahoru == 0)){
				SubMenu3(count);
				CursorUp();
				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);
				line = 1;
			}
			if((line == 2) && (nahoru == 0)){
				SubMenu3_SameScreen(count);
				CursorUp();
				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);
				line = 1;
			}
			if((line == 2) && (nahoru == 1)){
				SubMenu3_NewScreen(count);
				CursorUp();
				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);
				nahoru = 0;
				line = 1;
			}
			if(button == BUTTON_PRESSED){
				menu = 2;
				line = 0;
				count = 0;
				nahoru = 0;
				button = BUTTON_NOT_PRESSED;
			}
		  break;
	  case(1):
			 if((line == 1) && (nahoru == 0)){
				SubMenu3_SameScreen(count - 1);
				CursorDown();

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				line = 2;
			 }
			 if((line == 3) && (nahoru == 0)){
				SubMenu3_SameScreen(count);
				CursorUp();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				nahoru = 1;
				line = 2;
			 }
			 if((line == 3) && (nahoru == 1)){
				SubMenu3_NewScreen(count);
				CursorUp();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				nahoru = 1;
				line = 2;
			 }
			 if(button == BUTTON_PRESSED){
				 if(nahoru == 1){
					 LCD1602_setCursor(1, 6);
					 LCD1602_print("*");
					 voltagesetvolba = 1;
					 count = 0;
					 menu = 4;
				 }
				 if(nahoru == 0){
					 LCD1602_setCursor(2, 6);
					 LCD1602_print("*");
					 voltagesetvolba = 1;
					 count = 0;
					 menu = 4;
				 }
			 	button = BUTTON_NOT_PRESSED;
			 }
		  break;
	  case(2):
			if((line == 2) && (nahoru == 0)){
				SubMenu3_NewScreen(count - 1);
				CursorDown();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				line = 3;
			}
			if((line == 2) && (nahoru == 1)){
				SubMenu3_SameScreen(count - 1);
				CursorDown();
				nahoru = 0;
				line = 3;

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset1, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

			}
			if((line == 4) && (nahoru == 0)){
				SubMenu3_SameScreen(count);
				CursorUp();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset3, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				nahoru = 1;
				line = 3;
			}
			if(button == BUTTON_PRESSED){
				 if(nahoru == 1){
					 LCD1602_setCursor(1, 6);
					 LCD1602_print("*");
					 count = 0;
					 voltagesetvolba = 2;
					 menu = 4;
				 }
				 if(nahoru == 0){
					 LCD1602_setCursor(2, 6);
					 LCD1602_print("*");
					 count = 0;
					 voltagesetvolba = 2;
					 menu = 4;
				 }
				button = BUTTON_NOT_PRESSED;
			 }
		  break;
	  case(3):
			if((line == 3) && (nahoru == 0)){
				SubMenu3_NewScreen(count - 1);
				CursorDown();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset3, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				line = 4;
			}
			if((line == 3) && (nahoru == 1)){
				SubMenu3_SameScreen(count - 1);
				CursorDown();

				LCD1602_setCursor(1, 10);
				LCDSendAndInteger(voltageset2, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(1, 15);
				LCD1602_print(mV);

				LCD1602_setCursor(2, 10);
				LCDSendAndInteger(voltageset3, 4);
				LCD1602_print(mezera);
				LCD1602_setCursor(2, 15);
				LCD1602_print(mV);

				nahoru = 0;
				line = 4;
			}
			if(button == BUTTON_PRESSED){
			 if(nahoru == 0){
				 LCD1602_setCursor(2, 6);
				 LCD1602_print("*");
				 count = 0;
				 voltagesetvolba = 3;
				 menu = 4;
			 }
			button = BUTTON_NOT_PRESSED;
		 }
		  break;
	  }
}
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
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  HAL_ADC_Start_DMA(&hadc, adc_dma_values, 3);
  LCDInitialize();
  LCD1602_clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  RotationDirectionDetermination();
	  CounterCheck();
	  switch(menu){
	  case 0:
		  DisplayMainMenu();
		  break;
	  case 1:
		  DisplaySubMenu1();
		  break;
	  case 2:
		  DisplaySubMenu2();
		  break;
	  case 3:
		  DisplaySubMenu3();
		  break;
	  case 4:
		  VoltageSetFunction(voltagesetvolba);
		  break;
	  default:
	  	  break;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 500;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, RS_Pin|E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RELAY1_Pin|RELAY2_Pin|RELAY3_Pin|D7_Pin
                          |D6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, D4_Pin|D5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS_Pin E_Pin */
  GPIO_InitStruct.Pin = RS_Pin|E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : RELAY1_Pin RELAY2_Pin RELAY3_Pin D7_Pin
                           D6_Pin */
  GPIO_InitStruct.Pin = RELAY1_Pin|RELAY2_Pin|RELAY3_Pin|D7_Pin
                          |D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : D4_Pin D5_Pin */
  GPIO_InitStruct.Pin = D4_Pin|D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DT_Pin */
  GPIO_InitStruct.Pin = DT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_Pin SWITCH_Pin */
  GPIO_InitStruct.Pin = CLK_Pin|SWITCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

ticker = 1;

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
