# BSP — DS1307 RTC & SSD1306 OLED

Board Support Package for the DS1307 RTC module and SSD1306 0.96" OLED display, running on the **STM32F446RE** using custom bare-metal I2C drivers (no HAL).

---

## Hardware

| Module | Bus | SCL | SDA |
|---|---|---|---|
| DS1307 RTC | I2C2 | PB10 | PB3 |
| SSD1306 OLED | I2C1 | PB8 | PB9 |

---

## Files

| File | Description |
|---|---|
| `ds1307.c / .h` | BSP for DS1307 RTC — init, set/get time and date, BCD conversion |
| `ssd1306_oled.c / .h` | BSP for SSD1306 OLED — init, clear, cursor, print string/char |

---

## DS1307 API

```c
uint8_t ds1307_init(void);                          // Returns 0 on success, 1 on failure
void    ds1307_set_current_time(RTC_time_t *);
void    ds1307_get_current_time(RTC_time_t *);
void    ds1307_set_current_date(RTC_date_t *);
void    ds1307_get_current_date(RTC_date_t *);
```

Time format options: `TIME_FORMAT_24HRS`, `TIME_FORMAT_12HRS_AM`, `TIME_FORMAT_12HRS_PM`

---

## SSD1306 API

```c
void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_SetCursor(uint8_t page, uint8_t col);
void SSD1306_PrintChar(char c);
void SSD1306_PrintString(const char *str);
void SSD1306_DisplayTwoLines(const char *line1, const char *line2);
```

Display: 128×64 pixels, 8 pages. Built-in 5×7 ASCII font (0x20–0x7E).

---

## Dependencies

Depends on the custom STM32F446xx peripheral drivers:
- `stm32f446xx_i2c_driver.h` — I2C master send/receive, peripheral control
- `stm32f446xx_gpio_driver.h` — GPIO init and configuration

---

## Notes

- DS1307 supports Standard Mode I2C only (100 kHz)
- SSD1306 runs at Fast Mode (400 kHz)
