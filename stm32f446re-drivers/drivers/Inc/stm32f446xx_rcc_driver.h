

#ifndef INC_STM32F446XX_RCC_DRIVER_H_
#define INC_STM32F446XX_RCC_DRIVER_H_

#include "stm32f446xx.h"

//This return APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//This return APB2 clock value
uint32_t RCC_GetPCLK2Value(void);

//returns PLL clock value
uint32_t RCC_GetPLLOutputClock();


#endif /* INC_STM32F446XX_RCC_DRIVER_H_ */
