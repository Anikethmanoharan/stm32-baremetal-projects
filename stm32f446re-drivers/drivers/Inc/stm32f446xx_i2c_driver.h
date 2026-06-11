#ifndef INC_STM32F446XX_I2C_DRIVER_H_
#define INC_STM32F446XX_I2C_DRIVER_H_

#include "stm32f446xx.h"


/*
 * Configuration struct for I2C peripheral  - user gives/sets these data before init
 */
typedef struct{

	uint32_t I2C_SCLSpeed;
	uint8_t I2C_DeviceAddress; //7bit address
	uint8_t I2C_ACKControl;     //ACK is disabled by default
	uint16_t I2C_FMDutyCycle;

}I2C_Config_t;

/*
 * Handle struct for I2C
 */
typedef struct{

	I2C_RegDef_t *pI2Cx;
	I2C_Config_t I2C_Config;

	uint8_t *pTxBuffer; //to store app. txbuffer address
	uint8_t *pRxBuffer; //to store app. rxbuffer address
	uint32_t TxLen;		//to store txlen
	uint32_t RxLen;		//to store rxlen
	uint8_t TxRxState;	//to store communication state @TxRxState halfduplex - one enough for both tx & rx
	uint8_t DevAddr;	//to store slave/device address
	uint32_t RxSize;	//to store rxsize
	uint8_t Sr;			//to store repeated start value

}I2C_Handle_t;

/*
 * I2C APPLICATION STATES @TxRxState
 */
#define I2C_READY		0
#define I2C_BUSY_IN_RX	1
#define I2C_BUSY_IN_TX	2

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM	100000 //standard mode
#define I2C_SCL_SPEED_FM4K	400000 //fast mode
#define I2C_SCL_SPEED_FM2K	200000 //fast mode


/*
 * @I2C_ACKControl
 */
#define I2C_ACK_ENABLE	1
#define I2C_ACK_DISABLE	0

/*
 * @I2C_FMDutyCycle
 */
#define I2C_FM_DUTY_2	0
#define I2C_FM_DUTY_16_9	1



/*
 * I2C RELATED STATUS FLAGS DEFINITIONS
 */
#define I2C_FLAG_TXE		(1 << I2C_SR1_TxE)
#define I2C_FLAG_RXNE		(1 << I2C_SR1_RxNE)
#define I2C_FLAG_SB			(1 << I2C_SR1_SB)
#define I2C_FLAG_ADDR		(1 << I2C_SR1_ADDR)
#define I2C_FLAG_BTF		(1 << I2C_SR1_BTF)
#define I2C_FLAG_STOPF		(1 << I2C_SR1_STOPF)
#define I2C_FLAG_BERR		(1 << I2C_SR1_BERR)
#define I2C_FLAG_ARLO		(1 << I2C_SR1_ARLO)
#define I2C_FLAG_AF			(1 << I2C_SR1_AF)
#define I2C_FLAG_OVR		(1 << I2C_SR1_OVR)
#define I2C_FLAG_TIMEOUT	(1 << I2C_SR1_TIMEOUT)

#define I2C_DISABLE_SR  	RESET
#define I2C_ENABLE_SR   	SET

/*
 * I2C APPLICATION EVENT MACROS - used in EV_IRQHANDLING
 */
#define I2C_EV_TX_CMPLT		0
#define I2C_EV_STOP			1
#define I2C_EV_RX_CMPLT		2

#define I2C_ERROR_BERR  	3
#define I2C_ERROR_ARLO  	4
#define I2C_ERROR_AF    	5
#define I2C_ERROR_OVR   	6
#define I2C_ERROR_TIMEOUT 	7

#define I2C_EV_DATA_REQ		8
#define I2C_EV_DATA_RCV		9

/*************************************************************************************************************
 * 								APIs supported by this driver
 **************************************************************************************************************/

/*
 * peripheral clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx , uint8_t EnorDi);


/*
 * init and deinit
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


/*
 * Data send and receive - BLOCKING
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);

/*
 * Data send and receive using interrupts - NON-BLOCKING
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr);

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceieveData(I2C_Handle_t *pI2CHandle);


void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx , uint8_t data); //we are sending byte by byte so no Len variable needed.
	//why is there a return type
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx);

/*
 * IRQ Configuration and ISR Handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

/*
 * Other peripheral control apis
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx , uint8_t EnorDi);

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);


//Application callback   -> this is implemented by the application
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle , uint8_t AppEvent);
#endif /* INC_STM32F446XX_I2C_DRIVER_H_ */
