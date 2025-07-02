# Product-Design-and-Development-Project-Motor-Controller-Device-

This project is a compact and smart DC motor controller built around the **ATmega328P (SMD package)** microcontroller. It is powered by a **3.7V Li-ion rechargeable battery** and features a **USB-C charging module (TP4056)**, offering portable and efficient motor control functionality.

The firmware was developed in C and programmed directly to the microcontroller using a **USBasp programmer** via the ISP interface — no Arduino bootloader required. This low-level approach provides maximum control over the microcontroller's resources and allows for optimization of power usage and memory.

### 🔧 Control & Features:
- **Push Button A**: Start/Stop the motor
- **Push Button B**: Select motor mode (cycle through 5 modes)
- **Motor Modes**:
  1. Full Speed
  2. Half Speed
  3. Low Speed
  4. 1-Second Pulse
  5. Manual (motor runs only while Button A is pressed)

### 💡 LED Feedback:
- **Mode Selection**: LEDs 8–12 indicate the selected speed/mode
- **Motor Activity**: LEDs 1–6 animate based on selected mode
- **Battery Status**: LEDs 13–15 indicate charge level
- **Startup Animation**: LEDs 17 & 18 show a logo sequence on power-up
- **Manual Mode Indicator**: LED 7 pulses when motor is running manually

### 📐 PCB:
- **Dimensions**: 2 cm × 16 cm
- **Microcontroller**: ATmega328P (TQFP-32 SMD)
- **Programmed via**: USBasp Programmer (AVR ISP)
- **Charging**: USB-C via TP4056

This project was custom-built for a client in Australia, combining efficient hardware layout, precise speed control, and intuitive LED feedback in a slim, compact PCB design.

<p align="center"> <img src="https://github.com/SujeewBandara/Product-Design-and-Development-Project-Motor-Controller-Device-/blob/main/Captureklhb.JPG"/>

<p align="center"> <img src="https://github.com/SujeewBandara/Product-Design-and-Development-Project-Motor-Controller-Device-/blob/main/motor_controller_2024-Sep-15_07-09-39PM-000_CustomizedView45209946325.jpg"/>

<p align="center"> <img src="https://github.com/SujeewBandara/Product-Design-and-Development-Project-Motor-Controller-Device-/blob/main/real_1.jpg"/>

<p align="center"> <img src="https://github.com/SujeewBandara/Product-Design-and-Development-Project-Motor-Controller-Device-/blob/main/WhatsApp%20Image%202024-10-07%20at%2022.39.37_8f383d1f.jpg"/>

