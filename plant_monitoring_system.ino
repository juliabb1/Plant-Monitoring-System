/* 
  Connections:
  Soil Moisture Sensor: A0
  Relay (Pump):         D3
  DHT11 Sensor:         D2
*/

#include "secrets.h"
#include <DHT.h>
#include <WiFi.h>
#include "BlynkSimpleWifi.h"

// ================= Configuration =================
#define DHT_PIN            2
#define RELAY_PIN          3
#define SOIL_SENSOR_PIN    A0
#define DHT_TYPE           DHT11
#define SOIL_DRY_THRESHOLD 550

#define SOIL_INTERVAL      21600000L // 6 hours
#define DHT_INTERVAL       21600000L // 6 hours
#define LOG_INTERVAL       21600000L // 6 hours
#define PUMP_DURATION_MS   2000      // 2 seconds

#define VPIN_TEMP          V0
#define VPIN_HUMIDITY      V1
#define VPIN_SOIL          V2
#define VPIN_MANUAL_BUTTON V3
#define VPIN_LOG           V99

// ================= WiFi and Blynk =================
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char auth[] = BLYNK_AUTH_TOKEN;

// ================= State Variables =================
unsigned long lastSerialLogTime = 0;
const unsigned long SERIAL_LOG_INTERVAL = 2000L; // 2 seconds
int soilValue = 0;
float temperature = 0.0;
float humidity = 0.0;
int relayState = HIGH;

DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer sensorTimer;
BlynkTimer logTimer;

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  scanWiFiNetworks();
  connectToWiFi();
  connectToBlynk();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayState);

  dht.begin();

  sensorTimer.setInterval(SOIL_INTERVAL, readSoilSensor);
  sensorTimer.setInterval(DHT_INTERVAL, readDHTSensor);
  sensorTimer.setInterval(SOIL_INTERVAL, activatePumpIfNeeded);
  logTimer.setInterval(LOG_INTERVAL, sendLogToBlynk);
}

void loop() {
  Blynk.run();
  sensorTimer.run();
  logTimer.run();

  if (millis() - lastSerialLogTime >= SERIAL_LOG_INTERVAL) {
    logToSerial();
    lastSerialLogTime = millis();
  }
}

// ================= Sensor Functions =================
void readSoilSensor() {
  soilValue = analogRead(SOIL_SENSOR_PIN);
  Blynk.virtualWrite(VPIN_SOIL, soilValue);
}

void readDHTSensor() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  Blynk.virtualWrite(VPIN_TEMP, temperature);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
}

void sendLogToBlynk() {
  String msg = evaluateSoilMoisture();
  Blynk.virtualWrite(VPIN_LOG, msg + "\n");

  msg = evaluateHumidityAndTemperature();
  Blynk.virtualWrite(VPIN_LOG, msg + "\n");
}

void logToSerial() {
  Serial.println("===== 🌱 Sensor Readings (Serial Log) =====");
  Serial.println("Soil Moisture: " + String(soilValue));
  Serial.println("Temperature:   " + String(temperature) + " °C");
  Serial.println("Humidity:      " + String(humidity) + " %");
  Serial.println("Pump Status:   " + String(relayState == LOW ? "ON (watering)" : "OFF"));
  Serial.println("===========================================");
}

// ================= Pump Control =================
void activatePumpIfNeeded() {
  if (soilValue > SOIL_DRY_THRESHOLD) {
    Serial.println("🌵 Starting watering...");
    Blynk.virtualWrite(VPIN_LOG, "🌵 Starting watering...");

    digitalWrite(RELAY_PIN, LOW);  // Relay ON (active LOW)
    delay(PUMP_DURATION_MS);
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF

    Serial.println("✅ Watering finished.");
    Blynk.virtualWrite(VPIN_LOG, "✅ Watering finished.");
  }
}

// ================= Manual Pump Control via Blynk =================
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_MANUAL_BUTTON);
}

BLYNK_WRITE(VPIN_MANUAL_BUTTON) {
  relayState = param.asInt();
  digitalWrite(RELAY_PIN, relayState);

  if (relayState == LOW) {
    Blynk.virtualWrite(VPIN_LOG, "🌵 Manual watering started");
  } else {
    Blynk.virtualWrite(VPIN_LOG, "✅ Manual watering stopped");
  }
}

// ================= Evaluation Helpers =================
String evaluateSoilMoisture() {
  String msg;
  if (soilValue < 450) {
    msg = "💧 Too wet – Skip watering. Value: ";
  } else if (soilValue > SOIL_DRY_THRESHOLD) {
    msg = "🌵 Too dry – Water now! Value: ";
  } else {
    msg = "🌿 Soil moisture is optimal. Value: ";
  }
  msg += soilValue;
  Serial.println(msg);
  return msg;
}

String evaluateHumidityAndTemperature() {
  if (isnan(humidity) || isnan(temperature)) {
    return "❌ Failed to read from DHT11 sensor!";
  }
  String msg = "Humidity: ";
  msg += humidity;
  msg += "%  |  Temperature: ";
  msg += temperature;
  msg += "°C";
  Serial.println(msg);
  return msg;
}

// ================= Network Functions =================
void connectToBlynk() {
  Serial.print("⏳ Connecting to Blynk...");
  while (!Blynk.connect()) {
    Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
    delay(2000);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to Blynk!");
}

void connectToWiFi() {
  Serial.print("🔌 Connecting to WiFi: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("📡 IP Address: ");
  Serial.println(WiFi.localIP());
}

void scanWiFiNetworks() {
  Serial.println("🔍 Scanning for WiFi networks...");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(WiFi.SSID(i));
  }
}
