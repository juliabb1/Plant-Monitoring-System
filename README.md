# Plant-Monitoring-System
I have created a Snart Plant Monitoring System with integrated watering capabilities!

In the following you will find Details about the
- System Scope
- Components
- Result

## System Scope
These are the technical and non-technical requirements:
- Watering based on moisture level​
- Watering based on User Action​
- Track environment temperature​
- Track environment humidity​
- Track Moisture​
- User friendly Dashboard​ with data analytics​
- Available System on Phone and Web​
  
## Components
### Hardware Components
- Arduino Uno R4 Wifi
- DHT11
- Capacitive Moisture Sensor V2.0
- 1 Channel 5V Relais
- 3.3V DC Water Pump

### Software Components
**Libraries**
- Adafruit DHT
- Wifi
- Blynk Simple Wifi

**Files**
- secrets.h
  - BLYNK Details
  - Wifi Details
  
- plant_monitoring_system.ino
  - Prepare Network 
    - Connect to Wifi
    - Connect to Blynk
  - Handle Sensor Data
    - Read Sensor Data
    - Read Virtual Blynk Pins
    - Send Data to Blynk
    - Send Data to Arduino
  - Smart Watering
    - Activate Pump if soil is dry
    - Activate Pump if Blynk Button was pressed
  - Logging
    - Log Data to Serial Monitor

## Result
Congrats! You have created a Smart Plant Monitoring System.
The System will read the sensor data every 2 seconds and evaluate if a plant watering should be done based on the soil moisture Value.
If the soil is too wet the water pump gets activated automatically until the threshold to dryness is reached. 
You don't have to do anything, except for letting your plant thrive in sufficient sun.
Interested User can monitor the Sensor Data over the Blynk System and manually activate the water pump if needed.
