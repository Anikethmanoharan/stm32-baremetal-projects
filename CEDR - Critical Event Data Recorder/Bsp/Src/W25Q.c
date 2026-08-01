/*read the chatgpt response before coding there is roadmap to what code next*/
#include <stdint.h>
#include "W25Q.h"

/*private function prototypes START*/

static void W25Q_Select(W25Q_Handle_t *pFlash);
static void W25Q_Deselect(W25Q_Handle_t *pFlash);
static uint8_t W25Q_TransferByte(W25Q_Handle_t *pFlash ,uint8_t txData);
static void W25Q_WaitBusy(W25Q_Handle_t *pFlash);
static void W25Q_SendAddress(W25Q_Handle_t *pFlash,uint32_t address);
static uint8_t check_flag_status(SPI_RegDef_t *pspix , uint32_t FlagName);

/*private function prototypes END*/


/**
 * @brief  Initializes and verifies the W25Q flash device.
 *
 * @details
 * Ensures the device is idle, reads the JEDEC ID,
 * stores device information into the flash handle,
 * and verifies that the connected device matches
 * the expected flash memory.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 *
 * @return W25Q_OK if device is detected successfully,
 *         W25Q_ERROR otherwise.
 */

W25Q_Status_t W25Q_Init(W25Q_Handle_t *pFlash)
{
	uint8_t id[3];

	/* Select Flash */
	W25Q_Deselect(pFlash);

	W25Q_WaitBusy(pFlash);

	//read JEDECIC ID
	W25Q_ReadJEDECID(pFlash, id);


	//storing inside handle
	pFlash->Manufacturer_ID = id[0];
	pFlash->Memory_type = id[1];
	pFlash->Capacity = id[2];

	//comparing if its 64bit version

	if(id[0] == W25Q_MANUFACTURER_ID && id[1] == W25Q_MEMORY_TYPE && id[2] == W25Q_CAPACITY)
	{
		return W25Q_OK;
	}else
	{
		return W25Q_ERROR;
	}
}


/**
 * @brief  Reads the JEDEC identification bytes from flash.
 *
 * @details
 * Sends the JEDEC ID command (0x9F) and reads:
 * Manufacturer ID, Memory Type, and Capacity.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  id Pointer to 3-byte buffer to store ID values.
 *
 * @return None.
 */
void W25Q_ReadJEDECID(W25Q_Handle_t *pFlash,uint8_t *id)
{
	//CS LOW
	W25Q_Select(pFlash);

	//transfer byte
	(void)W25Q_TransferByte(pFlash,W25Q_CMD_JEDEC_ID);

	id[0] = W25Q_TransferByte(pFlash,0xFF);

	id[1] = W25Q_TransferByte(pFlash,0xFF);

	id[2] = W25Q_TransferByte(pFlash,0xFF);

	//CS HIGH
	W25Q_Deselect(pFlash);
}


/**
 * @brief  Enables flash write/erase operations.
 *
 * @details
 * Sends Write Enable command (0x06), which sets the
 * Write Enable Latch (WEL) bit before any program
 * or erase command.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 *
 * @return None.
 */

void W25Q_WriteEnable(W25Q_Handle_t *pFlash)
{
	/* Select Flash */
	W25Q_Select(pFlash);

	(void)W25Q_TransferByte(pFlash, W25Q_CMD_WRT_EN);
/*
	//MODIFICATION - check WEL bit is set
	while(!(W25Q_ReadStatusReg1(pFlash)) & W25Q_SR1_WEL);
*/
	/* De-select Flash */
	W25Q_Deselect(pFlash);
}


/**
 * @brief  Reads Status Register-1 from flash.
 *
 * @details
 * Used to check flash status bits such as BUSY
 * and Write Enable Latch (WEL).
 *
 * @param  pFlash Pointer to W25Q handle structure.
 *
 * @return Status register value.
 */

uint8_t W25Q_ReadStatusReg1(W25Q_Handle_t *pFlash)
{
	uint8_t status;
	/* Select Flash */
	W25Q_Select(pFlash);

	(void)W25Q_TransferByte(pFlash, W25Q_CMD_READ_STATUS_REG1);

	//reading the status register
	status = W25Q_TransferByte(pFlash, 0xFF);

	/* De-select Flash */
	W25Q_Deselect(pFlash);

	return status;
}


