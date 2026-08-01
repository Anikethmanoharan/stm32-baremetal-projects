
#ifndef W25Q_H_
#define W25Q_H_

#include "stm32f446xx.h"
#include <stdint.h>


typedef struct
{
    SPI_RegDef_t *pSPIx;

    GPIO_RegDef_t *CSPort;

    uint8_t CSPin;

    uint8_t Manufacturer_ID;

    uint8_t Memory_type;

    uint8_t Capacity;

} W25Q_Handle_t;


typedef enum
{
    W25Q_OK, //0

    W25Q_ERROR //1

}W25Q_Status_t;

#define FLAG_IS_SET		1
#define FLAG_IS_RESET		0

#define pagesize	256U //TODO

//W25Q COMMAND MACROS

#define W25Q_CMD_JEDEC_ID   		 	 0x9F
#define W25Q_CMD_PAGE					 0x02
#define W25Q_CMD_READ					 0x03
#define W25Q_CMD_WRT_EN				 	 0x06
#define W25Q_CMD_READ_STATUS_REG1		 0x05
#define W25Q_CMD_SECTOR_ERASE			 0x20
#define W25Q_CMD_BLOCK_ERASE			 0xD8
#define W25Q_CMD_CHIP_ERASE			     0xC7

//W25Q FLAG MACROS
#define W25Q_SR1_BUSY      (1 << 0)
#define W25Q_SR1_WEL       (1 << 1)

#define BSY_FLAG (1 << SPI_SR_BSY)

#define W25Q_MANUFACTURER_ID   0xEF
#define W25Q_MEMORY_TYPE       0x40
#define W25Q_CAPACITY          0x17

//FUNCTION PROTOTYPES
W25Q_Status_t W25Q_Init(W25Q_Handle_t *pFlash);
void W25Q_ReadJEDECID(W25Q_Handle_t *pFlash,uint8_t *id);
void W25Q_WriteEnable(W25Q_Handle_t *pFlash);
uint8_t W25Q_ReadStatusReg1(W25Q_Handle_t *pFlash);
void W25Q_Read(W25Q_Handle_t *pFlash,uint32_t address,uint8_t *buffer,uint32_t length);
void W25Q_PageProgram(W25Q_Handle_t *pFlash,uint32_t address,uint8_t *buffer,uint32_t length);
void W25Q_SectorErase(W25Q_Handle_t *pFlash,uint32_t address);
void W25Q_BlockErase(W25Q_Handle_t *pFlash,uint32_t address);
void W25Q_ChipErase(W25Q_Handle_t *pFlash);
void W25Q_Write(W25Q_Handle_t *pFlash, uint32_t address, uint8_t *buffer, uint32_t length);

#endif /* W25Q_H_ */
