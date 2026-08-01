#ifndef MAIN_H_
#define MAIN_H_

/* USER CODE Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "stm32f446xx.h"	//MCU HEADER - F446RE
#include "ds1307.h" 		//RTC HEADER
#include "ssd1306_oled.h"   //OLED HEADER
#include "W25Q.h" 			//FLASH MODULE HEADER
#include "event_record.h"

extern SemaphoreHandle_t spiMutex;
extern SemaphoreHandle_t i2cMutex;

extern W25Q_Handle_t FlashHandle;
extern USART_Handle_t pUartHandle;

#define ALT_FN_NO		5
//#define ADC_THRESHOLD 3000


void task_evnt_manager(void* parameters);
void task_telemetry_log(void* parameters);

/* Generic peripheral status */
typedef enum
{
	SYSTEM_OK = 0,
	STATUS_FAIL,
}status_t;


/*
 * Stores the initialization status of
 * system peripherals.
 */
typedef struct
{
	status_t rtcStatus;

	//TODO
	status_t oledStatus;
	status_t flashStatus;

}SystemStatus_t;

extern SystemStatus_t systemStatus;

void OLED_DisplaySystemStatus(SystemStatus_t *status);

#endif /* MAIN_H_ */
