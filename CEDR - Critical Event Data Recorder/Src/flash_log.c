//#include "main.h"
#include <stdio.h>
#include <string.h>
#include "flash_log.h"


static uint32_t flashWriteAddress;

extern W25Q_Handle_t FlashHandle;

static bool flash_log_check_wrap(void);

static void flash_log_erase_region(void);
static void flash_log_print_record(EventRecord_t *record);

/*FOR PERSISTENT DATA WE IMPLEMENT METADATA FUNCTIONALITY*/
static void flash_log_find_next_address(void);


/*
 * flash_log_init()
 * Initializes the flash log manager and sets the
 * write pointer to the start of the log region.
 */
void flash_log_init(void)
{
    /*
     * Step 1
     * Initialize write pointer
     */
	//flashWriteAddress = FLASH_LOG_START_ADDR;
	flash_log_find_next_address();

}

/*
 * flash_log_write()
 * Writes an event record to flash. If the logging
 * region is full, it is erased before writing.
 */
void flash_log_write(EventRecord_t *record)
{
    /*
     * Step 1
     * Check whether wrapping is required
     */
	if(flash_log_check_wrap() == true)
	{
	    /*
	     * Step 2
	     * Erase sector(s) if required
	     */
		flash_log_erase_region();
	}

	/*
     * Step 3
     * Write record to flash
     */
	W25Q_Write(&FlashHandle, flashWriteAddress, (uint8_t *)record, sizeof(EventRecord_t));

    /*
     * Step 4
     * Advance write pointer
     */
	flashWriteAddress += sizeof(EventRecord_t);
}


/*
 * flash_log_check_wrap()
 * Checks whether there is enough space to store
 * another event record.
 */
static bool flash_log_check_wrap(void)
{
	if((flashWriteAddress + sizeof(EventRecord_t)) > FLASH_LOG_END_ADDR)
	{
		return true;
	}else
		return false;
}


/*
 * flash_log_erase_region()
 * Erases the reserved flash logging region and
 * resets the write pointer.
 */
static void flash_log_erase_region(void)
{
	uint32_t addr = FLASH_LOG_START_ADDR;

	while(addr < FLASH_LOG_END_ADDR)
	{
		W25Q_SectorErase(&FlashHandle, addr);
		addr += FLASH_SECTOR_SIZE;
	}

	flashWriteAddress = FLASH_LOG_START_ADDR;

}


/*
 * flash_log_read()
 * Reads a single event record from the specified
 * flash address.
 */
bool flash_log_read(uint32_t address, EventRecord_t *record)
{
	if(address >= FLASH_LOG_END_ADDR)
	{
		return false;
	}

	xSemaphoreTake(spiMutex,portMAX_DELAY);
	W25Q_Read(&FlashHandle, address, (uint8_t*)record, sizeof(EventRecord_t));
	xSemaphoreGive(spiMutex);
	return true;
}

/*
 * flash_log_dump()
 * Reads multiple event records from flash and
 * prints them over UART.
 */
void flash_log_dump(uint32_t startAddress, uint32_t maxRecords)
{
	EventRecord_t RecordDumb;
	uint32_t address = startAddress;

	for(uint32_t i = 0; i < maxRecords ; i++)
	{
		if(!flash_log_read(address, &RecordDumb))
		{
			break;
		}else
		{
			//check record is valid and print over uart
			flash_log_print_record(&RecordDumb);
		}

		address += sizeof(EventRecord_t);
	}

}

/*
 * flash_log_print_record()
 * Formats an event record into a readable string
 * and transmits it over UART.
 */
static void flash_log_print_record(EventRecord_t *record)
{
	char buffer[320];
	snprintf(buffer,
	         sizeof(buffer),
	         "\r\n"
	         "+--------------------------------------+\r\n"
	         "|      CRITICAL EVENT DATA RECORDER    |\r\n"
	         "+--------------------------------------+\r\n"
	         "| Date  : %02d/%02d/20%02d               |\r\n"
	         "| Time  : %02d:%02d:%02d                 |\r\n"
	         "| Event : %-24d |\r\n"
	         "+--------------------------------------+\r\n\r\n",
	         record->date.date,
	         record->date.month,
	         record->date.year,
	         record->time.hours,
	         record->time.minutes,
	         record->time.seconds,
	         record->eventType);

	USART_SendData(&pUartHandle,(uint8_t *)buffer,strlen(buffer));
}

static void flash_log_find_next_address(void)
{
    EventRecord_t record;

    flashWriteAddress = FLASH_LOG_START_ADDR;

    while(flashWriteAddress < FLASH_LOG_END_ADDR)
    {
        W25Q_Read(&FlashHandle,
                  flashWriteAddress,
                  (uint8_t *)&record,
                  sizeof(EventRecord_t));

        /* Found an empty record */
        if(record.eventType == 0xFF)
        {
            return;
        }

        flashWriteAddress += sizeof(EventRecord_t);
    }

    flashWriteAddress = FLASH_LOG_START_ADDR;
}

