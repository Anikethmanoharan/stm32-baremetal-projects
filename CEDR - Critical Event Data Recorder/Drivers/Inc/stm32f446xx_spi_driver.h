
#ifndef INC_STM32F446XX_SPI_DRIVER_H_
#define INC_STM32F446XX_SPI_DRIVER_H_

#include "stm32f446xx.h"
/*
 * Configuration structure for SPIx peripheral
 * */
typedef struct
{
	uint8_t SPI_DeviceMode; //Master or Slave
	uint8_t SPI_BusConfig;  //Which duplex
	uint8_t SPI_DFF;        //8bit or 16bit
	uint8_t SPI_CPHA;
	uint8_t SPI_CPOL;
	uint8_t SPI_SSM;		//Slave Select Management - Software or Hardware
	uint8_t SPI_SclkSpeed;      //8Mhz or 16Mhz

}SPI_Config_t;
// Config uses uint8_t (small values like 0/1 or 0–7, saves memory)


/*
 * Handle structure for SPIx peripheral
 * */
typedef struct
{
	SPI_RegDef_t *pSPIx;	//which spi peripheral to use. holds base address of SPIx -> SPI1/SPI2/SPI3/SPI4
	SPI_Config_t SPIConfig; //configuration items of spi to set it and run
	uint8_t		*pTxBuffer;//from here it is for api using interrupts
	uint8_t		*pRxBuffer;
	uint32_t	TxLen;
	uint32_t	RxLen;
	uint8_t		TxState;
	uint8_t		RxState;

}SPI_Handle_t;

/*
 * SPI application states
 */
#define SPI_READY		0
#define SPI_BUSY_IN_RX	1
#define SPI_BUSY_IN_TX	2

/*
 * possible spi application events
 */
#define SPI_EVENT_TX_CMPLT	1
#define SPI_EVENT_RX_CMPLT	2
#define SPI_EVENT_OVR_ERR	3
#define SPI_EVENT_CRC_ERR	4 //not implemented

//THIS IS FOR SPI_init() to fill out the @SPI_Config_t struct above , check SPI_CR1 reg for info
/*
 * @SPI_DeviceMode
 */
#define SPI_DEVICE_MODE_MASTER		1
#define SPI_DEVICE_MODE_SLAVE		0
/*
 * @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD					1
#define SPI_BUS_CONFIG_HD					2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY		3
/*
 * @SPI_SclkSpeed
 */
#define	SPI_SCLK_SPEED_DIV2			0
#define	SPI_SCLK_SPEED_DIV4			1
#define	SPI_SCLK_SPEED_DIV8			2
#define	SPI_SCLK_SPEED_DIV16		3
#define	SPI_SCLK_SPEED_DIV32		4
#define	SPI_SCLK_SPEED_DIV64		5
#define	SPI_SCLK_SPEED_DIV128		6
#define	SPI_SCLK_SPEED_DIV256		7
/*
 * @SPI_DFF
 */
#define SPI_DFF_8BITS		0 //default
#define SPI_DFF_16BITS		1
/*
 * @SPI_CPOL
 */
#define SPI_CPOL_HIGH		1
#define SPI_CPOL_LOW		0
/*
 * @SPI_CPHA
 */
#define SPI_CPHA_HIGH		1
#define SPI_CPHA_LOW		0
/*
 * @SPI_SSM , default -> SSM DISABLED
 */
#define SPI_SSM_EN		1
#define SPI_SSM_DI		0


/*
 * SPI RELATED STATUS FLAGS
 */
#define SPI_TXE_FLAG	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG	(1 << SPI_SR_BSY)


/*************************************************************************************************************
 * 								APIs supported by this driver
 **************************************************************************************************************/

/*
 * peripheral clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx , uint8_t EnorDi);


/*
 * init and deinit
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);


/*
 * Data send and receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx , uint8_t *pTxBuffer , uint32_t Len);
void SPI_ReceieveData(SPI_RegDef_t *pSPIx ,  uint8_t *pRxBuffer , uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle , uint8_t *pTxBuffer , uint32_t Len);
uint8_t SPI_ReceieveDataIT(SPI_Handle_t *pSPIHandle ,  uint8_t *pRxBuffer , uint32_t Len);

/*
 * IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 * Other peripheral control apis
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx , uint8_t EnorDi);

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName);

void SPI_SSIConfig(SPI_RegDef_t *pSPIx , uint8_t EnorDi);

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx , uint8_t EnorDi);

/*
 * Helper functions
 */
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

//Application callback   -> this is implemented by the application
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle , uint8_t AppEvent);

#endif /* INC_STM32F446XX_SPI_DRIVER_H_ */
