#include <stdint.h>
#include <string.h>
#include "main.h"
#include "stm32f446xx.h"
#include "flash_log.h"

static void SPI_InitHardware(void);
void UART_InitHardware(void);
static uint8_t RTC_InitHardware(void);
static void OLED_InitHardware(void);
static void FLASH_InitHardware(void);
static void peripheral_inits(void);

static void Button_InitHardware(void);
static void button_interrupt_handler(void);

/*
 * SPI2_MOSI -> PB15      ----> from DATASHEET OF F446RE
 * SPI2_MISO -> PB14
 * SPI2_SCK  -> PB13
 * SPI2_NSS  -> PB12
 * ALT FUNCTION MODE : 5
 */

/*HANDLE FOR EACH TASK*/

TaskHandle_t eventTaskHandle;
TaskHandle_t telemetryTaskHandle;

/***********************/

SemaphoreHandle_t i2cMutex;
SemaphoreHandle_t spiMutex;

W25Q_Handle_t FlashHandle;
USART_Handle_t pUartHandle;

SystemStatus_t systemStatus;


void SPI_InitHardware(void)
{
	// Configure SPI GPIO
	GPIO_Handle_t SPI_pins;
	GPIO_Handle_t SPI_CS_PIN;

	/* CRITICAL: zero every field. GPIO_Init() shifts these values by the pin
	 * number and ORs them in, so a garbage field corrupts NEIGHBOURING pins. */
	memset(&SPI_pins,   0, sizeof(SPI_pins));
	memset(&SPI_CS_PIN, 0, sizeof(SPI_CS_PIN));

	SPI_pins.pGPIOx = GPIOB;
	SPI_pins.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	SPI_pins.GPIO_PinConfig.GPIO_PinAltFunMode  = ALT_FN_NO;      /* AF5 */
	SPI_pins.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_PP;
	SPI_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPI_pins.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_HIGH;

	SPI_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15; GPIO_Init(&SPI_pins); /* MOSI */
	SPI_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14; GPIO_Init(&SPI_pins); /* MISO */
	SPI_pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13; GPIO_Init(&SPI_pins); /* SCK  */

	/* CS as plain push-pull output, idle HIGH */
	SPI_CS_PIN.pGPIOx = GPIOB;
	SPI_CS_PIN.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	SPI_CS_PIN.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_12;
	SPI_CS_PIN.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_PP;
	SPI_CS_PIN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	SPI_CS_PIN.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_HIGH;
	GPIO_Init(&SPI_CS_PIN);

	/* GPIO_Init() never touches ODR, so CS is LOW out of reset.
	 * Deselect BEFORE the peripheral starts clocking. */
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_SET);

	// Configure SPI2
	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;

	//since we are using GPIO pins to set the nss pin low
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	SPI_Init(&SPI2handle);

	//since we are using SSM, we tell master its not slave
	SPI_SSIConfig(SPI2, ENABLE);

	//ENABLE SPI2 peripheral
	SPI_PeripheralControl(SPI2, ENABLE);
}

void UART_InitHardware(void)
{
	//CONFIGURE THE PINS FOR USART
    GPIO_Handle_t USARTPins;

    USARTPins.pGPIOx = GPIOA;

    USARTPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    USARTPins.GPIO_PinConfig.GPIO_PinAltFunMode = 7;      // AF7 = USART2
    USARTPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    USARTPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    USARTPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // PA2 -> USART2_TX
    USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    GPIO_Init(&USARTPins);

    // PA3 -> USART2_RX
    USARTPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
    GPIO_Init(&USARTPins);

	//CONFIGURE THE USART PERIPHERAL
	pUartHandle.pUSARTx = USART2;

	pUartHandle.USART_Config.USART_Mode = USART_MODE_TXRX;
	pUartHandle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	pUartHandle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	pUartHandle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	pUartHandle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	pUartHandle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;

	USART_Init(&pUartHandle);
	//ENABLE USART PERIPHERAL
	USART_PeripheralControl(USART2, ENABLE);
}

