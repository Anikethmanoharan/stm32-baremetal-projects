# W25Q64 SPI Flash Driver (Bare-Metal, STM32F446RE)

A driver for the Winbond W25Q64 (64 Mbit) SPI NOR flash chip. No HAL — direct register access on the STM32F446RE, with a hand-written SPI driver underneath.

First time working with an external flash chip, so the goal was to actually understand how NOR flash works, not just call a library.

## What it can do

- Read JEDEC ID and verify the connected chip
- Read status register (busy / write-enable bits)
- Erase: sector (4 KB), block (64 KB), whole chip
- Write a single page (256 bytes)
- Write any length of data — automatically splits across pages
- Read any length of data

## How to use it

```c
W25Q_Handle_t flash;
flash.pSPIx  = SPI1;
flash.CSPort = GPIOB;
flash.CSPin  = GPIO_PIN_NO_6;

if (W25Q_Init(&flash) == W25Q_OK)
{
    uint8_t data[10] = "HelloFlash";
    uint8_t readBack[10];

    W25Q_SectorErase(&flash, 0x000000);   // must erase before writing
    W25Q_Write(&flash, 0x000000, data, 10);
    W25Q_Read(&flash, 0x000000, readBack, 10);
}
```

## Why flash is different from other peripherals

- You can't just overwrite it — it has to be erased first (resets to `0xFF`)
- Writes can't cross a 256-byte page boundary, or data silently corrupts
- Every erase/write is async — you poll a busy bit until the chip is actually done

## Not done yet

- No timeout on the busy-wait (a stuck chip hangs forever right now)
- No read-after-write verification
- No write-protection handling

## Hardware

- STM32F446RE Nucleo
- Winbond W25Q64 (SPI, standard mode)
