/*
 * SPI2_MOSI -> PB15      ----> from DATASHEET STM32F446xC/E
 * SPI2_MISO -> PB14
 * SPI2_SCK  -> PB13
 * SPI2_NSS  -> PB12
 * ALT FUNCTION MODE : 5
 */

#include <string.h> 
#include "stm32f446xx.h"

void SPI_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;

	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);          //peripheral clock is enabled in the function itself

	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);
}


void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; //SCK 0f 8MHZ
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN; //ssm enabled for nss pin, because we dont have slave for this application

	SPI_Init(&SPI2handle);  //peripheral clock is enabled in the function itself
							//else we would add -> SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);
}


int main(void)
{
	char user_data[] = "Hello World";

	//this function is to initialize the gpio pins to behave as spi pins
	SPI_GPIOInits();

	//this function is to inititlize the SPI2 peripheral paramenter
	SPI2_Inits();

	//enable the SPI2 peripheral
	SPI_PeripheralControl(SPI2,ENABLE); //setting SPE bit

	SPI_SendData(SPI2,(uint8_t*)user_data, strlen(user_data));

	while(1);
}