uint8_t RTC_InitHardware(void)
{
	uint8_t status = ds1307_init();

	RTC_time_t current_time;
	RTC_date_t current_date;

    current_date.day   = TUESDAY;
    current_date.date  = 01;
    current_date.month = 8;
    current_date.year  = 26;

    current_time.hours       = 11;
    current_time.minutes     = 35;
    current_time.seconds     = 00;
    current_time.time_format = TIME_FORMAT_12HRS_AM;

    ds1307_set_current_time(&current_time);
    ds1307_set_current_date(&current_date);

    return status;
}

void OLED_InitHardware(void)
{
	 SSD1306_Init();

}

void FLASH_InitHardware(void)
{

	//Configure Flash Handle
	FlashHandle.pSPIx = SPI2;

	FlashHandle.CSPort = GPIOB;

	FlashHandle.CSPin = GPIO_PIN_NO_12;

	//Initialize Flash
	if(W25Q_Init(&FlashHandle) != W25Q_OK)
	{
		while(1)
		{
			/*FLASH NOT INITIALIZED*/
		}
	}

}

static void Button_InitHardware(void)
{
	GPIO_Handle_t btn;
	memset(&btn, 0, sizeof(btn));   /* same reason as SPI_InitHardware */

	btn.pGPIOx = GPIOC;
	btn.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_13;
	btn.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IT_FT;  /* falling edge */
	btn.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
	btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;      /* idle HIGH, press pulls LOW */

	GPIO_Init(&btn);   /* enables GPIOC clock, SYSCFG clock, FTSR, IMR */


	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, 5);   /* MUST be >= configMAX_SYSCALL_INTERRUPT_PRIORITY */
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);
}

void peripheral_inits(void)
{
	SPI_InitHardware();

	UART_InitHardware();

	systemStatus.rtcStatus = (RTC_InitHardware() == 0) ? SYSTEM_OK : STATUS_FAIL;

	OLED_InitHardware();

	FLASH_InitHardware();

	Button_InitHardware();
}

int main(void)
{
	peripheral_inits();

    /*CREATING MUTEXES*/
    spiMutex = xSemaphoreCreateMutex();

    i2cMutex = xSemaphoreCreateMutex();

	configASSERT(spiMutex != NULL);
	configASSERT(i2cMutex != NULL);

	OLED_DisplaySystemStatus(&systemStatus);

    BaseType_t status;

    /*CREATING TASKS*/
    status = xTaskCreate(task_evnt_manager, "EVENT MANAGER TASK", 250, NULL, 4, &eventTaskHandle);
    configASSERT(status == pdPASS);

    status = xTaskCreate(task_telemetry_log, "TELEMETRY LOGGER TASK", 250, NULL, 1, &telemetryTaskHandle);
    configASSERT(status == pdPASS);

    flash_log_init();


    /*START THE SCHEDULER*/
    vTaskStartScheduler();

    /* Should never reach here */
    for(;;)
    {
    }
}

void button_interrupt_handler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* eNoAction DISCARDS the value — the task would read stale/garbage.
	 * eSetValueWithOverwrite actually delivers EVENT_BUTTON_PRESS. */
	xTaskNotifyFromISR(eventTaskHandle,
	                   EVENT_BUTTON_PRESS,
	                   eSetValueWithOverwrite,
	                   &xHigherPriorityTaskWoken);

	/* Event task is priority 4; if it just became ready, switch to it on exit. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void EXTI15_10_IRQHandler(void)
{
	/* ALWAYS clear the EXTI pending bit first. Until PR is cleared the NVIC
	 * re-pends this IRQ the instant we return -> infinite re-entry. */
	GPIO_IRQHandling(GPIO_PIN_NO_13);
	button_interrupt_handler();
}

