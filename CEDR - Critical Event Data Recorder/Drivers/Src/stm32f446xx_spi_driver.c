#include "stm32f446xx.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

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

	//7. Configure Software Slave Management
	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

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
		while( SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET );//reset == reset --> wait (data still sending)
																	// set == reset -->txe set, breaks while loop (data sending over)
		//2.check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) //set  : 16bit , else : 8bit
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
			//pSPIx->DR = *(pTxBuffer);
			*((volatile uint8_t *)&pSPIx->DR) = *pTxBuffer;
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
void SPI_ReceieveData(SPI_RegDef_t *pSPIx ,  uint8_t *pRxBuffer , uint32_t Len)
{
	while(Len > 0)
	{
		//1.wait until RXNE is set
		while( SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET );

		//2. check the DFF bit
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16bit DFF
			//load the data from DR to RXbuffer
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			pRxBuffer += 2;
		}else
		{
			//8bit format
			//*(pRxBuffer) = pSPIx->DR;
			*(pRxBuffer) = *((volatile uint8_t *)&pSPIx->DR);
			Len--;
			pRxBuffer++;
		}
	}

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
	//1. find the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);//location of where we shift
	*(NVIC_PR_BASE_ADDR + iprx) |=  ( IRQPriority << shift_amount );

	//REMEMBER : lower 4 bits of each 8bit section of an IPR reg is not implemented
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
	uint8_t temp1, temp2;
	//check for txe
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE); //flgset- temp1 = 1, flgrest - temp1 = 0
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		//handle txe
		spi_txe_interrupt_handle(pHandle);
	}

	//check for RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE); //flgset- temp1 = 1, flgrest - temp1 = 0
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//handle RXNE
		spi_rxne_interrupt_handle(pHandle);
	}


	//check the OVR flag
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR); //flgset- temp1 = 1, flgrest - temp1 = 0
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//handle OVR error
		spi_ovr_err_interrupt_handle(pHandle);
	}

}


uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle , uint8_t *pTxBuffer , uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//1. save Txbuffer and Len  information to a global variable. we created it in the handle struct
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//2. mark the SPI state as busy in transimission so that no other code can take over the same spi peripheral
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. enable TXEIE control bit to get interrupt whenever TXE flag is set in status register
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE); // (1 << 7)

		//4. data transission will be handled by ISR code -> spi_txe_interrupt_handle() @SPI_IRQHandling
	}

	return state; //why
}


uint8_t SPI_ReceieveDataIT(SPI_Handle_t *pSPIHandle ,  uint8_t *pRxBuffer , uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		//1. save Rxbuffer and Len  information to a global variable. we created it in the handle struct
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		//2. mark the SPI state as busy in reception so that no other code can take over the same spi peripheral
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		//3. enable RXNEIE control bit to get interrupt whenever TXE flag is set in status register
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE); // (1 << 7)

		//4. data reception will be handled by ISR code (TODO)
	}

	return state;
}



/*********************************************************************************************************/
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



/**
	* @fn  		- SPI_SSIConfig
	*
	* @brief   	- Function to
	*
	* @param  	-
	*
	* @return	- none
	*
	* @note 	- none
	*/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

/**
	* @fn  		- SPI_SSOEConfig
	*
	* @brief   	- Function to
	*
	* @param  	-
	*
	* @return	- none
	*
	* @note 	- none
	*/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}

/*****************some helper functions***********************/

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//2.check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) //set  : 16bit , else : 8bit
	{
		//16bit format
		//load data into DR reg
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer += 2;
	}else
	{
		//8bit format
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if(!pSPIHandle->TxLen)
	{
		//TxLEn is zero, close spi transmission, and inform application that tx is over
		//this prevents interupts from setting txe flag
		SPI_CloseTransmission(pSPIHandle); //return to normal state
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_TX_CMPLT);
	}

}
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//2.check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) //set  : 16bit , else : 8bit
	{
		//16bit format
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer += 2;
	}else
	{
		//8bit format
		*pSPIHandle->pRxBuffer = (uint8_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(!pSPIHandle->RxLen)
	{
		//reception is complete
		//turn off the rxneie interrupt
		SPI_CloseReception(pSPIHandle); //return to normal state
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT);
	}

}
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	//1.clear the ovr flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	//2.inform the application
	SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_ERR);
}



void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;

}
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;

}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}


__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle , uint8_t AppEvent)
{
	//this is a weak implementation , application may override this function.
}
