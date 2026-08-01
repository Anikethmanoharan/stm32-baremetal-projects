
#include <string.h>
#include "ssd1306_oled.h"
#include "stm32f446xx_i2c_driver.h"

/*---------------------------------------------------------------------------
 * Module-private I2C handle
 *--------------------------------------------------------------------------*/
static I2C_Handle_t g_ssd1306I2cHandle;

/*---------------------------------------------------------------------------
 * Minimal 5×7 ASCII font (columns 0x20 … 0x7E)
 * Each character is 5 bytes wide; one byte = one column of 8 pixels.
 *--------------------------------------------------------------------------*/
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* 0x20 space */
    {0x00,0x00,0x5F,0x00,0x00}, /* 0x21 !     */
    {0x00,0x07,0x00,0x07,0x00}, /* 0x22 "     */
    {0x14,0x7F,0x14,0x7F,0x14}, /* 0x23 #     */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* 0x24 $     */
    {0x23,0x13,0x08,0x64,0x62}, /* 0x25 %     */
    {0x36,0x49,0x55,0x22,0x50}, /* 0x26 &     */
    {0x00,0x05,0x03,0x00,0x00}, /* 0x27 '     */
    {0x00,0x1C,0x22,0x41,0x00}, /* 0x28 (     */
    {0x00,0x41,0x22,0x1C,0x00}, /* 0x29 )     */
    {0x14,0x08,0x3E,0x08,0x14}, /* 0x2A *     */
    {0x08,0x08,0x3E,0x08,0x08}, /* 0x2B +     */
    {0x00,0x50,0x30,0x00,0x00}, /* 0x2C ,     */
    {0x08,0x08,0x08,0x08,0x08}, /* 0x2D -     */
    {0x00,0x60,0x60,0x00,0x00}, /* 0x2E .     */
    {0x20,0x10,0x08,0x04,0x02}, /* 0x2F /     */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0x30 0     */
    {0x00,0x42,0x7F,0x40,0x00}, /* 0x31 1     */
    {0x42,0x61,0x51,0x49,0x46}, /* 0x32 2     */
    {0x21,0x41,0x45,0x4B,0x31}, /* 0x33 3     */
    {0x18,0x14,0x12,0x7F,0x10}, /* 0x34 4     */
    {0x27,0x45,0x45,0x45,0x39}, /* 0x35 5     */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 0x36 6     */
    {0x01,0x71,0x09,0x05,0x03}, /* 0x37 7     */
    {0x36,0x49,0x49,0x49,0x36}, /* 0x38 8     */
    {0x06,0x49,0x49,0x29,0x1E}, /* 0x39 9     */
    {0x00,0x36,0x36,0x00,0x00}, /* 0x3A :     */
    {0x00,0x56,0x36,0x00,0x00}, /* 0x3B ;     */
    {0x08,0x14,0x22,0x41,0x00}, /* 0x3C <     */
    {0x14,0x14,0x14,0x14,0x14}, /* 0x3D =     */
    {0x00,0x41,0x22,0x14,0x08}, /* 0x3E >     */
    {0x02,0x01,0x51,0x09,0x06}, /* 0x3F ?     */
    {0x32,0x49,0x79,0x41,0x3E}, /* 0x40 @     */
    {0x7E,0x11,0x11,0x11,0x7E}, /* 0x41 A     */
    {0x7F,0x49,0x49,0x49,0x36}, /* 0x42 B     */
    {0x3E,0x41,0x41,0x41,0x22}, /* 0x43 C     */
    {0x7F,0x41,0x41,0x22,0x1C}, /* 0x44 D     */
    {0x7F,0x49,0x49,0x49,0x41}, /* 0x45 E     */
    {0x7F,0x09,0x09,0x09,0x01}, /* 0x46 F     */
    {0x3E,0x41,0x49,0x49,0x7A}, /* 0x47 G     */
    {0x7F,0x08,0x08,0x08,0x7F}, /* 0x48 H     */
    {0x00,0x41,0x7F,0x41,0x00}, /* 0x49 I     */
    {0x20,0x40,0x41,0x3F,0x01}, /* 0x4A J     */
    {0x7F,0x08,0x14,0x22,0x41}, /* 0x4B K     */
    {0x7F,0x40,0x40,0x40,0x40}, /* 0x4C L     */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* 0x4D M     */
    {0x7F,0x04,0x08,0x10,0x7F}, /* 0x4E N     */
    {0x3E,0x41,0x41,0x41,0x3E}, /* 0x4F O     */
    {0x7F,0x09,0x09,0x09,0x06}, /* 0x50 P     */
    {0x3E,0x41,0x51,0x21,0x5E}, /* 0x51 Q     */
    {0x7F,0x09,0x19,0x29,0x46}, /* 0x52 R     */
    {0x46,0x49,0x49,0x49,0x31}, /* 0x53 S     */
    {0x01,0x01,0x7F,0x01,0x01}, /* 0x54 T     */
    {0x3F,0x40,0x40,0x40,0x3F}, /* 0x55 U     */
    {0x1F,0x20,0x40,0x20,0x1F}, /* 0x56 V     */
    {0x3F,0x40,0x38,0x40,0x3F}, /* 0x57 W     */
    {0x63,0x14,0x08,0x14,0x63}, /* 0x58 X     */
    {0x07,0x08,0x70,0x08,0x07}, /* 0x59 Y     */
    {0x61,0x51,0x49,0x45,0x43}, /* 0x5A Z     */
    {0x00,0x7F,0x41,0x41,0x00}, /* 0x5B [     */
    {0x02,0x04,0x08,0x10,0x20}, /* 0x5C \     */
    {0x00,0x41,0x41,0x7F,0x00}, /* 0x5D ]     */
    {0x04,0x02,0x01,0x02,0x04}, /* 0x5E ^     */
    {0x40,0x40,0x40,0x40,0x40}, /* 0x5F _     */
    {0x00,0x01,0x02,0x04,0x00}, /* 0x60 `     */
    {0x20,0x54,0x54,0x54,0x78}, /* 0x61 a     */
    {0x7F,0x48,0x44,0x44,0x38}, /* 0x62 b     */
    {0x38,0x44,0x44,0x44,0x20}, /* 0x63 c     */
    {0x38,0x44,0x44,0x48,0x7F}, /* 0x64 d     */
    {0x38,0x54,0x54,0x54,0x18}, /* 0x65 e     */
    {0x08,0x7E,0x09,0x01,0x02}, /* 0x66 f     */
    {0x0C,0x52,0x52,0x52,0x3E}, /* 0x67 g     */
    {0x7F,0x08,0x04,0x04,0x78}, /* 0x68 h     */
    {0x00,0x44,0x7D,0x40,0x00}, /* 0x69 i     */
    {0x20,0x40,0x44,0x3D,0x00}, /* 0x6A j     */
    {0x7F,0x10,0x28,0x44,0x00}, /* 0x6B k     */
    {0x00,0x41,0x7F,0x40,0x00}, /* 0x6C l     */
    {0x7C,0x04,0x18,0x04,0x78}, /* 0x6D m     */
    {0x7C,0x08,0x04,0x04,0x78}, /* 0x6E n     */
    {0x38,0x44,0x44,0x44,0x38}, /* 0x6F o     */
    {0x7C,0x14,0x14,0x14,0x08}, /* 0x70 p     */
    {0x08,0x14,0x14,0x18,0x7C}, /* 0x71 q     */
    {0x7C,0x08,0x04,0x04,0x08}, /* 0x72 r     */
    {0x48,0x54,0x54,0x54,0x20}, /* 0x73 s     */
    {0x04,0x3F,0x44,0x40,0x20}, /* 0x74 t     */
    {0x3C,0x40,0x40,0x40,0x3C}, /* 0x75 u     */
    {0x1C,0x20,0x40,0x20,0x1C}, /* 0x76 v     */
    {0x3C,0x40,0x30,0x40,0x3C}, /* 0x77 w     */
    {0x44,0x28,0x10,0x28,0x44}, /* 0x78 x     */
    {0x0C,0x50,0x50,0x50,0x3C}, /* 0x79 y     */
    {0x44,0x64,0x54,0x4C,0x44}, /* 0x7A z     */
    {0x00,0x08,0x36,0x41,0x00}, /* 0x7B {     */
    {0x00,0x00,0x7F,0x00,0x00}, /* 0x7C |     */
    {0x00,0x41,0x36,0x08,0x00}, /* 0x7D }     */
    {0x10,0x08,0x08,0x10,0x08}, /* 0x7E ~     */
};

