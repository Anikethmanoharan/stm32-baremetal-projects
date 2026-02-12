# GPIO Input Controls Onboard LED (STM32 Bare Metal)

## Objective
Control the onboard LED using GPIO input without HAL.

- PA0 configured as input
- PA5 configured as output (onboard LED)
- LED turns ON when PA0 is HIGH
- LED turns OFF when PA0 is LOW

Target board: STM32 Nucleo-F446RE.

---

## Registers Used
GPIO configured using direct register access:

| Register | Address | Purpose |
|-----------|----------|---------|
| RCC_AHB1ENR | 0x40023830 | Enable GPIOA clock |
| GPIOA_MODER | 0x40020000 | Configure pin modes |
| GPIOA_IDR | 0x40020010 | Read input state |
| GPIOA_ODR | 0x40020014 | Control LED output |

---

## Implementation Summary
1. Enable GPIOA clock via RCC.
2. Configure PA5 as output.
3. Configure PA0 as input.
4. Continuously read PA0 state.
5. Set or clear PA5 to control LED.

---

## Concepts Practiced
- Memory-mapped register access
- GPIO configuration
- Bit manipulation
- Input vs output control
- Bare-metal firmware development

---

