#include "stm32f446xx.h"

//on-going project

//peripheral clock setup
/*************************************************************************************
	* @fn  		- SPI_PeriClockControl
	*
	* @brief   	- Function enables and disables peripheral clock for given SPI
	*
	* @param  	- base address of SPI peripheral
	* @param  	- ENABLE or DISABLE macros
	*
	* @return	-	none
	*
	* @note 	-
	*/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
	}
		else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}



//Init & De-Init

/*************************************************************************************
	* @fn  		- SPI_Init
	*
	* @brief   	- Function to initialize given SPIx
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//enabling the peri clock here rather than doing it in the application code
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);


	//CONFIGURE SPI_CRI REGISTER

	uint32_t tempreg = 0;

	//1.configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR; // same as (1 << 2)
																	//  given macro name for pins

	//2.configure bus config - FD,HD,SMPLX
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//BIDI mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//BIDI mode should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//BIDI mode should be cleared
		//RXONLY bit must be set
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	//3.configure SCLK speed
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4.configure the DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5.configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6.configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	pSPIHandle->pSPIx->CR1 = tempreg;
}



/**
	* @fn  		- SPI_DeInit
	*
	* @brief   	- Function to De-initialize given SPI
	*
	* @param  	- base address of SPI peripheral
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName) //flag name -> flag bit name in sr reg
{																	//flag name : (1 << SPI_SR_TXE)
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}
//Data send and receive
/**
	* @fn  		- Data_SendData
	*
	* @brief   	- Function to Send Data
	*
	* @param  	- base address of SPI peripheral
	* @param  	-
	*
	* @return	- none
	*
	* @note 	- this is a blocking call
	*/
void SPI_SendData(SPI_RegDef_t *pSPIx , uint8_t *pTxBuffer , uint32_t Len)
{
	while(Len > 0)
	{
		//1.wait until TXE is set
		while( SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET );

		//2.check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) //set  : 16bit else : 8bit
		{
			//16bit format
			//load data into DR reg
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len--;
			Len--;
			pTxBuffer += 2;
		}else
		{
			//8bit format
			pSPIx->DR = *(pTxBuffer);
			Len--;
			pTxBuffer++;
		}
	}

}



/**
	* @fn  		- Data_ReceieveData
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
void Data_ReceieveData(SPI_RegDef_t *pSPIx ,  uint8_t *pRxBuffer , uint32_t Len)
{

}



                        //IRQ Configuration and ISR Handling

/**
	* @fn  		- SPI_IRQInterruptConfig
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
void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
{

}


/**
	* @fn  		- SPI_IRQPriorityConfig
	*
	* @brief   	- Function to configure IRQs
	* @param  	- IRQ priority value
	* @return	- none
	*
	* @note 	- none
	*/
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority)
{

}


/**
	* @fn  		- SPI_IRQHandling
	*
	* @brief   	- Function to handle ISR
	*
	* @param  	- GPIO pin number
	*
	* @return	- none
	*
	* @note 	- none
	*/
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{

}

/**
	* @fn  		- SPI_PeripheralControl
	*
	* @brief   	- Function to enable and disable spi peripheral using spe bit of CR1 register
	*
	* @param  	- GPIO pin number
	*
	* @return	- none
	*
	* @note 	- none
	*/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}
