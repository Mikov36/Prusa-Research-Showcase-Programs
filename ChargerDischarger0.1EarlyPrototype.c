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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile short button = 0;
volatile short rotation = 0;
volatile int count = 0;
volatile short last = 0;
int progtime;
int menu = 1;
int line = 0;
unsigned long adc_dma_values[5];
int cell_voltage_enabler = 0;
int cell_current_enabler = 0;
int cell_capacity_enabler = 0;
int button_menu_enabler = 0;
int cell_voltage1;
int cell_voltage2;
int cell_voltage3;
int cell_current = 0;
float capacity1 = 20.;
float capacity2 = 20.;
float capacity3 = 20.;
float coefitient_voltage = 1.048;
int start_discharge1 = 0;
int start_discharge2 = 0;
int start_discharge3 = 0;
int stop_discharge1 = 0;
int stop_discharge2 = 0;
int stop_discharge3 = 0;
int discharge_set1 = 3100;
int discharge_set2 = 3100;
int discharge_set3 = 3100;
int cell1 = 0;
int cell2 = 0;
int cell3 = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void RotationDirectionDetermination();
void EncoderInterruptReset();
void HlavniMenu();
void CounterCheck();
void EmptyPress();
void VoltageSetFunctions();
void CellVoltage();
void VentilatorStop();
void Discharge();
void StopDischarge();
void StopDischargeConditional();
void CellCapacityCalculation();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void CellCapacityCalculation(){

	if(cell_capacity_enabler == 1){
		if(cell1 == 1){
			capacity1 = capacity1 + ((2500/(60*60))/2);
		}
		if(cell2 == 1){
			capacity2 = capacity2 + ((2500/(60*60))/2);
		}
		if(cell3 == 1){
			capacity3 = capacity3 + ((2500/(60*60))/2);
		}
		cell_capacity_enabler = 0;
	}
}

void StopDischargeConditional(){

	if(cell1 == 1){
		if(cell_voltage1 < discharge_set1){
			stop_discharge1 = 1;
		}
	}
	if(cell2 == 1){
		if(cell_voltage2 < discharge_set2){
			stop_discharge2 = 1;
		}
	}
	if(cell3 == 1){
		if(cell_voltage3 < discharge_set3){
			stop_discharge3 = 1;
		}
	}
}

void StopDischarge(){

	if(stop_discharge1 == 1){
		HAL_GPIO_WritePin(DISCH1_GPIO_Port, DISCH1_Pin, GPIO_PIN_RESET);
		cell1 = 0;
		stop_discharge1 = 0;
	}
	if(stop_discharge2 == 1){
		HAL_GPIO_WritePin(DISCH2_GPIO_Port, DISCH2_Pin, GPIO_PIN_RESET);
		cell2 = 0;
		stop_discharge2 = 0;
	}
	if(stop_discharge3 == 1){
		HAL_GPIO_WritePin(DISCH3_GPIO_Port, DISCH3_Pin, GPIO_PIN_RESET);
		cell3 = 0;
		stop_discharge3 = 0;
	}

}

void Discharge(){
	// Tato funkce slouzi ke spinani REFE pro ZAHAJENI vybijeni, automaticky se zapinaji ventilatory.


	if(start_discharge1 == 1){
		if(cell_voltage1 > 3000){
			HAL_GPIO_WritePin(DISCH1_GPIO_Port, DISCH1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(VENT_GPIO_Port, VENT_Pin, GPIO_PIN_SET);
			cell1 = 1;
		}
		start_discharge1 = 0;
	}

	if(start_discharge2 == 1){
		if(cell_voltage2 > 3000){
			HAL_GPIO_WritePin(DISCH2_GPIO_Port, DISCH2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(VENT_GPIO_Port, VENT_Pin, GPIO_PIN_SET);
			cell2 = 1;
		}
		start_discharge2 = 0;
	}

	if(start_discharge3 == 1){
		if(cell_voltage3 > 3000){
			HAL_GPIO_WritePin(DISCH3_GPIO_Port, DISCH3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(VENT_GPIO_Port, VENT_Pin, GPIO_PIN_SET);
			cell3 = 1;
		}
		start_discharge3 = 0;
	}
}

void VentilatorStop(){

	if((cell1 == 0) && (cell2 == 0) && (cell3 == 0)){
			HAL_GPIO_WritePin(VENT_GPIO_Port, VENT_Pin, GPIO_PIN_RESET);
		}
}

void CellVoltage(){

	if(cell_voltage_enabler == 1){
		const int a = 350;
		const int b = 270;
		const int c = 200;
		cell_voltage1 = coefitient_voltage*adc_dma_values[0] + a;
		cell_voltage2 = coefitient_voltage*adc_dma_values[1] + b;
		cell_voltage3 = coefitient_voltage*adc_dma_values[2] + c;
		cell_voltage_enabler = 0;
	}
}

void RotationDirectionDetermination(){
/*
 * This function determines the rotation of the encoder, it increments or decrements according to the
 * conditionals in the interrupt c. file
 */
	if((rotation == 1) && (last == 1)){
		count++;
	 	rotation = 0;
	 	}

	if((rotation == 2) && (last == 1)){
	 	count--;
	 	rotation = 0;
	 	}
}

void EncoderInterruptReset(){
/*
 * This function nulls the LAST integer The delay there is to prevent bounces and so the interrupt
 * conditions will activate after the encoder is stable on 1 1.
 */
	if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)) {
		HAL_Delay(5);
		if(HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin) && HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin)){
			last = 0;
		 	}
		}
}

