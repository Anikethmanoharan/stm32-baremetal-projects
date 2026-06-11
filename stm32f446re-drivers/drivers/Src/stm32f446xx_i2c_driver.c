#include "stm32f446xx.h"



static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);

static void I2cMasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2CMasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1; //MAKING ROOM FOR R/W BIT
	SlaveAddr &= ~(1);          // MAKING THAT BIT O TO WRITE (SLAVE ADDR + R/W BIT)
	pI2Cx->DR = SlaveAddr;      //PUTTING THE ADDRESS TO DR
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1; //MAKING ROOM FOR R/W BIT
	SlaveAddr |= 1;          // MAKING THAT BIT 1 TO WRITE (SLAVE ADDR + R/W BIT)
	pI2Cx->DR = SlaveAddr;      //PUTTING THE ADDRESS TO DR
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)   /*I2C_RegDef_t *pI2Cx*/
{
	uint32_t dummy_read;
	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		//device in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				//disble the ACk
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//Clear the ADDr flag (read SR1 & SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}else
		{
			//Clear the ADDr flag (read SR1 & SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}

	}else
	{
		//device in slave mode
		//Clear the ADDr flag (read SR1 & SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}


}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}


//peripheral clock setup
/*************************************************************************************
	* @fn  		- I2C_PeriClockControl
	*
	* @brief   	- Function enables and disables peripheral clock for given I2C
	*
	* @param  	- base address of I2C peripheral
	* @param  	- ENABLE or DISABLE macros
	*
	* @return	-	none
	*
	* @note 	-
	*/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}

	}
}



