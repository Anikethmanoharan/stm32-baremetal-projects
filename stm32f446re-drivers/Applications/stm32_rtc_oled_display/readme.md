# stm32-rtc-oled-display

A bare-metal embedded project on the **STM32F446RE** that reads real-time clock data from a DS1307 RTC module and displays live time, date, and day of week on a SSD1306 0.96" OLED — built entirely without HAL using custom low-level C drivers.

---

## What It Does

- Sets the initial time and date once at startup
- Continuously reads from the DS1307 RTC over I2C
- Displays live updating time, date, and day on the OLED

---

## Hardware

| Module | Bus | SCL | SDA |
|---|---|---|---|
| DS1307 RTC | I2C2 | PB10 | PB3 |
| SSD1306 OLED | I2C1 | PB8 | PB9 |

Board: STM32F446RE (Nucleo-64)

---

## Built From Scratch

No HAL. No abstraction libraries. The following drivers were written from the ground up using direct register manipulation:

- I2C
- GPIO

The DS1307 BSP was written on top of these drivers. All communication with both modules goes through the custom I2C driver.

---
