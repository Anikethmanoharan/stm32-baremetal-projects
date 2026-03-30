#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"

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
	//1. configure mode of gpio pin   -- there are 4 posssible modes
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		//non interupt mode
		temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
		pGPIOHandle->pGPIOx->MODER &= ~(0x3  << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);//clearing bits
		pGPIOHandle->pGPIOx->MODER |= temp; //setting

	}else
	{
		//this part we do later //interupt mode
	}

	temp = 0;

	//2. configure speed of gpio pin
	temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->OSPEEDER &= ~(0x3  << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);//clearing bits
	pGPIOHandle->pGPIOx->OSPEEDER |= temp;

	temp = 0;

	//3. configure pupd setting
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3  << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);//clearing bits
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

		pGPIOHandle->pGPIOx->AFR[temp1] &= (0xF << (4 * temp2));

		pGPIOHandle->pGPIOx->AFR[temp1] |= pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2);
	}
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
	value = (uint8_t)(pGPIOx->IDR >> PinNumber) & 0x00000001;
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
	pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber);
}

//IRQ Configuration and ISR Handling

/**
	* @fn  		- GPIO_IRQConfig
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
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi)
{
  //TODO
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
  //TODO
}

