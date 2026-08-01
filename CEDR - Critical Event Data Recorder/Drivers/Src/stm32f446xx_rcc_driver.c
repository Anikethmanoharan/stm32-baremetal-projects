#include "stm32f446xx_rcc_driver.h"

uint16_t AHB_Prescalar[8] = {2, 4, 8, 16, 64, 128, 256, 512}; // array with prescalar values of AHB
uint16_t APB1_Prescalar[4] = {2, 4, 8, 16}; // array with prescalar values of APB1
uint16_t APB2_Prescalar[4] = {2, 4, 8, 16};

//calculate the clock frequency, we know its 16mhz but it might not be the case all the time, user may change it
//this is from the rcc register cfgr and SWS bit portion (reference manual)

uint32_t RCC_GetPCLK1Value(void) // for APB
{
	uint32_t pclk1 , SystemClk;
	uint8_t clksrc , temp , ahbp, apb1p;

	//a.FIND THE CLK SOURCE (HSI/HSE/PLL)
	clksrc = ( RCC->CFGR >> 2 ) & 0x3; //we are shifting the 2nd and 3rd bit to front and &ing to check

	if(clksrc == 0)
	{
		SystemClk = 16000000; //HSI = 16MHZ
	}else if(clksrc == 1)
	{
		SystemClk = 8000000; //HSE = 8MHZ
	}else if(clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock(); //PLL - we use this func to find PLL value
	}

	//b.FIND THE AHB1 PRESCALAR - HPRE bit of cfgr reg
	temp = (( RCC->CFGR >> 4 ) & 0xF);

	if(temp < 8)
	{
		ahbp = 1; //no prescalar
	}else
	{
		ahbp = AHB_Prescalar[temp - 8];
	}

	//c.FIND THE APB1 PRESCALAR - PPRE1 bit of cfgr reg
	temp = (( RCC->CFGR >> 10 ) & 0x7);

	if(temp < 4)
	{
		apb1p = 1;
	}else
	{
		apb1p = APB1_Prescalar[temp - 4];
	}

	//d.CALCULATING pclk1
	pclk1 = (SystemClk / ahbp) / apb1p;


	return pclk1;
}

uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t pclk2 , SystemClk;
	uint8_t clksrc , temp , ahbp, apb2p;

	//a.FIND THE CLK SOURCE (HSI/HSE/PLL)
	clksrc = ( RCC->CFGR >> 2 ) & 0x3; //we are shifting the 2nd and 3rd bit to front and &ing to check

	if(clksrc == 0)
	{
		SystemClk = 16000000; //HSI = 16MHZ
	}else if(clksrc == 1)
	{
		SystemClk = 8000000; //HSE = 8MHZ
	}else if(clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock(); //PLL - we use this func to find PLL value
	}

	//b.FIND THE AHB2 PRESCALAR - HPRE bit of cfgr reg
	temp = (( RCC->CFGR >> 4 ) & 0xF);

	if(temp < 8)
	{
		ahbp = 1; //no prescalar
	}else
	{
		ahbp = AHB_Prescalar[temp - 8];
	}

	//c.FIND THE APB2 PRESCALAR - PPRE2 bit of cfgr reg
	temp = (( RCC->CFGR >> 13 ) & 0x7);

	if(temp < 4)
	{
		apb2p = 1;
	}else
	{
		apb2p = APB2_Prescalar[temp - 4];
	}

	//d.CALCULATING pclk1
	pclk2 = (SystemClk / ahbp) / apb2p;


	return pclk2;

}

uint32_t RCC_GetPLLOutputClock()
{
	return 0; //not implemented for now
}
