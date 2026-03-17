# STM32 Bare-Metal Round-Robin Task Scheduler

A bare-metal **round-robin task scheduler** built from scratch for the **STM32F4** series (tested on STM32F446RE / Nucleo-F446RE). No RTOS. No HAL scheduler. Just registers, assembly, and C.

This is a learning project — built step by step to deeply understand how an OS scheduler actually works under the hood.

---

## What This Does

- Runs **4 independent tasks** concurrently using context switching
- Uses the **SysTick timer** to generate a 1ms tick interrupt
- On every tick, saves the current task's CPU context (registers R4–R11 + hardware-saved stack frame) and restores the next task's context
- Each task gets its own **1KB stack** carved out of SRAM
- Uses **PSP (Process Stack Pointer)** for tasks and **MSP (Main Stack Pointer)** for the scheduler/handler
- **No external libraries** — everything done via direct register access

---

## Memory Layout

```
SRAM (128KB): 0x20000000 → 0x20020000
                                         ← SRAM_END (0x20020000)
┌─────────────────┐  0x20020000
│   Task 1 Stack  │  1KB  (T1_STACK_START = SRAM_END)
├─────────────────┤  0x2001FC00
│   Task 2 Stack  │  1KB
├─────────────────┤  0x2001F800
│   Task 3 Stack  │  1KB
├─────────────────┤  0x2001F400
│   Task 4 Stack  │  1KB
├─────────────────┤  0x2001F000
│ Scheduler Stack │  1KB  (SCHED_START_STACK)
├─────────────────┤  0x2001EC00
│   (free SRAM)   │
└─────────────────┘  0x20000000
```

Stacks grow **downward** (full-descending). Each task's stack start is the top (highest address).

---

## How Context Switching Works

### The Stack Frame

When SysTick fires, the ARM Cortex-M hardware **automatically** pushes 8 registers onto the current task's PSP stack (SF1):

```
  [PSP before exception]
  xPSR
  PC      ← return address (task resumes here)
  LR
  R12
  R3
  R2
  R1
  R0
  [PSP after exception] ← hardware puts PSP here
```

The scheduler manually saves the remaining registers R4–R11 (SF2) using `STMDB`.

### SysTick_Handler Flow

```
1. MRS R0, PSP           → get current task's PSP
2. STMDB R0!, {R4-R11}   → push R4-R11 onto task stack (SF2)
3. BL save_psp_value      → save updated PSP into psp_of_tasks[current_task]
4. BL update_next_task    → current_task = (current_task + 1) % MAX_TASKS
5. BL get_psp_value       → load next task's saved PSP into R0
6. LDMIA R0!, {R4-R11}   → restore R4-R11 from next task's stack
7. MSR PSP, R0            → update PSP to next task's stack
8. BX LR                  → return; hardware auto-restores SF1 (R0-R3, R12, LR, PC, xPSR)
```

### Dummy Stack Frame (init_tasks_stack)

Before the scheduler starts, each task needs a fake initial stack frame so the first context restore works correctly:

```
Stack frame initialized for each task (top → bottom):
  DUMMY_XPSR (0x01000000) — Thumb bit set
  PC         → task function address
  LR         → 0xFFFFFFFD (return to Thread mode, use PSP)
  R12        → 0x00000000
  R3-R0      → 0x00000000
  R11-R4     → 0x00000000  (SF2, manually restored by scheduler)
```

---

## Key Concepts Covered

| Concept | Where |
|---|---|
| SysTick timer configuration | `init_systick_timer()` |
| MSP vs PSP stack separation | `init_scheduler_stack()`, `switch_sp_to_psp()` |
| Naked functions + inline assembly | `SysTick_Handler`, `switch_sp_to_psp` |
| CONTROL register (bit 1 = SPSEL) | `switch_sp_to_psp()` |
| EXC_RETURN value 0xFFFFFFFD | `init_tasks_stack()` |
| Hardware vs software saved registers | `SysTick_Handler` |
| Fault handler enabling (SHCSR) | `enable_processor_faults()` |

---

## Hardware

| Item | Value |
|---|---|
| MCU | STM32F446RE (Cortex-M4) |
| Board | Nucleo-F446RE |
| Clock | HSI @ 16 MHz |
| Tick rate | 1000 Hz (1ms per tick) |
| IDE | STM32CubeIDE |

---

## Planned Additions (WIP)
- Toggling of LEDs using multiple tasks
- Blocking states of tasks
- Blocking a task for given number of ticks
- Deciding next task to run
- Implementing pendSV handler for context switch


---

## References

- ARM Cortex-M4 Technical Reference Manual
- ARMv7-M Architecture Reference Manual
- STM32F446xx Reference Manual (RM0390)
- [FastBit Embedded Brain Academy — STM32Fx Microcontroller Custom Bootloader Development](https://www.udemy.com/course/stm32f4-arm-cortex-mx-custom-bootloader-development/)
