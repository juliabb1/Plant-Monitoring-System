# Smart Plant Monitoring & Watering System

I have created a Smart Plant Monitoring System that monitors environmental conditions and waters your plant when needed or on demand so you can sit back and let your greenery thrive.

![Slide18](https://github.com/user-attachments/assets/248c18fa-0188-4486-ae86-0db33ec3223e)

---

## Table of Contents

- [Overview](#-overview)  
- [System Features](#-system-features)  
- [Components](#-components)  
- [System Structure](#-system-structure)  
  - [How Data Moves Through the System](#-how-data-moves-through-the-system)  
- [How It Works](#-how-it-works)  
- [Result](#-result)  
- [Website Interface](#-website-interface)

---

## Overview

This project combines **sensors, smart logic, and real-time control** to keep your plant healthy with minimal effort.  

---

## System Features

### Automated & Manual Watering
- Smart watering based on **soil moisture levels**
- Manual control via **an external app** or **local website**
- Device communication over **MQTT**

### Environmental Monitoring
- Tracks:
  -  **Temperature**
  -  **Humidity**
  -  **Soil moisture**
  -  **Pump State**

### Smart Dashboard
- Visual real-time data on the **external app**, I used Home Assistant, and **local website**
- Works on both **mobile and web**, with a responsive interface for easy use on phones

---

## Components

### Hardware
- **Arduino Uno R4 WiFi**
- **DHT11** – Temperature & Humidity Sensor
- **Capacitive Moisture Sensor V2.0**
- **1-Channel 3.3V Relay**
- **3.3V DC Water Pump**

### Software

#### Libraries Used
- `Adafruit DHT`
- `WiFi`

#### Project Files
- `secrets.h`  
  - Home Assistant credentials
  - WiFi SSID and password

- `plant_monitoring_system.ino`  
  - Network setup  
  - Sensor data collection  
  - Smart watering logic 
  - Serial logging  

---

## System Structure

```text
[Soil Moisture Sensor] ─┐
                       │
                 [Arduino Uno R4 WiFi] ─── [WiFi]
                       │        │
    [DHT11 Sensor] ─────        │
                       │        └──── Manual & Remote Control
                       │
               [Relay Module] ─── [Water Pump]
```

---

## How Data Moves Through the System

- **Sensor Input**  
  - Soil moisture and temperature/humidity data are read by the Arduino every 5 seconds.
  
- **Data Processing & Decision Logic**  
  - The Arduino evaluates whether watering is needed based on moisture thresholds.
  - It listens for user input from the external app app or the local website to allow manual watering.

- **Actuation**  
  - If conditions are met (either automatically or manually), the relay module activates the water pump.
  
- **Communication & Visualization**  
  - Sensor readings are sent to external app via WiFi for remote monitoring.
  - Users can view live data and toggle the water pump remotely via the external app  or local website.
  
- **Website Interface**  
  - The website hosted by the **Arduino Uno R4 WiFi** operates independently of the external app, providing an alternative way to monitor and control the system.
  - It offers real-time sensor data, as well as the ability to manually activate the watering system.
  - The site is **responsive**, ensuring a good user experience on both mobile and desktop devices.

---

## How It Works

1. **Sensor Readings**:  
   - The soil moisture and temperature/humidity sensors are read by the Arduino every 2 seconds to gather up-to-date environmental data.

2. **Automatic Watering**:  
   - If the soil moisture is below the preset threshold, the Arduino activates the **water pump** through the relay module to water the plant.

3. **Manual Watering Control**:  
   - Users can activate the water pump manually via the **external app** or the **local website** by pressing the water button.

4. **Continuous Monitoring**:  
   - Sensor data is continuously sent to **external app** for remote monitoring, and the **website** refreshes every 5 seconds to display the latest data on temperature, humidity, soil moisture, and status.

5. **Watering Status**:  
   - The system stops watering when the soil moisture is above the threshold, ensuring the plant doesn’t overwater.

6. **Local Website Features**:  
   - The local website, hosted by the **Arduino Uno R4 WiFi**, provides a **real-time** view of the environmental data.
   - It is **responsive**, adjusting to different screen sizes, so users can access the monitoring system from any device, including smartphones and desktops.
   - The website offers full control over watering and provides up-to-date data without needing a mobile app.

---

## Result

The **Smart Plant Monitoring & Watering System** provides a fully functional automated plant care solution with:

- **Automatic Plant Watering**: The system waters your plant when needed, based on soil moisture levels.
- **Manual Control**: Watering can be triggered manually through the **external app** or **local website**.
- **Real-Time Monitoring**: View live data on temperature, humidity, and soil moisture from the **external app** or **local website**.
- **Responsive Website**: The website is mobile-friendly, offering a seamless experience on smartphones, tablets, and desktops.
- **Local Web Interface**: The Arduino Uno R4 WiFi hosts a **self-contained website**, allowing you to monitor and control the system without needing the external app.

This system ensures your plant gets the right amount of water at the right time, all while providing real-time data and control at your fingertips.