void EmptyPress(){
/*
 * This function nulls the switch integer, so when pressed on empty row, nothing happens and
 * it nulls so you can use it again
 * */
	if(button == 1){
		button = 0;
		}
}

void VoltageSetFunctions(){

	if(menu == 5){
		LCD1602_setCursor(2, 3);
		LCDSendAndInteger(discharge_set1, 4);
		discharge_set1 = discharge_set1 + (count * 10);
		count = 0;

		if(discharge_set1 < 3100){
			discharge_set1 = 3100;
		}
		if(discharge_set1 > 4000){
			discharge_set1 = 4000;
		}
	if(button == 1){
		LCD1602_clear();
		menu = 4;
		count = 1;
		button = 0;
	}
	}
	if(menu == 6){
		LCD1602_setCursor(2, 3);
		LCDSendAndInteger(discharge_set2, 4);
		discharge_set2 = discharge_set2 + (count * 10);
		count = 0;

		if(discharge_set2 < 3100){
			discharge_set2 = 3100;
		}
		if(discharge_set2 > 4000){
			discharge_set2 = 4000;
		}
	if(button == 1){
		LCD1602_clear();
		menu = 4;
		count = 2;
		button = 0;
	}

	}
	if(menu == 7){
		LCD1602_setCursor(2, 3);
		LCDSendAndInteger(discharge_set3, 4);
		discharge_set3 = discharge_set3 + (count * 10);
		count = 0;

		if(discharge_set3 < 3100){
			discharge_set3 = 3100;
		}
		if(discharge_set3 > 4000){
			discharge_set3 = 4000;
		}
		if(button == 1){
			LCD1602_clear();
			menu = 4;
			count = 3;
			button = 0;
		}
	}
}

