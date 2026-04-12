/*BOARD SPECIFIC HEADER FILE*/
#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stdint.h>

#define __vol						volatile

/*********************START:PROCESSOR SPECIFIC DETAILS*********************************/

//ARM Cortex mx ISERx register addressses

#define NVIC_ISER0					( (__vol uint32_t*)0xE000E100 )//we took ISER 0 - 3 because it covers majority of interupts
#define NVIC_ISER1					( (__vol uint32_t*)0xE000E104 )
#define NVIC_ISER2					( (__vol uint32_t*)0xE000E108 )
#define NVIC_ISER3					( (__vol uint32_t*)0xE000E10C )

//ARM Cortex mx ICERx register addressses

#define NVIC_ICER0					( (__vol uint32_t*)0XE000E180 )
#define NVIC_ICER1					( (__vol uint32_t*)0XE000E184 )
#define NVIC_ICER2					( (__vol uint32_t*)0XE000E188 )
#define NVIC_ICER3					( (__vol uint32_t*)0XE000E18C )

//ARM Cortex mx interrupt priority register addressses

#define NVIC_PR_BASE_ADDR			( (__vol uint32_t*)0xE000E400 )  //-> IPR

#define NO_PR_BITS_IMPLEMENTED		4 //in some section of reg 4 bits of 8bit is implemented, eg: IPR register

/*********************END:PROCESSOR SPECIFIC DETAILS*********************************/

/* base addr of flash and sram memory*/
#define FLASH_BASEADDR				0x08000000U
#define SRAM1_BASEADDR				0x20000000U //112KB
#define SRAM2_BASEADDR				0x20001C00U //1024 * 112 = 1C00
#define ROM_BASEADDR				0x1FFF0000	//sys memory(flash)
#define SRAM 						SRAM1_BASEADDR

/*AHBx and APBx bus peripheral addresses*/
#define PERIPH_BASE					0x40000000U
#define APB1PERIPH_BASEADDR			PERIPH_BASE
#define APB2PERIPH_BASEADDR			0x40010000U
#define AHB1PERIPH_BASEADDR			0x40020000U
#define AHB2PERIPH_BASEADDR			0x50000000U

/*
 * Base addresses of peripherals which are hanging on AHB1 bus
 * */
#define GPIOA_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR				(AHB1PERIPH_BASEADDR + 0X0C00)
#define GPIOE_BASEADDR				(AHB1PERIPH_BASEADDR + 0X1000)
#define GPIOF_BASEADDR				(AHB1PERIPH_BASEADDR + 0X1400)
#define GPIOG_BASEADDR				(AHB1PERIPH_BASEADDR + 0X1800)
#define GPIOH_BASEADDR				(AHB1PERIPH_BASEADDR + 0X1C00)

#define RCC_BASEADDR				(AHB1PERIPH_BASEADDR + 0X3800)

/*
 * Base addresses of peripherals which are hanging on APB1 bus
 * */
#define I2C1_BASEADDR				(APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR				(APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR				(APB1PERIPH_BASEADDR + 0x5C00)

#define SPI2_BASEADDR				(APB1PERIPH_BASEADDR + 0x3800)
#define SPI3_BASEADDR				(APB1PERIPH_BASEADDR + 0x3C00)

#define USART2_BASEADDR				(APB1PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR				(APB1PERIPH_BASEADDR + 0x4800)
#define UART4_BASEADDR				(APB1PERIPH_BASEADDR + 0x4C00)
#define UART5_BASEADDR				(APB1PERIPH_BASEADDR + 0x5000)

/*
 * Base addresses of peripherals which are hanging on APB2 bus
 * */
#define USART1_BASEADDR				(APB2PERIPH_BASEADDR + 0x1000)
#define USART6_BASEADDR				(APB2PERIPH_BASEADDR + 0x1400)

#define SPI1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3000)
#define SPI4_BASEADDR				(APB2PERIPH_BASEADDR + 0x3400)

#define EXT1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3C00)

#define SYSCFG_BASEADDR				(APB2PERIPH_BASEADDR + 0x3800)


/********************* PERIPHERAL REGISTER DEFINITION STRUCTURES *********************/

//peripheral register definiiton structure for GPIO
typedef struct
{				//this struct contains GPIO port registers
	__vol uint32_t MODER;					//GPIO port mode register
	__vol uint32_t OTYPER;					//GPIO port output type register
	__vol uint32_t OSPEEDER;				//GPIO port output speed register
	__vol uint32_t PUPDR;					//GPIO port pull-up/pull-down register
	__vol uint32_t IDR;						//GPIO port input data register
	__vol uint32_t ODR;						//GPIO port output data register
	__vol uint32_t BSRR;					//GPIO port bit set/reset register
	__vol uint32_t LCKR;					//GPIO port configuration lock register
	__vol uint32_t AFR[2];					//AFR[0] : GPIO alternate function low register , AFR[1] : GPIO alternate function high register
}GPIO_RegDef_t;

