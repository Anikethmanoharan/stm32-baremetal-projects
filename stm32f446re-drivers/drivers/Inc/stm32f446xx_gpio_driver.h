
#ifndef INC_STM32F446XX_GPIO_DRIVER_H_
#define INC_STM32F446XX_GPIO_DRIVER_H_

#include "stm32f446xx.h" //board specific header file



//This is a Configuration structure for gpio pin
typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;             /*< POSSIBLE VALUES FROM @GPIO_PIN_MODES >*/
	uint8_t GPIO_PinSpeed;				/*< POSSIBLE VALUES FROM @GPIO_PIN_SPEED >*/
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;

}GPIO_PinConfig_t;




//This is a Handle structure for gpio pin
typedef struct
{
	//pointer to hold the base address of gpio peripheral
	GPIO_RegDef_t *pGPIOx; 				/*<This holds the base addr of the gpio port which the pin belongs>*/
	GPIO_PinConfig_t GPIO_PinConfig;	/*<This holds GPIO pin configuration setting>*/

}GPIO_Handle_t;

//GPIO PIN NUMBERS
#define GPIO_PIN_NO_0		0
#define GPIO_PIN_NO_1		1
#define GPIO_PIN_NO_2		2
#define GPIO_PIN_NO_3		3
#define GPIO_PIN_NO_4		4
#define GPIO_PIN_NO_5		5
#define GPIO_PIN_NO_6		6
#define GPIO_PIN_NO_7		7
#define GPIO_PIN_NO_8		8
#define GPIO_PIN_NO_9		9
#define GPIO_PIN_NO_10		10
#define GPIO_PIN_NO_11		11
#define GPIO_PIN_NO_12		12
#define GPIO_PIN_NO_13		13
#define GPIO_PIN_NO_14		14
#define GPIO_PIN_NO_15		15


//GPIO possible pin modes  [USED FOR MODE REG] @GPIO_PIN_MODES

#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG	3

#define GPIO_MODE_IT_FT		4 // interupt falling edge [we defined this value]
#define GPIO_MODE_IT_RT		5 // interupt rising edge [we defined this value]
#define GPIO_MODE_IT_RFT	6 //rising edge falling edge trigger


//GPIO pin possible output types  [USED FOR OUTPUT TYPE REG]
#define GPIO_OP_TYPE_PP		0 //PUSHPULL
#define GPIO_OP_TYPE_OD		1 //OPENDRAIN

//GPIO pin possible output speeds [USED FOR OUTPUT SPEED REG] @GPIO_PIN_SPEED
#define GPIO_SPEED_LOW		0
#define GPIO_SPEED_MEDIUM	1
#define GPIO_SPEED_FAST		2
#define GPIO_SPEED_HIGH	    3

//GPIO pin pullup , pulldown configuration macros   [USED FOR PULLUP PULLDOWN REG]
#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU				1
#define GPIO_PIN_PD				2


/*************************************************************************************************************
 * 								APIs supported by this driver
 **************************************************************************************************************/

//peripheral clock setup
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx , uint8_t EnorDi);

//Init & De-Init
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx); //only need base addr as we have a reg in rcc to reset all the bits

//Data Read & Write
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber);
uint16_t  GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIO);

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx , uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber);

//IRQ Configuration and ISR Handling
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi);
void GPIO_IRQHandling(uint8_t PinNumber);

#endif /* INC_STM32F446XX_GPIO_DRIVER_H_ */