void HlavniMenu(){
/*
 * This is the main function for the menu. The switch function switches between cases of the count of the encoder
 * and then determines the menu the LCD is in, the line integer serves to determine if you are scrolling down or
 * up. If you want to press a button on certain case and menu, there is an if statement to switch between menus and
 * put the cursor back on case 0.
*/
	switch(count){
		case 0:
			if(menu == 1){
				if((line == 0) || (line == 2)){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b00111110);
					LCD1602_print("CELL VOLTAGE");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print("DISCHARGE     ");
					LCD1602_setCursor(2, 16);
					SendACharacter(0b01011110);
					line = 1;
					}
				if(button == 1){
					LCD1602_clear();
					menu = 2;
					line = 0;
					count = 0;
					button = 0;
					}
				}
			if(menu == 2){
				if(line == 2){
					LCD1602_clear();
					line = 0;
				}
				if(line == 0){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b00111110);
					LCD1602_print("MAIN MENU");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print("BAT1 BAT2 BAT3");
					line = 1;
				}

					if(button == 1){
						LCD1602_clear();
						menu = 1;
						line = 0;
						count = 0;
						button = 0;
						}
			}

		if(menu == 3){
			if((line == 0) || (line == 2)){
				LCD1602_setCursor(1, 1);
				SendACharacter(0b00111110);
				LCD1602_print("MAIN MENU");
				LCD1602_setCursor(1, 16);
				SendACharacter(0b01011110);
				LCD1602_setCursor(2, 1);
				LCD1602_print("v");
				LCD1602_print("DISCHARGE SET ");
				LCD1602_setCursor(2, 16);
				SendACharacter(0b01011110);
				line = 1;
			}
			if(button == 1){
				LCD1602_clear();
				menu = 1;
				line = 0;
				count = 0;
				button = 0;
				}

		}
		if(menu == 4){
			if((line == 0) || (line == 2)){
				if(line == 2){
				LCD1602_clear();
				}
				LCD1602_setCursor(1, 1);
				SendACharacter(0b00111110);
				LCD1602_print("BACK");
				LCD1602_setCursor(1, 16);
				SendACharacter(0b01011110);
				LCD1602_setCursor(2, 1);
				LCD1602_print("v");
				LCD1602_print(" ");
				LCD1602_print("BAT1: ");
				LCDSendAndInteger(discharge_set1, 4);
				LCD1602_print("mV");
				line = 1;
			}
			if(button == 1){
				LCD1602_clear();
				menu = 3;
				line = 0;
				count = 0;
				button = 0;
				}
		}
		if(menu == 8){
			if((line == 0) || (line == 2)){
				LCD1602_clear();
				LCD1602_setCursor(1, 1);
				SendACharacter(0b00111110);
				LCD1602_print(" MAIN MENU ");
				LCD1602_setCursor(1, 16);
				SendACharacter(0b01011110);
				LCD1602_setCursor(2, 1);
				LCD1602_print("v");
				LCD1602_print(" RESET ");
				line = 1;
			}
			if(button == 1){
				LCD1602_clear();
				menu = 1;
				line = 0;
				count = 0;
				button = 0;
			}
		}
			break;
		case 1:
			if(menu == 1){
				if(line == 1){
					LCD1602_setCursor(1, 1);
					LCD1602_print("CELL VOLTAGE ");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("DISCHARGE");
					LCD1602_setCursor(2, 16);
					SendACharacter(0b01011110);
					line = 2;
					}
				if(line == 3){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					SendACharacter(0b00111110);
					LCD1602_print("DISCHARGE");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("CAPACITY (mAh) ");
					LCD1602_setCursor(2, 16);
					SendACharacter(0b01011110);
					line = 2;
					}
				if(button == 1){
					LCD1602_clear();
					count = 0;
					line = 0;
					menu = 3;
					button = 0;
					}
				}
			if(menu == 2){
				if(line == 1){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("BAT1 BAT2 BAT3");
					line = 2;
					LCD1602_setCursor(2, 15);
					LCD1602_print("mV");
				}
					if(cell_voltage1 == 0){
						LCD1602_setCursor(2, 1);
						LCD1602_print("0");
						}
					if(cell_voltage2 == 0){
						LCD1602_setCursor(2, 6);
						LCD1602_print("0");
						}
					if(cell_voltage3 == 0){
						LCD1602_setCursor(2, 6);
						LCD1602_print("0");
						}
					if(cell_voltage1 != 0){
						LCD1602_setCursor(2, 1);
						LCDSendAndInteger(cell_voltage1, 4);
						LCD1602_print(" ");
					}
					if(cell_voltage2 != 0){
						LCD1602_setCursor(2, 6);
						LCDSendAndInteger(cell_voltage2, 4);
						LCD1602_print(" ");
					}
					if(cell_voltage3 != 0){
						LCD1602_setCursor(2, 11);
						LCDSendAndInteger(cell_voltage3, 4);
						if(cell_voltage3 < 1000){
						LCD1602_print(" ");
						}
					}
					EmptyPress();
				}
			if(menu == 3){
				if((line == 1) || (line == 3)){
					if(line == 3){
						LCD1602_clear();
					}
					LCD1602_setCursor(1, 1);
					LCD1602_print("MAIN MENU ");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("DISCHARGE SET");
					LCD1602_setCursor(2, 16);
					SendACharacter(0b01011110);
					line = 2;
				}
				if(button == 1){
					LCD1602_clear();
					menu = 4;
					line = 0;
					count = 0;
					button = 0;
					}
			}
			if(menu == 4){
				if(line == 1){
					LCD1602_setCursor(1, 1);
					LCD1602_print(" BACK");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("BAT1: ");
					LCDSendAndInteger(discharge_set1, 4);
					LCD1602_print("mV");
					line = 2;
				}
				if(line == 3){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					SendACharacter(0b00111110);
					LCD1602_print("BAT1: ");
					LCDSendAndInteger(discharge_set1, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print(" BAT2: ");
					LCDSendAndInteger(discharge_set2, 4);
					LCD1602_print("mV");
					line = 2;
				}
				if(button == 1){
					LCD1602_clear();
					menu = 5;
					count = 0;
					line = 1;
					LCD1602_setCursor(1, 1);
					LCD1602_print("CELL 1 VOLTAGE");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" :");
					LCD1602_setCursor(2, 7);
					LCD1602_print("mV");
					button = 0;
					}
				}
			if(menu == 8){
				if((line == 1) || (line == 3)){
					if(line == 3){
						LCD1602_clear();
					}
					LCD1602_setCursor(1, 1);
					LCD1602_print(" MAIN MENU ");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print(" RESET ");
					line = 2;
				}
				if(button == 1){
					capacity1 = 0;
					capacity2 = 0;
					capacity3 = 0;
					button = 0;
				}
			}
			break;
		case 2:
			if(menu == 1){
				if(line == 2){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("DISCHARGE");
					LCD1602_setCursor(1, 16);
					SendACharacter(0b01011110);
					LCD1602_setCursor(2, 1);
					SendACharacter(0b00111110);
					LCD1602_print("CAPACITY (mAh) ");
					LCD1602_setCursor(2, 16);
					SendACharacter(0b01011110);
					line = 3;
					}
				if(button == 1){
					LCD1602_clear();
					count = 0;
					line = 0;
					menu = 8;
					button = 0;
					}
				}
			if(menu == 3){
				if((line == 2) || (line == 4)){
					if(line == 2){
						LCD1602_clear();
					}
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("START ");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("BAT1 BAT2 BAT3");
					line = 3;
				}
				if(button == 1){
					start_discharge1 = 1;
					button = 0;
					}
			}
			if(menu == 4){
				if(line == 2){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print(" BAT1: ");
					LCDSendAndInteger(discharge_set1, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("BAT2: ");
					LCDSendAndInteger(discharge_set2, 4);
					LCD1602_print("mV");
					line = 3;
				}

				if(line == 4){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					SendACharacter(0b00111110);
					LCD1602_print("BAT2: ");
					LCDSendAndInteger(discharge_set2, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print(" BAT3: ");
					LCDSendAndInteger(discharge_set3, 4);
					LCD1602_print("mV");
					line = 3;
				}

				if(button == 1){
					LCD1602_clear();
					menu = 6;
					count = 0;
					line = 2;
					LCD1602_setCursor(1, 1);
					LCD1602_print("CELL 2 VOLTAGE");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" :");
					LCD1602_setCursor(2, 7);
					LCD1602_print("mV");
					button = 0;
					}
				}
			if(menu == 8){
				if(line == 2){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("BAT1 BAT2 BAT3");
					line = 3;
				}
				LCD1602_setCursor(2, 1);
				LCDSendAndInteger((int)capacity1, 4);
				LCD1602_setCursor(2, 6);
				LCDSendAndInteger((int)capacity2, 4);
				LCD1602_setCursor(2, 11);
				LCDSendAndInteger((int)capacity3, 4);
				EmptyPress();
			}

			break;
		case 3:
			if(menu == 3){
				if((line == 3) || (line == 5)){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("START ");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print(" BAT1");
					SendACharacter(0b00111110);
					LCD1602_print("BAT2 BAT3");
					line = 4;
				}
				if(button == 1){
					start_discharge2 = 1;
					button = 0;
					}
				}
			if(menu == 4){
				if(line == 3){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print(" BAT2: ");
					LCDSendAndInteger(discharge_set2, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					SendACharacter(0b00111110);
					LCD1602_print("BAT3: ");
					LCDSendAndInteger(discharge_set3, 4);
					LCD1602_print("mV");
					line = 4;
				}
				if(line == 5){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					SendACharacter(0b00111110);
					LCD1602_print("BAT3: ");
					LCDSendAndInteger(discharge_set3, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" CURRENT: ");
					LCD1602_setCursor(2, 15);
					LCD1602_print("mA");
					line = 4;
				}
				LCD1602_setCursor(2, 11);
				LCDSendAndInteger(cell_current, 4);
				if(button == 1){
					LCD1602_clear();
					menu = 7;
					count = 0;
					line = 3;
					LCD1602_setCursor(1, 1);
					LCD1602_print("CELL 3 VOLTAGE");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" :");
					LCD1602_setCursor(2, 7);
					LCD1602_print("mV");
					button = 0;
					}
			}
			break;
		case 4:
			if(menu == 3){
				if((line == 4) || (line == 6)){
					if(line == 6){
						LCD1602_clear();
					}
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("START ");
					LCD1602_setCursor(2, 1);
					LCD1602_print("v");
					LCD1602_print(" BAT1 BAT2");
					SendACharacter(0b00111110);
					LCD1602_print("BAT3");
					line = 5;
				}
				if(button == 1){
					start_discharge3 = 1;
					button = 0;
					}
			}
			if(menu == 4){
				if(line == 4){
					LCD1602_clear();
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print(" BAT3: ");
					LCDSendAndInteger(discharge_set3, 4);
					LCD1602_print("mV");
					LCD1602_setCursor(2, 1);
					SendACharacter(0b00111110);
					LCD1602_print("CURRENT: ");
					LCD1602_setCursor(2, 15);
					LCD1602_print("mA");
					line = 5;
				}
				LCD1602_setCursor(2, 11);
				LCDSendAndInteger(cell_current, 4);
				EmptyPress();
			}
			break;
		case 5:
			if(menu == 3){
				if((line == 5) || (line == 7)){
					if(line == 5){
						LCD1602_clear();
					}
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("STOP");
					LCD1602_setCursor(2, 1);
					SendACharacter(0b00111110);
					LCD1602_print("BAT1 BAT2 BAT3");
					line = 6;
				}
				if(button == 1){
					stop_discharge1 = 1;
					button = 0;
					}
			}
			break;
		case 6:
			if(menu == 3){
				if((line == 6) || (line == 8)){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("STOP");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" BAT1");
					SendACharacter(0b00111110);
					LCD1602_print("BAT2 BAT3");
					line = 7;
				}
				if(button == 1){
					stop_discharge2 = 1;
					button = 0;
					}
			}
			break;
		case 7:
			if(menu == 3){
				if(line == 7){
					LCD1602_setCursor(1, 1);
					SendACharacter(0b01011110);
					LCD1602_print("STOP");
					LCD1602_setCursor(2, 1);
					LCD1602_print(" BAT1 BAT2");
					SendACharacter(0b00111110);
					LCD1602_print("BAT3");
					line = 8;
				}
				if(button == 1){
					stop_discharge3 = 1;
					button = 0;
					}
			}
			break;
	}
}
void CounterCheck(){
/* This function checks and corrects the count pulses depends on your MENU,
 * if you need more lines or more sub-menus, you can just add them by adding
 * more conditionals
 * */
	if(menu == 1){
		if(count > 2){
			count = 2;
		}
		if(count < 0){
			count = 0;
		}
	}
	if(menu == 2){
		if(count > 1){
			count = 1;
		}
		if(count < 0){
			count = 0;
		}
	}
	if(menu == 3){
		if(count > 7){
			count = 7;
		}
		if(count < 0){
			count = 0;
		}
	}
	if(menu == 4){
		if(count > 5){
			count = 5;
		}
		if(count < 0){
			count = 0;
		}
	}
	if(menu == 8){
		if(count > 2){
			count = 2;
		}
		if(count < 0){
			count = 0;
		}
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
  HAL_ADC_Start_DMA(&hadc, adc_dma_values, 5);
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
	 EncoderInterruptReset();
	 CounterCheck();
	 HlavniMenu();
	 VoltageSetFunctions();
	 CellVoltage();
	 VentilatorStop();
	 Discharge();
	 StopDischarge();
	 StopDischargeConditional();
	 CellCapacityCalculation();
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
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_4;
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
  HAL_GPIO_WritePin(GPIOA, VENT_Pin|D4_Pin|D5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DISCH1_Pin|DISCH2_Pin|DISCH3_Pin|D7_Pin
                          |D6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS_Pin E_Pin */
  GPIO_InitStruct.Pin = RS_Pin|E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : VENT_Pin D4_Pin D5_Pin */
  GPIO_InitStruct.Pin = VENT_Pin|D4_Pin|D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DISCH1_Pin DISCH2_Pin DISCH3_Pin D7_Pin
                           D6_Pin */
  GPIO_InitStruct.Pin = DISCH1_Pin|DISCH2_Pin|DISCH3_Pin|D7_Pin
                          |D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

cell_voltage_enabler = 1;
cell_capacity_enabler = 1;

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
