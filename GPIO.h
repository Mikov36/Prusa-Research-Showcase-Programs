/*
 * GPIO.h
 *
 *  Created on: 27. 10. 2020
 *      Author: mikov
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#define PORT_STATE_ON 1
#define PORT_STATE_OFF 0

unsigned short PortAState = PORT_STATE_OFF;
unsigned short PortBState = PORT_STATE_OFF;
unsigned short PortCState = PORT_STATE_OFF;
unsigned short PortDState = PORT_STATE_OFF;
unsigned short PortEState = PORT_STATE_OFF;
unsigned short PortFState = PORT_STATE_OFF;

void GPIOOutputSetup(GPIO_TypeDef *port, unsigned short pin);
void GPIOInputSetup(GPIO_TypeDef *port, unsigned short pin);
void GPIOAnalogInputSetup(GPIO_TypeDef *port, unsigned short pin);
void GPIOAlternateFunctionSetup(GPIO_TypeDef *port, unsigned short pin);
void GPIOPinSet(GPIO_TypeDef *port, unsigned short pin);
void GPIOPinReset(GPIO_TypeDef *port, unsigned short pin);

void GPIOOutputSetup(GPIO_TypeDef *port, unsigned short pin){

	if((port == GPIOA) && (PortAState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		PortAState = PORT_STATE_ON;
	}
	if((port == GPIOB) && (PortBState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		PortBState = PORT_STATE_ON;
	}
	if((port == GPIOC) && (PortCState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		PortCState = PORT_STATE_ON;
	}
	if((port == GPIOD) && (PortDState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIODEN;
		PortDState = PORT_STATE_ON;
	}
	if((port == GPIOE) && (PortEState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
		PortEState = PORT_STATE_ON;
	}
	if((port == GPIOF) && (PortFState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
		PortFState = PORT_STATE_ON;
	}

	port->MODER &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->MODER |= (1 << (pin * 2));
	port->OTYPER &= ~(1 << pin);
	port->OSPEEDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->PUPDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));

}

void GPIOInputSetup(GPIO_TypeDef *port, unsigned short pin){

	if((port == GPIOA) && (PortAState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		PortAState = PORT_STATE_ON;
	}
	if((port == GPIOB) && (PortBState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		PortBState = PORT_STATE_ON;
	}
	if((port == GPIOC) && (PortCState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		PortCState = PORT_STATE_ON;
	}
	if((port == GPIOD) && (PortDState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIODEN;
		PortDState = PORT_STATE_ON;
	}
	if((port == GPIOE) && (PortEState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
		PortEState = PORT_STATE_ON;
	}
	if((port == GPIOF) && (PortFState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
		PortFState = PORT_STATE_ON;
	}

	port->MODER &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->OTYPER &= ~(1 << pin);
	port->OSPEEDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->PUPDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));

}

void GPIOAnalogInputSetup(GPIO_TypeDef *port, unsigned short pin){

	if((port == GPIOA) && (PortAState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		PortAState = PORT_STATE_ON;
	}
	if((port == GPIOB) && (PortBState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		PortBState = PORT_STATE_ON;
	}
	if((port == GPIOC) && (PortCState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		PortCState = PORT_STATE_ON;
	}
	if((port == GPIOD) && (PortDState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIODEN;
		PortDState = PORT_STATE_ON;
	}
	if((port == GPIOE) && (PortEState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
		PortEState = PORT_STATE_ON;
	}
	if((port == GPIOF) && (PortFState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
		PortFState = PORT_STATE_ON;
	}

	port->MODER |= 0b11 << (pin * 2);
	port->OTYPER &= ~(1 << pin);
	port->OSPEEDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->PUPDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));

}

void GPIOAlternateFunctionSetup(GPIO_TypeDef *port, unsigned short pin){

	if((port == GPIOA) && (PortAState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		PortAState = PORT_STATE_ON;
	}
	if((port == GPIOB) && (PortBState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		PortBState = PORT_STATE_ON;
	}
	if((port == GPIOC) && (PortCState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		PortCState = PORT_STATE_ON;
	}
	if((port == GPIOD) && (PortDState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIODEN;
		PortDState = PORT_STATE_ON;
	}
	if((port == GPIOE) && (PortEState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
		PortEState = PORT_STATE_ON;
	}
	if((port == GPIOF) && (PortFState == PORT_STATE_OFF)){
		RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
		PortFState = PORT_STATE_ON;
	}

	port->MODER &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->MODER |= 1 << (pin * 2 + 1);
	port->OTYPER &= ~(1 << pin);
	port->OSPEEDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));
	port->PUPDR &= ~((1 << (pin * 2)) | (1 << (pin * 2 + 1)));

}


void GPIOPinSet(GPIO_TypeDef *port, unsigned short pin){

	port->BSRR |= 1 << pin;

}

void GPIOPinReset(GPIO_TypeDef *port, unsigned short pin){

	port->BRR |= 1 << pin;

}

void GPIOPinToggle(GPIO_TypeDef *port, unsigned short pin){

	unsigned int odr;
	unsigned int PinMask;

	odr = port->ODR;
	PinMask = 1 << pin;

	port->BSRR = ((odr & PinMask) << 16) | (~odr & PinMask);

}


#endif /* INC_GPIO_H_ */