/*************************************************************************************
	* @fn  		- I2C_Init
	*
	* @brief   	- Function to initialize given I2Cx
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	//add periclkcontrol here
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	uint32_t tempreg = 0;

	//1.setting ACK -- enables acking
	tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << 10;  //ACK bit in cr1
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//2.configure the FREQ field of cr2  -- we write 16 here
	tempreg = 0;
	tempreg = RCC_GetPCLK1Value() / 1000000U ; //16MHZ / 1MHZ = 16 -> FREQ
	pI2CHandle->pI2Cx->CR2 = tempreg; //no shifting needed as 1st 5 bits are FREQ, tempreg & 0x3F

	//3.program the device own address - OAR1 reg  (own address register)
	tempreg = 0;
	tempreg = pI2CHandle->I2C_Config.I2C_DeviceAddress << 1; //because in OAR1 reg 0th bit is not valid we write from 1st bit
	tempreg |= (1 << 14);	//reference manual says 14th bit must be kept 1 by software
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//4. CCR calculations -- this is important t produce serial clock, ccr value decides the scl
	uint16_t ccr_value = 0;
	tempreg = 0;

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		//MODE IS STANDARD MODE
		//calculating ccr value FOR STANDARD MODE
		ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		tempreg |= (ccr_value & 0xFFF);
	}else
	{
		//MODE IS FAST MODE - below procedures are based on reference manual

		//setting to fast mode
		tempreg |= (1 << 15);

		//programming the duty cycle
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);

		//calculating ccr value FOR FAST MODE - there are 2 equations
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
		{
			ccr_value = RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}else
		{
			ccr_value = RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		}
		tempreg |= (ccr_value & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = tempreg;

	//5.TRISE CONFIGURATION
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		//MODE IS STANDARD MODE
		tempreg = (RCC_GetPCLK1Value() / 1000000U) + 1;

	}else
	{
		//MODE IS FAST MODE
		tempreg = (( RCC_GetPCLK1Value() * 300 ) / 1000000000U) + 1;
	}

	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);

}


/*****************************************************
	* @fn  		- I2C_DeInit
	*
	* @brief   	- Function to De-initialize given SPI
	*
	* @param  	- base address of I2C peripheral
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	}else if(pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	}else if(pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}



//HELPER FUNCTION TO GET THE FLAG STATUS
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName) //flag name -> flag bit name in sr1 reg
{																	//flag name : eg: I2C_SR_TXE
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}
/*****************************************************
	* @fn  		- I2C_MasterSendData
	*
	* @brief   	- Function to Send data
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	//1. Generate start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2.Confirm start generation is complete by checking the SB bit in sr1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)); //not set - true
							//SB flag will be cleared in address phase (writing to DR)

	//3.Send the address of the slave with r/w bit set to w(0)
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm the address phase is completed by checking the ADDR flag in SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));
							//ADDR flag will be cleared by reading sr1 reg and sr2 reg
	//5.clearing the ADDR flag
	I2C_ClearADDRFlag(pI2CHandle);

	//6.Send the data until Len become 0
	while(Len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)); //wait till txe is set and break
		pI2CHandle->pI2Cx->DR = *pTxbuffer;
		pTxbuffer++;
		Len--;
	}

	//7.When Len become 0, wait for TXE=1 and BTF=1 before generating stop condition
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	//8.Generate stop condition , stop condition clears BTF
	if(Sr == I2C_DISABLE_SR )
	{
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}

}


/*****************************************************
	* @fn  		- I2C_MasterReceiveData
	*
	* @brief   	- Function to Receive data
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	//1. Generate start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2.Confirm start generation is complete by checking the SB bit in sr1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)); //not set - true
							//SB flag will be cleared in address phase (writing to DR)

	//3.Send the address of the slave with r/w bit set to w(0)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm the address phase is completed by checking the ADDR flag in SR1
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));
							//ADDR flag will be cleared inn the if block below.

/////////////////////same as send data till here///////////////////

	//5. Procedure to Read 1 byte from the slave
	if(Len == 1)
	{
		//disable the ACKING
		I2C_ManageAcking(pI2CHandle->pI2Cx , DISABLE);

		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//Generate stop condition
		if(Sr == I2C_DISABLE_SR )
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}


		//wait until RXNE is set
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		//read data into buffer
		*pTxbuffer = pI2CHandle->pI2Cx->DR;

		return;
	}

	//6. Procedure to Read 1 byte from the slave
	if(Len > 1)
	{
		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		////read the data until Len becomes zero
		for(uint32_t i = Len ; i > 0 ; i--)
		{
			//wait until RXNE is set
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

			if(i == 2) //if last 2 bytes are remaining / what to do if Len == 2
			{
				//clear the ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx , DISABLE);

				//generate stop condition
				if(Sr == I2C_DISABLE_SR )
				{
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				}

			}
			//read the data from data register
			*pTxbuffer = pI2CHandle->pI2Cx->DR;

			//increment the buffer address
			pTxbuffer++;

		}

	}
	//re-enable acking
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx , ENABLE);
		//we disable acking while using this api, so we enable it so others send data can use it
	}

}


/*****************************************************
	* @fn  		- I2C_MasterSendDataIT
	*
	* @brief   	-
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxbuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVTEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);

	}

	return busystate;
}

/*****************************************************
	* @fn  		- I2C_MasterReceiveDataIT
	*
	* @brief   	-
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxbuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;
}



void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == I2C_ACK_ENABLE)
	{
		//enable the ack
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}else
	{
		//disable the ack
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}



/*********************************************************************************************************/
/**
	* @fn  		- I2C_PeripheralControl
	*
	* @brief   	- Function to enable and disable I2C peripheral using PE bit of I2C_CR1 register
	*
	* @param  	-
	*
	* @return	- none
	*
	* @note 	- we use RCC reset register
	*/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx , uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}


						//IRQ Configuration and ISR Handling - START

/*************************************
* @fn  		- I2C_IRQInterruptConfig
*
* @brief   	- Function to configure IRQs
*
* @param  	- base address of I2C peripheral
* @param  	- IRQ priority value
* @param    - ENABLE or DISABLE macros
*
* @return	- none
*
* @note 	- none
*/


void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
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
	* @fn  		- I2C_IRQPriorityConfig
	*
	* @brief   	- Function to configure IRQs
	* @param  	- IRQ priority value
	* @return	- none
	*
	* @note 	- none
	*/
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority)
{
	//1. find the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);//location of where we shift
	*(NVIC_PR_BASE_ADDR + iprx) |=  ( IRQPriority << shift_amount );

	//REMEMBER : lower 4 bits of each 8bit section of an IPR reg is not implemented
}


