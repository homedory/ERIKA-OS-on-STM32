# ERIKA-OS-on-STM32

## Project Overview
This project aims to study the STM32 microcontroller and the ERIKA real-time operating system. The controller sends Function Call data to an RPC (Remote Procedure Call) Server using UART. The Function Call includes a Function ID and parameters.

## Key Features
- LCD Display: Send text to be displayed.
- Motor Control: Send commands to rotate the motor by a specified degree.
- Ultrasonic Sensor: Detect objects and trigger rapid LED blinking.

The STM32 board controls peripherals using the Standard Peripheral Library (SPL).

<br>



<p align="center">
  <img src="https://github.com/user-attachments/assets/32eb62e2-f0f8-47ed-bbad-00afc340e400">
</p>

## Hardware Components

### Main Controller
* STM32F407 Board

### RPC Server
* Raspberry Pi

### Peripherals
* LCD: HD44780
* Motor: MG996R
* Ultrasonic Sensor: HC-SR04
* LED


## [TroubleShooting](https://www.notion.so/ERIKA-with-STM32-TroubleShooting-db2061828d1c404698cab6170e69e514?pvs=4)


## References
[STM32 DataSheet](https://www.st.com/resource/en/datasheet/stm32f405rg.pdf)
<br>
[ERIKA Manual](https://download.tuxfamily.org/erika/webdownload/manuals_pdf/ee_refman_1_4_5.pdf)
<br>
[WiringPi](https://github.com/WiringPi/WiringPi)
