/*
 * ADC1Configuration.h
 *
 *  Created on: 27. 10. 2020
 *      Author: mikov
 */

#ifndef INC_ADC1CONFIGURATION_H_
#define INC_ADC1CONFIGURATION_H_

#include "DelayFunction.h"

unsigned int CalibrationValue = 0;
unsigned short ConvertedValue;

void ADC1ClockConfiguration(unsigned short prescaler);
void ADC1Calibration(void);
void ADC1Configuration(void);
void ADC1IRQEnable(void);
void ADC1Enable(void);
void TIM15Configuration(void);
void ADC1StartConversion(void);

void ADC1ClockConfiguration(unsigned short prescaler){

	ADC1_2_COMMON->CCR &= ~(ADC12_CCR_CKMODE);
	switch(prescaler){
	case(1):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1;
			break;
	case(2):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV2;
			break;
	case(4):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV4;
			break;
	case(6):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV6;
			break;
	case(8):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV8;
			break;
	case(10):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV10;
			break;
	case(12):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV12;
			break;
	case(16):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV16;
			break;
	case(32):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV32;
			break;
	case(64):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV64;
			break;
	case(128):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV128;
			break;
	case(256):
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV256;
			break;
	default:
		RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV256;
		break;
	}

	RCC->AHBENR |= RCC_AHBENR_ADC12EN;
}

void ADC1Calibration(void){

	ADC1->CR &= ~(ADC_CR_ADVREGEN);
	ADC1->CR |= ADC_CR_ADVREGEN_0;
	Delay(1);
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
	ADC1->CR |= ADC_CR_ADCAL;
	while((ADC1->CR & ADC_CR_ADCAL) != 0){

	}

	CalibrationValue = ADC1->CALFACT;

}

void ADC1Configuration(void){

	ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_ALIGN | ADC_CFGR_RES);
	ADC1->CFGR &= ~(ADC_CFGR_EXTEN);
	ADC1->CFGR |= ADC_CFGR_EXTEN_0;
	ADC1->CFGR |= ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_3;

	ADC1->SQR1 &= ~(ADC_SQR1_L);
	ADC1->SQR1 &= ~(ADC_SQR1_SQ1);
	ADC1->SQR1 |= ADC_SQR1_SQ1_0;

	ADC1->SMPR1 |= ADC_SMPR1_SMP1;

}

void ADC1IRQEnable(void){

	ADC1->IER |= ADC_IER_EOC;

	NVIC_EnableIRQ(ADC1_2_IRQn);
	NVIC_SetPriority(ADC1_2_IRQn, 0);

}

void ADC1Enable(void){

	do{
		   ADC1->CR |= ADC_CR_ADEN;
	   }while(ADC1->ISR && ADC_ISR_ADRDY == 0);

}

void TIM15Configuration(void){

	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;

	TIM15->CR2 &= ~(TIM_CR2_MMS);
	TIM15->CR2 |= TIM_CR2_MMS_1;
	TIM15->PSC = 999;
	TIM15->ARR = 4800;
	TIM15->CR1 |= TIM_CR1_CEN;

}

void ADC1StartConversion(void){

	ADC1->CR |= ADC_CR_ADSTART;

}

#endif /* INC_ADC1CONFIGURATION_H_ */