void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//disable ITEBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}


void I2C_CloseReceieveData(I2C_Handle_t *pI2CHandle)
{
	//disable ITEBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
	}
}

//private helper function
static void I2cMasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	 if(pI2CHandle->TxLen > 0)
	 {
		 //1.load data to DR
		 pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		 //2. decrement TxLen
		 pI2CHandle->TxLen--;

		 //3.increment the buffer address
		 pI2CHandle->pTxBuffer++;
	 }
}


//private helper function
static void I2CMasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	//we have to do data reception
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			//clear the ACk bit
			I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
		}

		//read DR
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0)
	{
		//close the reception and notify the app
		//1. generate the stop condition
		if(pI2CHandle->Sr == I2C_DISABLE_SR)
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//2.close I2C Rx
		I2C_CloseReceieveData(pI2CHandle);
		//3.notify the application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
	}
}


/*****************************************************
	* @fn  		- I2C_SlaveSendData
	*
	* @brief   	- Function to Receive data for slave
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx , uint8_t data)
{
	pI2Cx->DR = data;
}


/*****************************************************
	* @fn  		- I2C_SlaveReceiveData
	*
	* @brief   	- Function to Receive data for slave
	*
	* @param  	-
	*
	* @return	-	none
	*
	* @note 	-   none
	*/
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t) pI2Cx->DR;
}



void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//interrupt handling for both master and slave device
	/****EV interrupt can happen due to all the things in sr1 reg****/

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	//1.Handle for interrupt[t generated by SB event
	//NB:SB flag is only applicable in master mode, cause for slave SB = 0 always
	if(temp1 && temp3)
	{
		//Interrupt is generated because of SB event
		//when control reaches here means start condition was successful
		//so here we execute next step ie. address phase

		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}


	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	//2.Handle for interrupt[t generated by ADDR event
	if(temp1 && temp3)
	{
		//Interrupt is generated because of ADDR event
		//we clear the flag, thats it
		I2C_ClearADDRFlag(pI2CHandle);
	}


	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	//3.Handle for interrupt generated by BTF event
	if(temp1 && temp3)
	{
		//Interrupt is generated because of BTF event -**we use it to end i2c transmission**
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX) //same logic as blocking api
		{
			//make sure txe is set -> is DR empty?
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TxE))
			{
				//BTF, TXE  = 1 , both set
				if(pI2CHandle->TxLen == 0)
				{
					//close transmission
					//1.generate stop condition
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					//2.reset all elements of handle structure
					I2C_CloseSendData(pI2CHandle);

					//3.notify application
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}

		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;
		}
	}


	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	//4.Handle for interrupt[t generated by STOPF event
	//only applicable for slave mode, when slave detects stop condition
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//we have to clear it - read SR1 & write to CR1
		//read SR! already done above with temp3
		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		//notify the application that stop is detected
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);

	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TxE);
	//5.Handle for interrupt[t generated by TXE event
	if(temp1 && temp2 && temp3)  //---- for txe both ITEVTEN & ITBUFEN have to be set with TXE flag
	{
		//check if master or slave, only do if its master
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//TXE flag is set
			//we have to do data transmission

			 if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			 {
				 I2cMasterHandleTXEInterrupt(pI2CHandle);
			 }else
			 {
				 //slave
				 //make sure slave is really in transmitter mode
				 if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)) //TRA influenced by R/W bit in addr phase
				 {
					 I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);//why are we not using i2c_slavesenddata here
				 }
			 }
		}
	}


	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RxNE);
	//6.Handle for interrupt[t generated by RXNE event
	if(temp1 && temp2 && temp3)   //---- for RXNE both ITEVTEN & ITBUFEN have to be set with RNNE flag
	{
		//check if master or slave
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{

			//RXNE flag is set
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2CMasterHandleRXNEInterrupt(pI2CHandle);
			}
		}else
		{
			//slave
			//make sure slave is really in transmitter mode
			 if(!(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))) //TRA influenced by R/W bit in addr phase
			 {
				 I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);//why are we not using i2c_slavereceivedata here
			 }
		}

	}
}



void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);

	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}
						//IRQ Configuration and ISR Handling - END


