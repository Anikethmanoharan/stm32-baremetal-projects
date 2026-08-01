#include "stm32f446xx.h"


//peripheral clock setup
/**
	* @fn  		- GPIO_PeriClockControl
	*
	* @brief   	- Function enables and disables peripheral clock for given GPIO Port
	*
	* @param  	- base address of gpio peripheral
	* @param  	- ENABLE or DISABLE macros
	*
	* @return	-	none
	*
	* @note 	-
	*/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		}else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();
		}else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();
		}else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		}else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		}else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		}else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		}else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		}else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();
		}else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_DI();
		}else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();
		}
	}
}

//Init & De-Init

/**
	* @fn  		- GPIO_Init
	*
	* @brief   	- Function to initialize given GPIO
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0;

	//enabling the peripheral clock here ratherthan doing it in the application code
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	//1. configure mode of gpio pin   -- there are 4 posssible modes
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		//non interupt mode
		temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//clearing bits
		pGPIOHandle->pGPIOx->MODER |= temp; //setting

	}else
	{
		 //interupt mode  -- there are 3 modes which we propose
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			//1. configure falling trigger selection register[FTSR]
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			//clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//1. configure rising trigger selection register[RTSR]
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			//clear the corresponding FTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{

			//3. configure both FTSR AND RTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}
		//2. configure the gpio port selection in SYSCFG_EXTICR    [MUX TO SELECT THE PIN]

		//SYSCFG has 4 EXTICR registers (EXTICR[0] to EXTICR[3]). Each register handles 4 pins.

		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;  //which EXTI REG
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;  //which SLOT IN THE EXTI REG
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		//3, enable exti interupt delivery using interupt mask register [IMR]
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //here we are enabling delivery on the exti line
	}

	temp = 0;

	//2. configure speed of gpio pin
	//speed
	temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->OSPEEDER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//clearing bits
	pGPIOHandle->pGPIOx->OSPEEDER |= temp;

	temp = 0;

	//3. configure pupd setting
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));//clearing bits
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	//4. configure the output type
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1  << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);//clearing bits
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;

	//5. configure the alt functionality [if required]
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		//configure alternate function register
		uint8_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2)); //why 4 -> each slot is 4 bit

		pGPIOHandle->pGPIOx->AFR[temp1] |= pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2);
	}
	/*       Pins:   0 1 2 3 4 5 6 7 | 8 9 10 11 12 13 14 15
                     --------AFR[0]--|------AFR[1]---------   ->  GPIOA - GPIOH    */
}


/**
	* @fn  		- GPIO_DeInit
	*
	* @brief   	- Function to De-initialize given GPIO
	*
	* @param  	- base address of gpio peripheral
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)

{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();
	}else if(pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();
	}else if(pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();
	}else if(pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();
	}else if(pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();
	}else if(pGPIOx == GPIOF)
	{
		GPIOF_REG_RESET();
	}else if(pGPIOx == GPIOG)
	{
		GPIOG_REG_RESET();
	}else if(pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();
	}

}


//Data Read & Write
/**
	* @fn  		- GPIO_ReadFromInputPin
	*
	* @brief   	- Function to read from input pin
	*
	* @param  	- base address of gpio peripheral
	* @param  	- GPIO pin number
	*
	* @return	- none
	*
	* @note 	- none
	*/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)(pGPIOx->IDR >> PinNumber) & 0x00000001; //manipulating copy of IDR and shifting
	return value;
}

/**
	* @fn  		- GPIO_ReadFromInputPort
	*
	* @brief   	- Function to read from input port
	*
	* @param  	- base address of gpio peripheral
	*
	* @return	- 0 or 1
	*
	* @note 	- none
	*/
uint16_t  GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIO)
{
	uint16_t value;
	value = (uint16_t)pGPIO->IDR;
	return value;
}


/**
	* @fn  		- GPIO_WriteToOutputPin
	*
	* @brief   	- Function to write to output pin
	*
	* @param  	- base address of gpio peripheral
	* @param  	- GPIO pin number
	* @param  	- value
	*
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		//write 1 to ODR of the bit field corresponding to the pin number
		pGPIOx->ODR |= (1 << PinNumber);

	}else
	{
		//write 0
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}


/**
	* @fn  		- GPIO_WriteToOutputPort
	*
	* @brief   	- Function to write to output port
	*
	* @param  	- base address of gpio peripheral
	* @param  	- value
	*
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx , uint16_t Value)
{
	pGPIOx->ODR = Value;
}

/**
	* @fn  		- GPIO_ToggleOutputPin
	*
	* @brief   	- Function to toggle GPIO pin
	*
	* @param  	- base address of gpio peripheral
	* @param  	- GPIO pin number
	*
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber)
{
	pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber); // XOR - flips the value, 0 -> 1 , 1 -> 0
}


// HERE ONWARDS WE ARE USING PROCESSOR SIDE AND USE NVIC REGISTRS OF THE PROCESSOR
//IRQ Configuration and ISR Handling

/**
	* @fn  		- GPIO_IRQInterruptConfig
	*
	* @brief   	- Function to configure IRQs
	*
	* @param  	- base address of gpio peripheral
	* @param  	- IRQ priority value
	* @param    - ENABLE or DISABLE macros
	*
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi) //@GPIO_IRQConfig
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ISER1 register
			*NVIC_ISER1 |= (1 << IRQNumber % 32); //because we are on the nxt reg and we want to start counting from 0
												  //like bit 32 will be 0,cause 0-31 is in ISER0, from 32 its in ISER1
		}else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//program ISER2 register
			*NVIC_ISER3 |= (1 << IRQNumber % 64);
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ICER1 register
			*NVIC_ICER1 |= (1 << IRQNumber % 32);

		}else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//program ICER2 register
			*NVIC_ICER3 |= (1 << IRQNumber % 64);
		}

	}

}


/**
	* @fn  		- GPIO_IRQPriorityConfig
	*
	* @brief   	- Function to configure IRQs
	* @param  	- IRQ priority value
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority)
{
	//1. find the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);

	//REMEMBER : lower 4 bits of each 8bit section of an IPR reg is not implemented
}


/**
	* @fn  		- GPIO_IRQHandling
	*
	* @brief   	- Function to handle ISR
	*
	* @param  	- GPIO pin number
	*
	* @return	- none
	*
	* @note 	- none
	*/
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//1. clear the exti pending register corresponding to the pin number
	if(EXTI->PR & ( 1 << PinNumber))
	{
		//clear the pending register bit
		EXTI->PR = (1 << PinNumber); //to clear we write 1 (from register details)
	}

}


/*                     HOW DO WE USE THESE
 * 1. Look up pin number  →  find EXTI line number  (same as pin number)
 * 2. Look up EXTI line   →  find IRQ number        (from datasheet vector table)
 * 3. Use IRQ number      →  call GPIO_IRQInterruptConfig(IRQ_NO_EXTIx, ENABLE)
 * 4. Use IRQ number      →  call GPIO_IRQPriorityConfig(IRQ_NO_EXTIx, priority)
 * 5. Define the ISR      →  name comes from startup file vector table
 * 6. Inside ISR          →  call GPIO_IRQHandling(pin_number) first, always
 */
