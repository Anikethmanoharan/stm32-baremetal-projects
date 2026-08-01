#ifndef FLASH_LOG_H_
#define FLASH_LOG_H_

#include "main.h"
#include <stdbool.h>

#define FLASH_LOG_START_ADDR    0x00010000U
#define FLASH_LOG_END_ADDR      0x0007FFFFU
#define FLASH_END_ADDR		   0x800000U
#define FLASH_SECTOR_SIZE      0x1000U
#define FLASH_META_ADDR        0x000000U

#define FLASH_MAGIC 0x43454452U //CEDR IN HEXADECIMAL

typedef struct
{
    uint32_t magic;
    uint32_t writeAddress;

} FlashMetadata_t;

void flash_log_init(void);

/*API TO WRITE TO FLASH*/
void flash_log_write(EventRecord_t *record);

bool flash_log_read(uint32_t address, EventRecord_t *record);

void flash_log_dump(uint32_t startAddress, uint32_t maxRecords);

#endif /* FLASH_LOG_H_ */