//peripheral register definiiton structure for RCC
typedef struct
{					                    //this struct contains clock (RCC) registers
	__vol uint32_t CR;
	__vol uint32_t PLL;
	__vol uint32_t CFGR;
	__vol uint32_t CIR;
	__vol uint32_t AHB1RSTR;
	__vol uint32_t AHB2RSTR;
	__vol uint32_t AHB3RSTR;
	__vol uint32_t RESERVED0;
	__vol uint32_t APB1RSTR;
	__vol uint32_t APB2RSTR;
	__vol uint32_t RESERVED1[2];
	__vol uint32_t AHB1ENR;
	__vol uint32_t AHB2ENR;
	__vol uint32_t AHB3ENR;
	__vol uint32_t RESERVED2;
	__vol uint32_t APB1ENR;
	__vol uint32_t APB2ENR;
	__vol uint32_t RESERVED3[2];
	__vol uint32_t AHB1LPENR;
	__vol uint32_t AHB2LPENR;
	__vol uint32_t AHB3LPENR;
	__vol uint32_t RESERVED4;
	__vol uint32_t APB1LPENR;
	__vol uint32_t APB2LPENR;
	__vol uint32_t RESERVED5[2];
	__vol uint32_t BDCR;
	__vol uint32_t CSR;
	__vol uint32_t RESERVED6[2];
	__vol uint32_t SSCGR;
	__vol uint32_t PLLI2SCFGR;
	__vol uint32_t PLLSAICFGR;
	__vol uint32_t DCKCFGR;
	__vol uint32_t CKGATENR;
	__vol uint32_t DCKCFGR2;

}RCC_RegDef_t;


//peripheral register definiiton structure for EXTI

typedef struct
{		//this struct contains EXTI registers
	__vol uint32_t IMR;						//Interrupt mask register
	__vol uint32_t EMR;						//Event mask register
	__vol uint32_t RTSR;					//Rising trigger selection register
	__vol uint32_t FTSR;					//Falling trigger selection register
	__vol uint32_t SWIER;					//Software interrupt event register
	__vol uint32_t PR;						//Pending register

}EXTI_RegDef_t;


//peripheral register definiiton structure for SYSCFG_EXTICR

typedef struct
{//this struct contains SYSCFG_EXTICR registers
	__vol uint32_t MEMRMP;					//SYSCFG memory remap register
	__vol uint32_t PMC;						//SYSCFG peripheral mode configuration register
	__vol uint32_t EXTICR[4];					//SYSCFG external interrupt configuration register 1
	uint32_t RESERVED1[2];
	__vol uint32_t CMPCR;					//Compensation cell control register
	uint32_t RESERVED2[2];
	__vol uint32_t CFGR;					//SYSCFG configuration register

}SYSCFG_RegDef_t;

//peripheral register definiiton structure for SPI
typedef struct
{
	__vol uint32_t CR1;			//SPI control register 1
	__vol uint32_t CR2; 		//SPI control register 2
	__vol uint32_t SR;			//SPI status register
	__vol uint32_t DR;			//SPI data register
	__vol uint32_t CRCPR;		//SPI CRC polynomial register
	__vol uint32_t RXCRCR;		//SPI RX CRC register
	__vol uint32_t TXCRCR;		//SPI TX CRC register
	__vol uint32_t I2SCFGR;		//SPI_I2S configuration register
	__vol uint32_t I2SPR;		//SPI_I2S prescaler register

}SPI_RegDef_t;

/*
 *peripheral definitions             -- using macro will not use ram and it is direct execution
 */

#define GPIOA					(GPIO_RegDef_t*)GPIOA_BASEADDR
#define GPIOB					(GPIO_RegDef_t*)GPIOB_BASEADDR
#define GPIOC					(GPIO_RegDef_t*)GPIOC_BASEADDR
#define GPIOD					(GPIO_RegDef_t*)GPIOD_BASEADDR
#define GPIOE					(GPIO_RegDef_t*)GPIOE_BASEADDR
#define GPIOF					(GPIO_RegDef_t*)GPIOF_BASEADDR
#define GPIOG					(GPIO_RegDef_t*)GPIOG_BASEADDR
#define GPIOH					(GPIO_RegDef_t*)GPIOH_BASEADDR

#define RCC						((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI					((EXTI_RegDef_t*)EXT1_BASEADDR)

