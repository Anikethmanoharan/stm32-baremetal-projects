/******************************************************************************
 * @file    task_handler.c
 * @brief   FreeRTOS application tasks for the Critical Event Data Recorder.
 *
 * This module implements the application-level tasks responsible for:
 *  - Event processing
 *  - Telemetry logging
 *  - OLED status updates
 *  - Flash data logging
 *  - UART log output
 *
 * Tasks:
 *  - task_evnt_manager()
 *  - task_telemetry_log()
 *
 ******************************************************************************/

#include "main.h"
#include "stm32f446xx_gpio_driver.h"
#include "flash_log.h"

/*This api is also being used in main.c during initialisation
 * to use oled on startup.
 * */
/*
 * Displays the default system status screen on the OLED.
 *
 * This screen is shown:
 * 1. During system startup after peripheral initialization.
 * 2. After a temporary event notification expires.
 *
 * Displays the current system health based on the
 * SystemStatus_t structure.
 */
void OLED_DisplaySystemStatus(SystemStatus_t *status)
{
	SSD1306_Clear();

	SSD1306_SetCursor(0, 25);
	SSD1306_PrintString("CEDR - STATUS");

	if(status->rtcStatus == SYSTEM_OK)
	{
		SSD1306_SetCursor(4,0);
		SSD1306_PrintString("PERIODIC LOG ");

		SSD1306_SetCursor(6,0);
		SSD1306_PrintString("SYSTEM ON");
	}else
	{
		SSD1306_SetCursor(6,0);
		SSD1306_PrintString("SYSTEM FAILED");
	}

}

/*
 * Displays a temporary event notification on the OLED.
 *
 * Converts the event timestamp into a human-readable format
 * and displays the event type along with the logging status.
 *
 * NOTE:
 * Currently supports only button press and periodic log events.
 * Additional event types can be added later.
 */
static void OLED_DisplayEvent(EventRecord_t *record) //TODO implement other event prints
{
	char timestr[20]; //buffer to store the time in correct format - "HH:MM:SS"

	/*converting time*/
	timestr[0] = (record->time.hours / 10) + '0';
	timestr[1] = (record->time.hours % 10) + '0';

	timestr[2] = ':';

	timestr[3] = (record->time.minutes / 10) + '0';
	timestr[4] = (record->time.minutes % 10) + '0';

	timestr[5] = ':';

	timestr[6] = (record->time.seconds / 10) + '0';
	timestr[7] = (record->time.seconds % 10) + '0';

	timestr[8] = '\0';

	//clear the screen
	SSD1306_Clear();

	//print the title
	SSD1306_SetCursor(0, 50);
	SSD1306_PrintString("CEDR");

	//print event type
	switch(record->eventType)
	{
	case EVENT_BUTTON_PRESS:
		SSD1306_SetCursor(2, 0);
		SSD1306_PrintString("EVENT : BUTTON PRESS");

		break;
	case EVENT_PERIODIC_LOG:
		SSD1306_SetCursor(2, 0);
		SSD1306_PrintString("EVENT : PERIODIC LOG");
		break;

	default:
		break;
	}

	//TODO : print time
	SSD1306_SetCursor(4, 40);
	SSD1306_PrintString(timestr);

	SSD1306_SetCursor(6,40);
	SSD1306_PrintString("LOGGED");
}


/*
 * Event Manager Task
 *
 * Responsibilities:
 * - Waits indefinitely for event notifications from ISRs.
 * - Reads the current RTC timestamp.
 * - Creates an event record.
 * - Stores the event into external flash.
 * - Displays the event on the OLED.
 * - Restores the default OLED status screen.
 * - Performs software debounce.
 * - Dumps the most recent log history over UART.
 *
 * Synchronization:
 * - SPI access protected using spiMutex.
 * - OLED access protected using i2cMutex.
 */
void task_evnt_manager(void* parameters)
{
	uint32_t eventType;
	EventRecord_t record;

	while(1)
	{
		//wait until notification arrives for an event
		xTaskNotifyWait(0,0,&eventType,portMAX_DELAY);

		//read RTC
        ds1307_get_current_time(&record.time);
        ds1307_get_current_date(&record.date);

        switch(eventType){

        case EVENT_BUTTON_PRESS:

            record.eventType = EVENT_BUTTON_PRESS;

    		//get SPI MUTEX
            xSemaphoreTake(spiMutex,portMAX_DELAY);

            flash_log_write(&record);

    		//give SPI mutex
            xSemaphoreGive(spiMutex);

    		//alert in oled
            xSemaphoreTake(i2cMutex,portMAX_DELAY);

            OLED_DisplayEvent(&record);

            xSemaphoreGive(i2cMutex);

            /* Keep event visible */

            vTaskDelay(pdMS_TO_TICKS(3000));

            /* Restore idle screen */

            xSemaphoreTake(i2cMutex, portMAX_DELAY);

            OLED_DisplaySystemStatus(&systemStatus);

            xSemaphoreGive(i2cMutex);

			/* ================= DEBOUNCE — HERE ================= */
			/* Both mutexes are released, so blocking here starves nobody.
			 * The 50 ms window covers the mechanical bounce; anything the
			 * ISR notified during it is then discarded so one physical
			 * press produces exactly one logged event.                  */
			vTaskDelay(pdMS_TO_TICKS(50));
			xTaskNotifyStateClear(NULL);   /* NULL = this task */
			flash_log_dump(FLASH_LOG_START_ADDR, 10);
			/* =================================================== */

            break;
        default:
            break;
        }

	}
}


/*
 * Telemetry Logger Task
 *
 * Executes periodically every second.
 *
 * Responsibilities:
 * - Acquire the current RTC timestamp.
 * - Create a periodic telemetry event.
 * - Store the event into external flash memory.
 *
 * Future Work:
 * - Acquire ADC sensor data.
 * - Log system telemetry values.
 */
void task_telemetry_log(void* parameters)
{
	EventRecord_t record;

	while(1)
	{
		// get adc value TODO : LATER

		//get current current date & time
        ds1307_get_current_time(&record.time);
        ds1307_get_current_date(&record.date);

        /* Mark as periodic telemetry */
        record.eventType = EVENT_PERIODIC_LOG;

        /* Write to Flash */
        xSemaphoreTake(spiMutex,portMAX_DELAY);

        flash_log_write(&record);

        xSemaphoreGive(spiMutex);

        /* Wait 1 second */
        vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


