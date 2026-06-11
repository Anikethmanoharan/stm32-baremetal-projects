/*
 * Wiring (4-pin I2C OLED):
 *   VCC  -> 3.3V
 *   GND  -> GND
 *   SCL  -> SSD1306_I2C_SCL_PIN  (default: PB8)
 *   SDA  -> SSD1306_I2C_SDA_PIN  (default: PB9)
 *
 * The SSD1306 shares I2C bus with the DS1307 RTC, so we re-use the
 */

#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_

#include "stm32f446xx.h"
#include "stm32f446xx_i2c_driver.h"

/*---------------------------------------------------------------------------
 * Application-configurable items
 *--------------------------------------------------------------------------*/

#define SSD1306_I2C                 I2C1

/* GPIO port and pins for the OLED I2C lines */
#define SSD1306_I2C_GPIO_PORT       GPIOB
#define SSD1306_I2C_SCL_PIN         GPIO_PIN_NO_8//GPIO_PIN_NO_6    /* PB6 -> I2C1_SCL (AF4) */
#define SSD1306_I2C_SDA_PIN         GPIO_PIN_NO_9//GPIO_PIN_NO_7    /* PB7 -> I2C1_SDA (AF4) */
#define SSD1306_I2C_SPEED           I2C_SCL_SPEED_FM4K  /* SSD1306 supports up to 400 kHz */
#define SSD1306_I2C_PUPD            GPIO_NO_PUPD     /* Use external pull-ups (4.7 kΩ) */

/* 7-bit I2C address of the SSD1306.
 */
#define SSD1306_I2C_ADDRESS         0x3C

/*---------------------------------------------------------------------------
 * Display geometry
 *--------------------------------------------------------------------------*/
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64
#define SSD1306_PAGES               8    /* 64 / 8 = 8 pages */

/*---------------------------------------------------------------------------
 * Built-in 5×7 font (one character = 5 bytes, column-encoded)
 * Characters 0x20 (space) … 0x7E (~) are supported.
 *--------------------------------------------------------------------------*/
#define SSD1306_FONT_WIDTH          6    /* 5 pixel columns + 1 space */
#define SSD1306_FONT_HEIGHT         8    /* 1 page = 8 rows            */

/*---------------------------------------------------------------------------
 * Public API
 *--------------------------------------------------------------------------*/

/**
 * @brief  Initialize the SSD1306 OLED display.
 *         Configures GPIO, I2C, and sends the required SSD1306 init sequence.
 *         Call this once before any other SSD1306 function.
 */
void SSD1306_Init(void);

/**
 * @brief  Clear the entire display (fill with 0x00) and reset cursor to (0,0).
 */
void SSD1306_Clear(void);

/**
 * @brief  Set the cursor position.
 * @param  page  Page number  0 … (SSD1306_PAGES - 1)
 * @param  col   Column index 0 … (SSD1306_WIDTH  - 1)
 */
void SSD1306_SetCursor(uint8_t page, uint8_t col);

/**
 * @brief  Print a null-terminated ASCII string at the current cursor position.
 *         Advances the cursor automatically after each character.
 *         Characters outside printable ASCII range are replaced with a space.
 * @param  str  Pointer to null-terminated string.
 */
void SSD1306_PrintString(const char *str);

/**
 * @brief  Print a single printable ASCII character at the current cursor.
 * @param  c  Character to print.
 */
void SSD1306_PrintChar(char c);

/**
 * @brief  Convenience: clear display and print two lines.
 *         line1 is printed on page 0, line2 on page 2.
 * @param  line1  Top line text.
 * @param  line2  Bottom-ish line text.
 */
void SSD1306_DisplayTwoLines(const char *line1, const char *line2);

#endif /* SSD1306_OLED_H_ */