/*---------------------------------------------------------------------------
 * Internal cursor tracking
 *--------------------------------------------------------------------------*/
static uint8_t g_currentPage = 0;
static uint8_t g_currentCol  = 0;

/*---------------------------------------------------------------------------
 * I2C helpers
 *--------------------------------------------------------------------------*/

/**
 * @brief Send a single command byte to the SSD1306.
 */
static void ssd1306_send_command(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00;  /* Co=0, D/C#=0 -> command */
    buf[1] = cmd;
    I2C_MasterSendData(&g_ssd1306I2cHandle, buf, 2, SSD1306_I2C_ADDRESS, I2C_DISABLE_SR);
}

/**
 * @brief Send a buffer of data bytes to the SSD1306 GDDRAM.
 *        The first byte is the 0x40 control byte; the rest are pixel data.
 */
static void ssd1306_send_data(uint8_t *data, uint32_t len)
{
    /* We prepend the control byte 0x40 in a small staging buffer.
     * Maximum single call: SSD1306_WIDTH + 1 = 129 bytes, so 130 is safe. */
    uint8_t buf[130];
    if(len > (sizeof(buf) - 1)) len = sizeof(buf) - 1;

    buf[0] = 0x40;  /* Co=0, D/C#=1 -> data */
    uint32_t i;
    for(i = 0; i < len; i++)
    {
        buf[i + 1] = data[i];
    }
    I2C_MasterSendData(&g_ssd1306I2cHandle, buf, len + 1, SSD1306_I2C_ADDRESS, I2C_DISABLE_SR);
}

