# STM32F446RE Peripheral Drivers

A bare-metal peripheral driver library for the **STM32F446RE**, implementing GPIO, SPI, I2C, and USART
from scratch.

---

## 📁 Folder Structure

```
stm32f446re-drivers/
├── drivers/
│   ├── Inc/                             # Driver header files
│   │   ├── stm32f446xx.h               # Device-specific header (base addresses, register definitions)
│   │   ├── stm32f446xx_gpio_driver.h
│   │   ├── stm32f446xx_spi_driver.h    (coming soon)
│   │   ├── stm32f446xx_i2c_driver.h    (coming soon)
│   │   └── stm32f446xx_usart_driver.h  (coming soon)
│   └── Src/                             # Driver source / implementation files
│       ├── stm32f446xx_gpio_driver.c
│       ├── stm32f446xx_spi_driver.c    (coming soon)
│       ├── stm32f446xx_i2c_driver.c    (coming soon)
│       └── stm32f446xx_usart_driver.c  (coming soon)
│
├── applications/                        # Test applications built using the drivers above
│   ├── led-toggle-gpio/                 # LED blink using custom GPIO driver
│   │   ├── 001led_toggle.c
│   │   └── README.md
│   └── led-toggle-button/              # Button-controlled LED using custom GPIO driver
│       ├── 002led_button.c
│       └── README.md
│
└── README.md
```

---

## ✅ Progress

| Driver        | Header File                   | Source File                   | Status         |
|---------------|-------------------------------|-------------------------------|----------------|
| Device Header | `stm32f446xx.h`               | —                             | ✅ In Progress  |
| GPIO          | `stm32f446xx_gpio_driver.h`   | `stm32f446xx_gpio_driver.c`   | ✅ Done         |
| SPI           | —                             | —                             | ⏳ Not Started  |
| I2C           | —                             | —                             | ⏳ Not Started  |
| USART         | —                             | —                             | ⏳ Not Started  |

---

## 🧪 Applications

Test applications written using the drivers developed in this project.

| Application          | Driver Used | Description                                          |
|----------------------|-------------|------------------------------------------------------|
| `led-toggle-gpio`    | GPIO        | Toggles onboard LED (PA5) continuously               |
| `led-toggle-button`  | GPIO        | Toggles onboard LED (PA5) on button press (PC13)     |

---

## 📄 What's in `stm32f446xx.h`

The device-specific header file contains:

- Base addresses of Flash, SRAM1, SRAM2, ROM
- AHB1, AHB2, APB1, APB2 bus peripheral base addresses
- Peripheral register structure definitions (GPIO, RCC, SPI, I2C, USART, EXTI, SYSCFG)
- Clock enable / disable macros for all peripherals
- IRQ numbers and priority macros
- Bit position definitions for peripheral registers
- Generic utility macros (ENABLE, DISABLE, SET, RESET, etc.)

---

## 🛠️ Tools & Environment

- **Board:** STM32 Nucleo-F446RE
- **MCU:** STM32F446RET6 (ARM Cortex-M4, 180 MHz)
- **IDE:** STM32CubeIDE
- **Language:** Embedded C (bare-metal)
- **Reference:** STM32F446xx Reference Manual (RM0390)

---

*This repo is updated as I progress through the course. Files marked 🔄 are work in progress, ⏳ are coming soon.*