/**
 * @brief  Reads data from flash memory.
 *
 * @details
 * Sends read command, starting address, and receives
 * the requested number of bytes into the user buffer.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  address 24-bit flash memory start address.
 * @param  buffer Pointer to destination buffer.
 * @param  length Number of bytes to read.
 *
 * @return None.
 */

void W25Q_Read(W25Q_Handle_t *pFlash,uint32_t address,uint8_t *buffer,uint32_t length)
{
	uint8_t addr1,addr2,addr3;

	/* Select Flash */
	W25Q_Select(pFlash);

	//READ COMMAND
	(void)W25Q_TransferByte(pFlash, W25Q_CMD_READ);

	addr1 = (address & (0xFF0000)) >> 16;
	addr2 = (address & (0x00FF00)) >> 8;
	addr3 = (address & (0x0000FF)) >> 0;

	(void)W25Q_TransferByte(pFlash, addr1);
	(void)W25Q_TransferByte(pFlash, addr2);
	(void)W25Q_TransferByte(pFlash, addr3);

	for(int i = 0; i < length ;i++)
	{
		buffer[i] = W25Q_TransferByte(pFlash, 0xFF);
	}

	/* De-select Flash */
	W25Q_Deselect(pFlash);

}


/**
 * @brief  Programs data into a single flash page.
 *
 * @details
 * Writes data using Page Program command.
 * The write must not cross the 256-byte page boundary.
 * For larger writes use W25Q_Write().
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  address Flash memory start address.
 * @param  buffer Pointer to data buffer.
 * @param  length Number of bytes to program.
 *
 * @return None.
 */

void W25Q_PageProgram(W25Q_Handle_t *pFlash,uint32_t address,uint8_t *buffer,uint32_t length)
{

	W25Q_WriteEnable(pFlash);

	/* Select Flash */
	W25Q_Select(pFlash);

	//send page command
	(void)W25Q_TransferByte(pFlash, W25Q_CMD_PAGE);

	W25Q_SendAddress(pFlash, address);

	//sending data to flash (page 256 bytes)
	for(int i = 0; i < length ; i++)
	{
		(void)W25Q_TransferByte(pFlash, buffer[i]);
	}

	/* De-select Flash */
	W25Q_Deselect(pFlash);

	W25Q_WaitBusy(pFlash);

}


/**
 * @brief  Erases a 4KB flash sector.
 *
 * @details
 * Sends sector erase command. The sector containing
 * the provided address will be erased.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  address Address inside the sector to erase.
 *
 * @return None.
 */
void W25Q_SectorErase(W25Q_Handle_t *pFlash,uint32_t address)
{
	W25Q_WriteEnable(pFlash);

	/* Select Flash */
	W25Q_Select(pFlash);

	//send sector erase  command
	(void)W25Q_TransferByte(pFlash, W25Q_CMD_SECTOR_ERASE);

	//sending address
	W25Q_SendAddress(pFlash, address);

	/* De-select Flash */
	W25Q_Deselect(pFlash);

	W25Q_WaitBusy(pFlash);
}


/**
 * @brief  Erases a 64KB flash block.
 *
 * @details
 * Sends block erase command. The block containing
 * the provided address will be erased.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  address Address inside the block to erase.
 *
 * @return None.
 */

void W25Q_BlockErase(W25Q_Handle_t *pFlash,uint32_t address)
{
	W25Q_WriteEnable(pFlash);

	/* Select Flash */
	W25Q_Select(pFlash);

	//send block erase  command
	(void)W25Q_TransferByte(pFlash, W25Q_CMD_BLOCK_ERASE);

	//sending address
	W25Q_SendAddress(pFlash, address);

	/* De-select Flash */
	W25Q_Deselect(pFlash);

	W25Q_WaitBusy(pFlash);
}


/**
 * @brief  Erases the entire flash memory.
 *
 * @details
 * Sends chip erase command. All memory locations
 * are reset to erased state (0xFF).
 *
 * @param  pFlash Pointer to W25Q handle structure.
 *
 * @return None.
 */
