
#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stdint.h>
#define __vol						volatile

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


/*********************PERIPHERAL REGISTER DEFINITION STRUCTURES*********************/

typedef struct
{
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

typedef struct
{
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

/*
 *peripheral definitions
 */

#define GPIOA					(GPIO_RegDef_t*)GPIOA_BASEADDR
#define GPIOB					(GPIO_RegDef_t*)GPIOB_BASEADDR
#define GPIOC					(GPIO_RegDef_t*)GPIOC_BASEADDR
#define GPIOD					(GPIO_RegDef_t*)GPIOD_BASEADDR
#define GPIOE					(GPIO_RegDef_t*)GPIOE_BASEADDR
#define GPIOF					(GPIO_RegDef_t*)GPIOF_BASEADDR
#define GPIOG					(GPIO_RegDef_t*)GPIOG_BASEADDR
#define GPIOH					(GPIO_RegDef_t*)GPIOH_BASEADDR

#define RCC						(RCC_RegDef_t*)RCC_BASEADDR


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
#define SPI1_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 12) )
#define SPI4_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 13) )
#define SPI2_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 14) )
#define SPI3_PCLK_DI()		( RCC->APB1ENR &= ~(1 << 15) )

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
#endif /* INC_STM32F446XX_H_ */