#define SYSCFG					((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define SPI1					((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2					((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3					((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4					((SPI_RegDef_t*)SPI4_BASEADDR)


/*
 * CLOCK ENABLE MACRO FOR GPIOx PERIPHERALS
 * */
#define GPIOA_PCLK_EN()		( RCC->AHB1ENR |= (1 << 0) )  //AHB1 BECAUSE GPIO HANGS IN AHB1 BUS & OTH BIT POSITION IS FOR GPIOA
#define GPIOB_PCLK_EN()		( RCC->AHB1ENR |= (1 << 1) )
#define GPIOC_PCLK_EN()		( RCC->AHB1ENR |= (1 << 2) )
#define GPIOD_PCLK_EN()		( RCC->AHB1ENR |= (1 << 3) )
#define GPIOE_PCLK_EN()		( RCC->AHB1ENR |= (1 << 4) )
#define GPIOF_PCLK_EN()		( RCC->AHB1ENR |= (1 << 5) )
#define GPIOG_PCLK_EN()		( RCC->AHB1ENR |= (1 << 6) )
#define GPIOH_PCLK_EN()		( RCC->AHB1ENR |= (1 << 7) )


/*
 * CLOCK ENABLE MACRO FOR I2Cx PERIPHERALS
 * */
#define I2C1_PCLK_EN()		( RCC->APB1ENR |= (1 << 21) ) //I2C1 HANGS ON APB1 AND 12CEN IS 21ST BIT ON APB1ENR REG
#define I2C2_PCLK_EN()		( RCC->APB1ENR |= (1 << 22) )
#define I2C3_PCLK_EN()		( RCC->APB1ENR |= (1 << 23) )


/*
 * CLOCK ENABLE MACRO FOR SPIx PERIPHERALS
 * */
#define SPI1_PCLK_EN()		( RCC->APB2ENR |= (1 << 12) ) //SPI1 & SPI4 HANGS ON APB2
#define SPI4_PCLK_EN()		( RCC->APB2ENR |= (1 << 13) ) //SPI2 & SPI3 HANGS ON APB1
#define SPI2_PCLK_EN()		( RCC->APB1ENR |= (1 << 14) )
#define SPI3_PCLK_EN()		( RCC->APB1ENR |= (1 << 15) )


/*
 * CLOCK ENABLE MACRO FOR USARTx PERIPHERALS
 * */
#define USART1_PCLK_EN()		( RCC->APB2ENR |= (1 << 4) )
#define USART6_PCLK_EN()		( RCC->APB2ENR |= (1 << 5) )
#define USART2_PCLK_EN()		( RCC->APB1ENR |= (1 << 17) )
#define USART3_PCLK_EN()		( RCC->APB1ENR |= (1 << 18) )
#define UART4_PCLK_EN()			( RCC->APB1ENR |= (1 << 19) )
#define UART5_PCLK_EN()			( RCC->APB1ENR |= (1 << 20) )


/*
 * CLOCK ENABLE MACRO FOR SYSCFG PERIPHERALS
 * */
#define SYSCFG_PCLK_EN()		( RCC->APB2ENR |= (1 << 14) )

/******************************************************************************************************/

/*
 * CLOCK DISABLE MACRO FOR GPIOx PERIPHERALS
 * */
#define GPIOA_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 0) )
#define GPIOB_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 1) )
#define GPIOC_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 2) )
#define GPIOD_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 3) )
#define GPIOE_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 4) )
#define GPIOF_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 5) )
#define GPIOG_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 6) )
#define GPIOH_PCLK_DI()		( RCC->AHB1ENR &= ~(1 << 7) )

/*
 * CLOCK DISABLE MACRO FOR I2Cx PERIPHERALS
 * */
#define I2C1_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 21) ) //I2C1 HANGS ON APB1 AND 12CEN IS 21ST BIT ON APB1ENR REG
#define I2C2_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 22) )
#define I2C3_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 23) )

/*
 * CLOCK DISABLE MACRO FOR SPIx PERIPHERALS
 * */
#define SPI1_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 12) )
#define SPI4_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 13) )
#define SPI2_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 14) )
#define SPI3_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 15) )

/*
 * CLOCK DISABLE MACRO FOR USARTx PERIPHERALS
 * */
#define USART1_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 4) )
#define USART6_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 5) )
#define USART2_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 17) )
#define USART3_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 18) )
#define UART4_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 19) )
#define UART5_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 20) )

/*
 * CLOCK DISABLE MACRO FOR SYSCFG PERIPHERALS
 * */
#define SYSCFG_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 14) )


/*
 * MACROS TO RESET GPIOx PERIPHERALS
 */
