# The Unwearable Fitbit

> A DIY smartwatch built from scratch on a Cortex-M4 microcontroller — because why buy a Fitbit when you can build one?

**Final Project — Embedded Systems Design**

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-STM32F411-orange.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f411.html)
[![Demo](https://img.shields.io/badge/demo-video-red.svg)](https://drive.google.com/file/d/1EiO3yBaUMoaBZPptObyAyf9rLdIWgki8/view?usp=sharing)

---

## Demo

[▶ Watch the demo video](https://drive.google.com/file/d/1EiO3yBaUMoaBZPptObyAyf9rLdIWgki8/view?usp=sharing)

---

## Overview

The Unwearable Fitbit is a bare-metal embedded smartwatch implemented on the **STM32F411VE** (ARM Cortex-M4). It integrates real-time heart rate monitoring, joystick navigation, an LCD display, Bluetooth messaging, and on-device ML-based workout classification — all written in C without an RTOS.

The name? A tongue-in-cheek nod to how "wearable" it ended up being. The engineering, though, is very real.

---

## Features

- **Heart Rate Monitoring** — Real-time BPM readings via MAX30102 pulse oximeter sensor over I2C
- **LCD Display** — Live UI rendered on an SPI-driven color LCD with joystick navigation
- **Bluetooth Messaging** — Send and receive text messages via HC-05 Bluetooth module over UART
- **Workout Classification** — On-device ML inference using STM32 X-CUBE-AI to detect workout type from sensor data
- **Custom Peripheral Drivers** — Hand-written drivers for SPI, I2C, and UART; no HAL shortcuts

---

## Hardware

| Component | Module | Interface |
|---|---|---|
| Microcontroller | STM32F411VE (Cortex-M4 @ 100 MHz) | — |
| Display | Color LCD | SPI |
| Heart Rate Sensor | MAX30102 | I2C |
| Navigation | Analog Joystick | ADC |
| Bluetooth | HC-05 | UART |


## ML Inference

Workout classification runs entirely on-device using **STM32 X-CUBE-AI**. Sensor data is fed into a neural network model at inference time to detect the type of physical activity — no cloud, no latency.

---

##  Team

Daphne Felt - * *
Shruti Kalyankar – *Shruti.Kalyankar@colorado.edu*
---


