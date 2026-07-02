# STM32 Bare-Metal Round-Robin Scheduler

A bare-metal **preemptive round-robin task scheduler** built from scratch for the **STM32F446RE (ARM Cortex-M4)**.

This project was built as a learning exercise to understand how an RTOS works internally by implementing core scheduling concepts without using FreeRTOS or any scheduler library.

---

## Features

- Preemptive round-robin scheduling
- PendSV-based context switching
- SysTick 1 ms system tick
- Separate MSP and PSP stacks
- Manual task stack initialization
- Ready and Blocked task states
- Tick-based `task_delay()`
- Idle task
- Direct register programming (no HAL scheduler)

---

## Demo

The scheduler runs three independent tasks concurrently:

| Task | LED | Blink Rate |
|------|-----|-----------:|
| Task 1 | 🔴 Red | 1000 ms |
| Task 2 | 🟢 Green | 800 ms |
| Task 3 | 🔵 Blue | 400 ms |

---

## How It Works

- **SysTick** generates a 1 ms system tick.
- Blocked tasks are moved back to the Ready state when their delay expires.
- **PendSV** performs the context switch by:
  - Saving R4–R11 of the current task
  - Selecting the next READY task
  - Restoring the next task's context
- Each task executes using its own **Process Stack Pointer (PSP)**, while exceptions use the **Main Stack Pointer (MSP)**.

---

## Task States

```text
READY ── task_delay() ──► BLOCKED
   ▲                         │
   └──── Delay Expired ──────┘
```

---

## Hardware

- **MCU:** STM32F446RE (ARM Cortex-M4)
- **Board:** Nucleo-F446RE
- **Clock:** HSI @ 16 MHz
- **Tick Rate:** 1000 Hz
- **IDE:** STM32CubeIDE

---

## Concepts Covered

- PendSV & SysTick
- Context Switching
- PSP vs MSP
- Manual Exception Stack Frames
- Task Control Blocks (TCBs)
- Critical Sections (`PRIMASK`)
- Bare-Metal Scheduling

---

## References

- ARM Cortex-M4 Generic User Guide
- ARMv7-M Architecture Reference Manual
- STM32F446 Reference Manual (RM0390)