#define GPIOA_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 0)) ; (RCC->AHB1RSTR &= ~(1 << 0));}while(0)
#define GPIOB_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 1)) ; (RCC->AHB1RSTR &= ~(1 << 1));}while(0)
#define GPIOC_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 2)) ; (RCC->AHB1RSTR &= ~(1 << 2));}while(0)
#define GPIOD_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 3)) ; (RCC->AHB1RSTR &= ~(1 << 3));}while(0)
#define GPIOE_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 4)) ; (RCC->AHB1RSTR &= ~(1 << 4));}while(0)
#define GPIOF_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 5)) ; (RCC->AHB1RSTR &= ~(1 << 5));}while(0)
#define GPIOG_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 6)) ; (RCC->AHB1RSTR &= ~(1 << 6));}while(0)
#define GPIOH_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 7)) ; (RCC->AHB1RSTR &= ~(1 << 7));}while(0)


/*
 * MACROS TO RESET SPIx PERIPHERALS
 */
#define SPI1_REG_RESET()		do{(RCC->APB2RSTR |= (1 << 12)) ; (RCC->APB2RSTR &= ~(1 << 12));}while(0)
#define SPI2_REG_RESET()		do{(RCC->APB1RSTR |= (1 << 14)) ; (RCC->APB1RSTR &= ~(1 << 14));}while(0)
#define SPI3_REG_RESET()		do{(RCC->APB1RSTR |= (1 << 15)) ; (RCC->APB1RSTR &= ~(1 << 15));}while(0)
#define SPI4_REG_RESET()		do{(RCC->APB2RSTR |= (1 << 13)) ; (RCC->APB2RSTR &= ~(1 << 13));}while(0)

//return portcode for given GPIOx base addr	//c conditional / ternary operation

#define GPIO_BASEADDR_TO_CODE(x)	   ( (x == GPIOA) ? 0 :\
										 (x == GPIOB) ? 1 :\
										 (x == GPIOC) ? 2 :\
										 (x == GPIOD) ? 3 :\
                                         (x == GPIOE) ? 4 :\
                                         (x == GPIOF) ? 5 :\
                                         (x == GPIOG) ? 6 :\
                                         (x == GPIOH) ? 7 :0)


/*
 * IRQ (interrupt request)  numbers of stm32f446xx mcu for EXTI --> reference manual vector table
 * */
//for @GPIO_IRQConfig in stm32f446xx_gpio_driver.c

#define IRQ_NO_EXTI0				6
#define IRQ_NO_EXTI1				7
#define IRQ_NO_EXTI2				8
#define IRQ_NO_EXTI3				9
#define IRQ_NO_EXTI4				10
#define IRQ_NO_EXTI9_5				23
#define IRQ_NO_EXTI15_10			40

//SOME GENERIC MACROS to be used by all drivers
#define ENABLE 				1
#define DISABLE				0
#define SET					ENABLE
#define RESET				DISABLE
#define GPIO_PIN_SET 		SET
#define GPIO_PIN_RESET 		RESET

#define FLAG_RESET			RESET
#define FLAG_SET			SET



/****************************************************
 *BIT POSITION DEFINITION FOR SPI PERIPHERAL REGISTERS
 ****************************************************/
          //SPI_CRC1 reg
#define SPI_CR1_CPHA		0
#define SPI_CR1_CPOL		1
#define SPI_CR1_MSTR		2
#define SPI_CR1_BR			3
#define SPI_CR1_SPE			6
#define SPI_CR1_LSBFIRST	7
#define SPI_CR1_SSI			8
#define SPI_CR1_SSM			9
#define SPI_CR1_RXONLY		10
#define SPI_CR1_DFF			11
#define SPI_CR1_CRCNEXT		12
#define SPI_CR1_CRCEN		13
#define SPI_CR1_BIDIOE		14
#define SPI_CR1_BIDIMODE	15

          //SPI_CRC2 reg
#define SPI_CR2_RXDMAEN		0
#define SPI_CR2_TXDMAEN		1
#define SPI_CR2_SSOE		2
#define SPI_CR2_FRF			4
#define SPI_CR2_ERRIE		5
#define SPI_CR2_RXNEIE		6
#define SPI_CR2_TXEIE		7

          //SPI_SR reg
#define SPI_SR_RXNE			0
#define SPI_SR_TXE			1
#define SPI_SR_CHSIDE		2
#define SPI_SR_UDR			3
#define SPI_SR_CRCERR		4
#define SPI_SR_MODF			5
#define SPI_SR_OVR			6
#define SPI_SR_BSY			7
#define SPI_SR_FRE			8



#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"
#endif /* INC_STM32F446XX_H_ */
