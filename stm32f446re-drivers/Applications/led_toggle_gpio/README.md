# LED Toggle — GPIO Driver

A bare-metal LED toggle application for the **STM32F446RE (Nucleo-F446RE)**,
built using a custom GPIO driver written from scratch — no HAL, no CubeMX.

---

## What This Does

Toggles the onboard LED (PA5) ON and OFF continuously using a software delay,
driven entirely through the custom GPIO driver API.

---

## Hardware

| Item        | Details               |
|-------------|-----------------------|
| Board       | STM32 Nucleo-F446RE   |
| MCU         | STM32F446RET6         |
| LED Pin     | PA5 (onboard LD2)     |
| Output Type | Push-Pull             |
| Pull-up/down| None                  |
| Clock Speed | Default HSI           |

---

## How It Works

1. PA5 is configured as **Output Push-Pull, Fast Speed, No Pull**
2. GPIOA peripheral clock is enabled via `GPIO_PeriClockControl()`
3. Pin is initialized via `GPIO_Init()`
4. Inside the `while(1)` loop, `GPIO_ToggleOutputPin()` flips PA5 state
5. A software delay of ~500000 cycles creates the visible blink

```c
GPIO_PeriClockControl(GPIOA, ENABLE);
GPIO_Init(&GpioLed);

while(1) {
    GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    delay();  // ~500000 cycles
}
```

---

## Driver Dependency

Uses the custom GPIO driver from:

```
stm32f446re-drivers/drivers/
├── Inc/
│   ├── stm32f446xx.h              ← device-specific header
│   └── stm32f446xx_gpio_driver.h  ← GPIO driver API
└── Src/
    └── stm32f446xx_gpio_driver.c  ← GPIO driver implementation
```

> **Note:** `stm32f446xx_gpio_driver.h` is currently included separately in `main.c`.
> Planned improvement: include it directly inside `stm32f446xx.h` so only the MCU
> header needs to be included in application files.

---

## Tools

- **IDE:** STM32CubeIDE
- **Language:** Embedded C (bare-metal)
- **Debugger:** ST-Link (onboard Nucleo)
- **Reference:** STM32F446xx Reference Manual (RM0390)

---
