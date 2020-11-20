/*
 * EXTIFallingEdge.h
 *
 *  Created on: 27. 10. 2020
 *      Author: mikov
 */

#ifndef INC_EXTIFALLINGEDGE_H_
#define INC_EXTIFALLINGEDGE_H_

void EXTIChannelSetup(GPIO_TypeDef *port, unsigned short pin);
void EXTIFallingEdgeConfiguration(unsigned short pin);
void EXTINVICEnable(unsigned short pin);


void EXTIChannelSetup(GPIO_TypeDef *port, unsigned short pin){

	if(pin < 4){
		if(port == GPIOA){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA << (pin * 4);
		}
		if(port == GPIOB){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB << (pin * 4);
		}
		if(port == GPIOC){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC << (pin * 4);
		}
		if(port == GPIOD){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PD << (pin * 4);
		}
		if(port == GPIOE){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PE << (pin * 4);
		}
		if(port == GPIOF){
			SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PF << (pin * 4);
		}
	}

	if((3 < pin) && (pin < 8)){
		if(port == GPIOA){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA << ((pin - 4) * 4);
		}
		if(port == GPIOB){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PB << ((pin - 4) * 4);
		}
		if(port == GPIOC){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PC << ((pin - 4) * 4);
		}
		if(port == GPIOD){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PD << ((pin - 4) * 4);
		}
		if(port == GPIOE){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PE << ((pin - 4) * 4);
		}
		if(port == GPIOF){
			SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PF << ((pin - 4) * 4);
		}
	}

	if((7 < pin) && (pin < 12)){
		if(port == GPIOA){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA << ((pin - 8) * 4);
		}
		if(port == GPIOB){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PB << ((pin - 8) * 4);
		}
		if(port == GPIOC){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PC << ((pin - 8) * 4);
		}
		if(port == GPIOD){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PD << ((pin - 8) * 4);
		}
		if(port == GPIOE){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PE << ((pin - 8) * 4);
		}
		if(port == GPIOF){
			SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PF << ((pin - 8) * 4);
		}
	}

	if((11 < pin) && (pin < 16)){
		if(port == GPIOA){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PA << ((pin - 12) * 4);
		}
		if(port == GPIOB){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB << ((pin - 12) * 4);
		}
		if(port == GPIOC){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC << ((pin - 12) * 4);
		}
		if(port == GPIOD){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PD << ((pin - 12) * 4);
		}
		if(port == GPIOE){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PE << ((pin - 12) * 4);
		}
		if(port == GPIOF){
			SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PF << ((pin - 12) * 4);
		}
	}
}

void EXTIFallingEdgeConfiguration(unsigned short pin){

	EXTI->IMR |= 1 << pin;
	EXTI->FTSR |= 1 << pin;

}

void EXTINVICEnable(unsigned short pin){

	if(pin == 0){
		NVIC_SetPriority(EXTI0_IRQn, 0);
		NVIC_EnableIRQ(EXTI0_IRQn);
	}

	if(pin == 1){
			NVIC_SetPriority(EXTI1_IRQn, 0);
			NVIC_EnableIRQ(EXTI1_IRQn);
		}

	if(pin == 2){
			NVIC_SetPriority(EXTI2_TSC_IRQn, 0);
			NVIC_EnableIRQ(EXTI2_TSC_IRQn);
		}

	if(pin == 3){
			NVIC_SetPriority(EXTI3_IRQn, 0);
			NVIC_EnableIRQ(EXTI3_IRQn);
		}

	if(pin == 4){
			NVIC_SetPriority(EXTI4_IRQn, 0);
			NVIC_EnableIRQ(EXTI4_IRQn);
		}

	if((4 < pin) && (pin < 10)){
			NVIC_SetPriority(EXTI9_5_IRQn, 0);
			NVIC_EnableIRQ(EXTI9_5_IRQn);
		}

	if((9 < pin) && (pin < 16)){
			NVIC_SetPriority(EXTI15_10_IRQn, 0);
			NVIC_EnableIRQ(EXTI15_10_IRQn);
		}
}



#endif /* INC_EXTIFALLINGEDGE_H_ */
