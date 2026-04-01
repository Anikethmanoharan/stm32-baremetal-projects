# LED Toggle with Button — GPIO Driver

A bare-metal application for the **STM32F446RE (Nucleo-F446RE)** that toggles
the onboard LED using the onboard user button, built with a custom GPIO driver
written from scratch — no HAL, no CubeMX.

---

## What This Does

Reads the onboard user button (PC13). Each time the button is pressed, the
onboard LED (PA5) toggles. A small delay handles button debouncing.

---

## Hardware

| Item         | Details                              |
|--------------|--------------------------------------|
| Board        | STM32 Nucleo-F446RE                  |
| MCU          | STM32F446RET6                        |
| LED Pin      | PA5 (onboard LD2)                    |
| Button Pin   | PC13 (onboard B1 user button)        |
| LED Config   | Output Push-Pull, No Pull            |
| Button Config| Input, Pull-Down                     |

---

## How It Works

1. **PA5** configured as Output Push-Pull for LED control
2. **PC13** configured as Input with Pull-Down resistor enabled
3. The Nucleo board has an **external pull-up** on PC13 (verified from schematic),
   so the pin reads HIGH when idle and LOW when button is pressed
4. `BTN_PRESSED` is defined as `0` (active low)
5. When button press is detected, a debounce delay runs before toggling the LED

```c
#define BTN_PRESSED 0

while(1) {
    if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == BTN_PRESSED) {
        delay();  // debounce ~250000 cycles
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    }
}
```

> **Why Pull-Down on PC13?**
> The Nucleo-F446RE schematic shows an external pull-up resistor already present
> on the button pin. However, to ensure a clean LOW signal when button is pressed,
> an internal pull-down is also configured in software.

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
---

## Tools

- **IDE:** STM32CubeIDE
- **Language:** Embedded C (bare-metal)
- **Debugger:** ST-Link (onboard Nucleo)
- **Reference:** STM32F446xx Reference Manual (RM0390)
- **Schematic:** Nucleo-F446RE board schematic (for pull-up verification)

---