/*---------------------------------------------------------------------------
 * GPIO and I2C peripheral configuration (private)
 *--------------------------------------------------------------------------*/

static void ssd1306_i2c_pin_config(void)
{
    GPIO_Handle_t i2c_sda, i2c_scl;

    memset(&i2c_sda, 0, sizeof(i2c_sda));
    memset(&i2c_scl, 0, sizeof(i2c_scl));

    /* SCL */
    i2c_scl.pGPIOx                            = SSD1306_I2C_GPIO_PORT;
    i2c_scl.GPIO_PinConfig.GPIO_PinNumber      = SSD1306_I2C_SCL_PIN;
    i2c_scl.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
    i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode  = 4;              /* AF4 = I2C */
    i2c_scl.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_OD;
    i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = SSD1306_I2C_PUPD;
    i2c_scl.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
    GPIO_Init(&i2c_scl);

    /* SDA */
    i2c_sda.pGPIOx                            = SSD1306_I2C_GPIO_PORT;
    i2c_sda.GPIO_PinConfig.GPIO_PinNumber      = SSD1306_I2C_SDA_PIN;
    i2c_sda.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
    i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode  = 4;
    i2c_sda.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_OD;
    i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = SSD1306_I2C_PUPD;
    i2c_sda.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
    GPIO_Init(&i2c_sda);
}

static void ssd1306_i2c_config(void)
{
    g_ssd1306I2cHandle.pI2Cx                    = SSD1306_I2C;
    g_ssd1306I2cHandle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    g_ssd1306I2cHandle.I2C_Config.I2C_SCLSpeed   = SSD1306_I2C_SPEED;
    I2C_Init(&g_ssd1306I2cHandle);
}

/*---------------------------------------------------------------------------
 * SSD1306 initialisation sequence
 *--------------------------------------------------------------------------*/

static void ssd1306_init_sequence(void)
{
    ssd1306_send_command(0xAE); /* Display OFF (sleep mode)            */

    ssd1306_send_command(0x20); /* Set Memory Addressing Mode          */
    ssd1306_send_command(0x00); /*   Horizontal Addressing Mode        */

    ssd1306_send_command(0xB0); /* Set Page Start Address (page 0)     */

    ssd1306_send_command(0xC8); /* COM Output Scan Direction: remapped */

    ssd1306_send_command(0x00); /* Low column start address            */
    ssd1306_send_command(0x10); /* High column start address           */

    ssd1306_send_command(0x40); /* Set display start line: 0           */

    ssd1306_send_command(0x81); /* Set Contrast                        */
    ssd1306_send_command(0xFF); /*   Maximum contrast                  */

    ssd1306_send_command(0xA1); /* Segment re-map: col 127 -> SEG0     */

    ssd1306_send_command(0xA6); /* Normal display (not inverted)       */

    ssd1306_send_command(0xA8); /* Set Multiplex Ratio                 */
    ssd1306_send_command(0x3F); /*   63 (64 rows)                      */

    ssd1306_send_command(0xA4); /* Output follows RAM content          */

    ssd1306_send_command(0xD3); /* Set Display Offset                  */
    ssd1306_send_command(0x00); /*   No offset                         */

    ssd1306_send_command(0xD5); /* Set Display Clock Divide Ratio      */
    ssd1306_send_command(0xF0); /*   Divide ratio=1, Fosc=maximum      */

    ssd1306_send_command(0xD9); /* Set Pre-charge Period               */
    ssd1306_send_command(0x22);

    ssd1306_send_command(0xDA); /* Set COM Pins Hardware Configuration */
    ssd1306_send_command(0x12);

    ssd1306_send_command(0xDB); /* Set VCOMH Deselect Level            */
    ssd1306_send_command(0x20);

    ssd1306_send_command(0x8D); /* Charge Pump Setting                 */
    ssd1306_send_command(0x14); /*   Enable charge pump                */

    ssd1306_send_command(0xAF); /* Display ON                          */
}

