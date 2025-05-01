# 🌿 Smart Plant Monitoring & Watering System

Welcome to your own **Smart Plant Monitoring System** – an automated, IoT-enabled plant care assistant!  
This system monitors environmental conditions and waters your plant when needed — or on demand — so you can sit back and let your greenery thrive. 🌞💧

---

## 📋 Table of Contents

- [🔍 Overview](#-overview)  
- [🧠 System Features](#-system-features)  
- [🧩 Components](#-components)  
- [🏗️ System Structure](#-system-structure)  
  - [🧬 How Data Moves Through the System](#-how-data-moves-through-the-system)  
- [🚀 How It Works](#-how-it-works)  
- [🎉 Result](#-result)

---

## 🔍 Overview

This project combines **sensors, smart logic, and real-time control** to keep your plant healthy with minimal effort.  
Perfect for hobbyists, plant lovers, or anyone curious about smart home automation.

---

## 🧠 System Features

### ✅ Automated & Manual Watering
- Smart watering based on **soil moisture levels**
- Manual control via **Blynk app**

### ✅ Environmental Monitoring
- Tracks:
  - 🌡️ **Temperature**
  - 💧 **Humidity**
  - 🌱 **Soil moisture**

### ✅ Smart Dashboard
- Visual real-time data on the **Blynk app**
- Works on both **mobile and web**

---

## 🧩 Components

### 🔧 Hardware
- **Arduino Uno R4 WiFi**
- **DHT11** – Temperature & Humidity Sensor
- **Capacitive Moisture Sensor V2.0**
- **1-Channel 5V Relay**
- **3.3V DC Water Pump**

### 💻 Software

#### Libraries Used
- `Adafruit DHT`
- `WiFi`
- `Blynk Simple WiFi`

#### Project Files
- `secrets.h`  
  Contains:
  - Blynk credentials
  - WiFi SSID and password

- `plant_monitoring_system.ino`  
  Handles:
  - Network setup  
  - Sensor data collection  
  - Smart watering logic  
  - Serial logging  

---

## 🏗️ System Structure

```text
[Soil Moisture Sensor] ─┐
                       │
                 [Arduino Uno R4 WiFi] ─── [WiFi/Blynk Cloud]
                       │        │
    [DHT11 Sensor] ─────        │
                       │        └──── Manual & Remote Control
                       │
               [Relay Module] ─── [Water Pump]
```

### 🧬 How Data Moves Through the System

- **Sensor Input**  
  - Soil moisture and temperature/humidity data are read by the Arduino every 2 seconds.
  
- **Data Processing & Decision Logic**  
  - The Arduino evaluates whether watering is needed based on moisture thresholds.
  - It listens for user input from the Blynk app to allow manual watering.

- **Actuation**  
  - If conditions are met (either automatically or manually), the relay module activates the water pump.
  
- **Communication & Visualization**  
  - Sensor readings are sent to Blynk via WiFi for remote monitoring.
  - Users can view live data and toggle the water pump remotely.

---

## 🚀 How It Works

1. **Sensor readings** are refreshed every 2 seconds.
2. If the **soil is too dry**, the Arduino activates the **water pump** through the relay.
3. Users can also activate the pump manually using a **button in the Blynk app**.
4. All environmental data is continuously sent to **Blynk**, allowing real-time remote monitoring.
5. The pump **automatically stops** once the soil reaches a sufficient moisture level.

---

## 🎉 Result

Fully functional **smart watering system** that:

- 🌱 Waters plants automatically
- 📲 Supports remote manual watering
- 📊 Offers live sensor monitoring via a web & mobile dashboard