void W25Q_ChipErase(W25Q_Handle_t *pFlash)
{
	W25Q_WriteEnable(pFlash);

	/* Select Flash */
	W25Q_Select(pFlash);

	//send chip erase  command
	(void)W25Q_TransferByte(pFlash, W25Q_CMD_CHIP_ERASE);

	/* De-select Flash */
	W25Q_Deselect(pFlash);

	W25Q_WaitBusy(pFlash);
}

/**
 * @brief  Writes data of any length to flash memory.
 *
 * @details
 * Splits large writes into multiple page program
 * operations to prevent crossing page boundaries.
 *
 * @param  pFlash Pointer to W25Q handle structure.
 * @param  address Flash memory start address.
 * @param  buffer Pointer to data buffer.
 * @param  length Number of bytes to write.
 *
 * @return None.
 */
void W25Q_Write(W25Q_Handle_t *pFlash, uint32_t address, uint8_t *buffer, uint32_t length)
{
	//W25Q_Write() takes a large write request and breaks it into multiple safe W25Q_PageProgram() calls.
	uint32_t pageOffset;
	uint32_t pageRemaining;
	uint32_t bytesToWrite;


	while(length > 0)
	{
		//finding which page are we in
		pageOffset = address % 256;


		//finding the reaining space left in the page
		pageRemaining = 256 - pageOffset;

		//write size
		if(length <  pageRemaining)
		{
			bytesToWrite = length;

		}else
		{
			bytesToWrite = pageRemaining;
		}

		W25Q_PageProgram(pFlash, address, buffer, bytesToWrite);

		buffer += bytesToWrite;
		address += bytesToWrite;
		length -= bytesToWrite;
	}

}

                        /*static helper function APIs*/

/**
 * @brief Pulls CS pin LOW to select flash device.
 */
static void W25Q_Select(W25Q_Handle_t *pFlash)
{
	GPIO_WriteToOutputPin(pFlash->CSPort,pFlash->CSPin,GPIO_PIN_RESET);
}

/**
 * @brief Pulls CS pin HIGH to release flash device.
 */
static void W25Q_Deselect(W25Q_Handle_t *pFlash)
{
	GPIO_WriteToOutputPin(pFlash->CSPort,pFlash->CSPin,GPIO_PIN_SET);
}

/**
 * @brief  Transfers one byte over SPI.
 *
 * @details
 * Sends one byte and returns the simultaneously
 * received byte from the flash device.
 *
 * @return Received byte.
 */
static uint8_t W25Q_TransferByte(W25Q_Handle_t *pFlash ,uint8_t txData)
{
	uint8_t rxdata;
	SPI_SendData(pFlash->pSPIx, &txData, 1);

	while(check_flag_status(pFlash->pSPIx, BSY_FLAG));

	SPI_ReceieveData(pFlash->pSPIx, &rxdata, 1);

	return rxdata;
}

/**
 * @brief Waits until flash internal operation completes.
 *
 * @details
 * Polls BUSY bit in Status Register-1 until flash
 * becomes ready.
 */
static void W25Q_WaitBusy(W25Q_Handle_t *pFlash) //TODO timeout functionality
{
    while (W25Q_ReadStatusReg1(pFlash) & W25Q_SR1_BUSY)
    {
        /* Wait until BUSY bit clears */
    }
}

/**
 * @brief Sends a 24-bit flash address.
 *
 * @details
 * Splits address into three bytes:
 * Address[23:16], Address[15:8], Address[7:0].
 */
static void W25Q_SendAddress(W25Q_Handle_t *pFlash,uint32_t address)
{
	uint8_t addr1,addr2,addr3;

	//splitting and sending address
	addr1 = (address & (0xFF0000)) >> 16;
	addr2 = (address & (0x00FF00)) >> 8;
	addr3 = (address & (0x0000FF)) >> 0;

	(void)W25Q_TransferByte(pFlash, addr1);
	(void)W25Q_TransferByte(pFlash, addr2);
	(void)W25Q_TransferByte(pFlash, addr3);
}

static uint8_t check_flag_status(SPI_RegDef_t *pspix , uint32_t FlagName)
{
	if(pspix->SR & FlagName)
	{
		return FLAG_IS_SET;
	}else
	{
		return FLAG_IS_RESET;
	}

}