/*---------------------------------------------------------------------------
 * Public API implementation
 *--------------------------------------------------------------------------*/

void SSD1306_Init(void)
{
    /* 1. Configure GPIO pins for I2C */
    ssd1306_i2c_pin_config();

    /* 2. Configure and initialise the I2C peripheral */
    ssd1306_i2c_config();

    /* 3. Enable the I2C peripheral */
    I2C_PeripheralControl(SSD1306_I2C, ENABLE);

    /* 4. Small software delay to let the OLED power up */
    for(volatile uint32_t i = 0; i < 200000; i++);

    /* 5. Send SSD1306 initialisation commands */
    ssd1306_init_sequence();

    /* 6. Clear the display */
    SSD1306_Clear();
}


void SSD1306_Clear(void)
{
    uint8_t blank[SSD1306_WIDTH];
    memset(blank, 0x00, SSD1306_WIDTH);

    uint8_t page;
    for(page = 0; page < SSD1306_PAGES; page++)
    {
        /* Move to the start of this page */
        ssd1306_send_command(0xB0 | page);  /* Set page address */
        ssd1306_send_command(0x00);          /* Low nibble of column = 0 */
        ssd1306_send_command(0x10);          /* High nibble of column = 0 */

        /* Send 128 blank bytes */
        ssd1306_send_data(blank, SSD1306_WIDTH);
    }

    g_currentPage = 0;
    g_currentCol  = 0;
}


void SSD1306_SetCursor(uint8_t page, uint8_t col)
{
    if(page >= SSD1306_PAGES)  page = SSD1306_PAGES - 1;
    if(col  >= SSD1306_WIDTH)  col  = 0;

    g_currentPage = page;
    g_currentCol  = col;

    ssd1306_send_command(0xB0 | page);           /* Page address              */
    ssd1306_send_command(col & 0x0F);             /* Column lower nibble       */
    ssd1306_send_command(0x10 | (col >> 4));      /* Column upper nibble       */
}


void SSD1306_PrintChar(char c)
{
    /* Clamp to supported range */
    if(c < 0x20 || c > 0x7E) c = ' ';

    /* Wrap to next page if needed */
    if(g_currentCol + SSD1306_FONT_WIDTH > SSD1306_WIDTH)
    {
        g_currentPage++;
        if(g_currentPage >= SSD1306_PAGES) g_currentPage = 0;
        SSD1306_SetCursor(g_currentPage, 0);
    }

    /* 5 pixel columns from the font table */
    uint8_t glyph[6];
    uint8_t i;
    for(i = 0; i < 5; i++)
    {
        glyph[i] = font5x7[(uint8_t)(c - 0x20)][i];
    }
    glyph[5] = 0x00; /* 1-pixel spacing column */

    ssd1306_send_data(glyph, 6);
    g_currentCol += SSD1306_FONT_WIDTH;
}


void SSD1306_PrintString(const char *str)
{
    while(*str)
    {
        SSD1306_PrintChar(*str);
        str++;
    }
}


void SSD1306_DisplayTwoLines(const char *line1, const char *line2)
{
    SSD1306_Clear();

    SSD1306_SetCursor(0, 0);
    SSD1306_PrintString(line1);

    SSD1306_SetCursor(3, 0);   /* page 3 gives visible vertical separation */
    SSD1306_PrintString(line2);
}
