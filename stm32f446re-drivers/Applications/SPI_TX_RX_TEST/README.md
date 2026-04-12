# SPI TX/RX Test (STM32F446RE)

This application demonstrates **SPI transmission (TX)** using a custom bare-metal SPI driver.
It is structured to later include **SPI receive (RX)** functionality.

---

## 🎯 Objective

* Verify SPI driver transmit functionality
* Prepare structure for TX + RX validation
* Understand GPIO alternate function configuration for SPI

---

## 🔌 Hardware Configuration

SPI2 pins are configured as follows:

| Signal | Pin  | Description                 |
| ------ | ---- | --------------------------- |
| SCK    | PB13 | Serial Clock                |
| MOSI   | PB15 | Master Out Slave In         |
| MISO   | PB14 | Not used (yet)              |
| NSS    | PB12 | Not used (Software managed) |

**Alternate Function Mode:** AF5

---

## ⚙️ SPI Configuration

* Mode: Master
* Bus Config: Full Duplex
* Clock Speed: fPCLK / 2 (≈ 8 MHz)
* Data Frame Format: 8-bit
* CPOL: Low
* CPHA: Low
* NSS Management: Software (SSM enabled)

---

## 🚀 Execution Flow

1. Configure GPIO pins for SPI functionality
2. Initialize SPI2 peripheral
3. Enable SPI (SPE bit = 1)
4. Transmit data using polling

```c
SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));
```

---

## 📤 Data Sent

```c
"Hello World"
```

---

## 🧠 Key Learning Points

* GPIO Alternate Function configuration for SPI
* SPI peripheral initialization using registers
* Polling mechanism using **TXE flag**
* Importance of **SPE bit** for enabling SPI
* Software Slave Management (SSM)

---

## ⚠️ Notes

* Current implementation supports **TX only**
* RX functionality will be added next
* MISO and NSS are not used in this stage
* No slave device is connected (open-loop transmission)

---

## 💬 Developer Note

This is a **driver validation test module** focused on understanding
**register-level SPI communication** without using HAL.
