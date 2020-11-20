/*
 * DelayFunction.h
 *
 *  Created on: 27. 10. 2020
 *      Author: mikov
 */

#ifndef INC_DELAYFUNCTION_H_
#define INC_DELAYFUNCTION_H_


volatile unsigned int TimingDelay = 0;

void Delay(unsigned int Delay);



void Delay(unsigned int Delay){

	TimingDelay = Delay;

	while(TimingDelay != 0){

	}

}

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	TimingDelay--;
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

#endif /* INC_DELAYFUNCTION_H_ */
