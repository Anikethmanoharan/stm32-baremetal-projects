# CEDR — Critical Event Data Recorder

A bare-metal **FreeRTOS** application for the **STM32F446RE (Nucleo)** that captures
timestamped events, persists them to external SPI flash, and shows live status on an
OLED. It behaves like a lightweight "black box": every button press and every periodic
telemetry tick is stamped with the real-time clock and written to non-volatile memory
that survives power loss.

---

## Features

- **Event capture** — logs button-press events (via EXTI interrupt) and once-per-second periodic telemetry.
- **Real timestamps** — each record carries date and time read from a **DS1307 RTC**.
- **Persistent logging** — records are appended to a **W25Q** SPI NOR flash and survive reboots.
- **Live display** — an **SSD1306** OLED shows system status and flashes each event as it happens.
- **UART log dump** — recent history is streamed over serial in a human-readable table.
- **RTOS design** — two prioritised tasks, ISR-to-task notifications, and mutex-protected shared buses.

---

## Hardware

| Peripheral      | Part        | Bus / Pins                                             | Notes |
|-----------------|-------------|--------------------------------------------------------|-------|
| MCU             | STM32F446RE | —                                                      | Nucleo-F446RE, 16 MHz core clock |
| External flash  | W25Q64      | **SPI2** — CS `PB12`, SCK `PB13`, MISO `PB14`, MOSI `PB15` (AF5) | 8 MB NOR, JEDEC ID `EF 40 17` |
| Real-time clock | DS1307      | **I2C1** — SCL `PB8`, SDA `PB9` (AF4), addr `0x68`     | Standard mode; shares the bus with the OLED |
| Display         | SSD1306     | **I2C1** — SCL `PB8`, SDA `PB9` (AF4), addr `0x3C`     | 128×64, shares the bus with the RTC |
| Serial console  | USART2      | TX `PA2`, RX `PA3` (AF7)                                | 115200 8N1 |
| User button     | —           | `PC13`, EXTI15_10                                       | Falling edge, internal pull-up |

> **Shared I2C bus:** the DS1307 and SSD1306 sit on the same I2C1 lines (PB8/PB9).
> Bus access is serialized in software with `i2cMutex`.

---

## Software architecture

Built **without CubeMX** — the FreeRTOS package was added to the project manually, and
every low-level driver (GPIO / SPI / I2C / USART / RCC) is a custom bare-metal
implementation.

### Tasks

| Task | Priority | Role |
|------|----------|------|
| `task_evnt_manager`  | 4 | Blocks on a task notification from the button ISR, timestamps and writes an event record to flash, flashes the event on the OLED for 3 s, restores the status screen, debounces, then dumps recent history over UART. |
| `task_telemetry_log` | 1 | Wakes every second, builds an `EVENT_PERIODIC_LOG` record, and appends it to flash. |

### Synchronization

- `spiMutex` — serializes all access to the shared **SPI2** bus (flash).
- `i2cMutex` — serializes all access to the shared **I2C1** bus (RTC + OLED).

### Event flow

```
Button press (PC13)
  └─ EXTI15_10 IRQ ─ clear pending bit ─ xTaskNotifyFromISR()
        └─ task_evnt_manager wakes
             ├─ read RTC (date + time)
             ├─ flash_log_write()      [spiMutex]
             ├─ OLED_DisplayEvent()    [i2cMutex]
             ├─ 3 s display, then restore status screen
             ├─ 50 ms debounce + clear pending notifications
             └─ flash_log_dump() over UART
```

---

## Flash logging design

Records are written **append-only**. Because NOR flash cannot reprogram cells that
have not been erased, the log manager scans the reserved region at boot
(`flash_log_find_next_address`) and resumes writing at the first empty (`0xFF`) slot —
so logs persist across reboots without corruption. When the region fills, it is erased
sector-by-sector and writing wraps back to the start.

| Region            | Address                     |
|-------------------|-----------------------------|
| Metadata          | `0x000000`                  |
| Log region start  | `0x010000`                  |
| Log region end    | `0x07FFFF`                  |
| Sector size       | `0x1000` (4 KB)             |

Each record (`EventRecord_t`) holds a date, a time, and an `EventType_t`
(`BUTTON_PRESS`, `PERIODIC_LOG`, plus reserved types for ADC threshold, sensor
failure, and flash error).

The W25Q driver handles the SPI details: JEDEC-ID verification on init, page-program
writes that automatically split across 256-byte page boundaries (`W25Q_Write`), and
sector / block / chip erase.

---

## Source layout

| File | Contents |
|------|----------|
| `main.c` / `main.h`               | Peripheral init (SPI, UART, RTC, OLED, flash, button), mutex + task creation, EXTI handler, scheduler start |
| `task_handler.c`                  | The two application tasks and OLED status / event rendering |
| `flash_log.c` / `flash_log.h`     | Flash log manager — init, write, read, dump, wrap/erase, persistent next-slot scan |
| `event_record.h`                  | `EventType_t` enum and `EventRecord_t` layout |
| `ds1307.c` / `ds1307.h`           | DS1307 RTC driver (BCD conversion, time/date get/set) |
| `ssd1306_oled.c` / `ssd1306_oled.h` | SSD1306 OLED driver (init sequence, 5×7 font, cursor, string printing) |
| `W25Q.c` / `W25Q.h`               | W25Q SPI NOR flash driver |
| `system.c`                        | `SystemCoreClock` definition (16 MHz) |

*(The bare-metal MCU driver layer — GPIO / SPI / I2C / USART / RCC — and the FreeRTOS
port are part of the wider project.)*

---

## Roadmap

- Add ADC sensor sampling to the telemetry task (threshold → `EVENT_ADC_THRESHOLD`).
- Extend OLED event rendering to the remaining event types.
