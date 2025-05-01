/* Connections
Soil.    A0
RELAIS.  3
Temp.    2
*/
#include "secrets.h"

// Packages
#include <DHT.h>
#include <WiFi.h>
#include "BlynkSimpleWifi.h"

// WIFI
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char auth[] = BLYNK_AUTH_TOKEN;
int status = WL_IDLE_STATUS;

// DHT
#define DHT11_PIN 2
DHT dht11(DHT11_PIN, DHT11);
BlynkTimer sensorTimer;
BlynkTimer serialTimer;  // Logging every 2 seconds


// Global Variables
const int moisterSensorPin = A0;  // Sensor an A0
const int relaisPin = 3;          // Relais (IN)
const int trockenGrenze = 550;
int moisterSensorValue;
float humidity;
float temp;

void checkPhysicalButton();
int relay1State = HIGH;  // AUS
int pushButton1State = HIGH;
#define VPIN_BUTTON_1 V3


void setup() {
  // Prepare Network
  Serial.begin(9600);
  scanNetworks();
  connectToWifi();
  connectToBlynk();

  // Prepare Sensors
  pinMode(relaisPin, OUTPUT);
  digitalWrite(relaisPin, HIGH);  // Relais AUS
  dht11.begin();                  // initialize the sensor


  // Call the function
  sensorTimer.setInterval(2000L, soilMoistureSensor);
  sensorTimer.setInterval(2000, DHT11sensor);
  sensorTimer.setInterval(500L, checkPhysicalButton);
  sensorTimer.setInterval(500L, activatePump);
  serialTimer.setInterval(2000L, logSensorData);  // New timer for serial logging
}

void loop() {
  // read moister, temp and humidity
  moisterSensorValue = analogRead(moisterSensorPin);
  temp = dht11.readTemperature();
  humidity = dht11.readHumidity();

  // Run Blynk Library and timers
  Blynk.run();  //Run the Blynk library
  sensorTimer.run();
  serialTimer.run();
}

void logSensorData() {
  String msg;
  msg = checkSoilMoisture();
  Blynk.virtualWrite(V99, msg + "\n");

  msg = checkHumidityAndTemp();
  Blynk.virtualWrite(V99, msg + "\n");
}

void soilMoistureSensor() {
  Blynk.virtualWrite(V2, moisterSensorValue);
}

void DHT11sensor() {
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humidity);
}

void checkPhysicalButton() {
  // Update Button Widget
  Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  if (relay1State == 0) {
    Blynk.virtualWrite(V99, "🌵 Gießen startet!");
  } else {
    Blynk.virtualWrite(V99, "✅ Gießen beendet.");
  }
  digitalWrite(relaisPin, relay1State);
  
}


void connectToBlynk() {
  Serial.print("Trying to connect to Blynk Cloud");

  bool connection_status = false;
  while (connection_status == false) {
    Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
    connection_status = Blynk.connect();
    delay(2000);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to Blynk!");
}


void connectToWifi() {
  String msg = "🔌 Trying to connect to " + String(ssid);
  Serial.println(msg);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(2000);
    Serial.print(".");
  }

  msg = "\n✅ Connected to WiFi!\n";
  msg += "📡 IP Address: " + WiFi.localIP().toString() + "\n";
  msg += String("📶 SSID: ") + WiFi.SSID() + "\n";
  msg += String("📏 Signal Strength (RSSI): ") + String(WiFi.RSSI()) + " dBm";

  Serial.println(msg);
}


void scanNetworks() {
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  String msg = "SSID List: ";
  msg += numSsid;
  Serial.println(msg);

  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    String netMsg = String(thisNet) + ") Network: " + WiFi.SSID(thisNet);
    Serial.println(netMsg);
  }
}

// Funktion zur Auswertung der Bodenfeuchte
String checkSoilMoisture() {
  String msg;

  if (moisterSensorValue < 450) {
    msg = "💧 Zu nass – Gießen aussetzen. Wert: ";
  } else if (moisterSensorValue > trockenGrenze) {
    msg = "🌵 Zu trocken – Gießen! Wert: ";
  } else if (moisterSensorValue >= 450 && moisterSensorValue <= trockenGrenze) {
    msg = "🌿 Feuchtigkeit optimal für Basilikum. Wert: ";
  } else {
    msg = "🤔 Übergangsbereich – Beobachten. Wert: ";
  }

  msg += moisterSensorValue;
  Serial.println(msg);
  return msg;
}


String checkHumidityAndTemp() {
  String msg;

  if (isnan(humidity) || isnan(temp)) {
    msg = "❌ Failed to read from DHT11 sensor!";
  } else {
    msg = "Humidity: ";
    msg += humidity;
    msg += "%  |  Temperature: ";
    msg += temp;
    msg += "°C ~\n";
  }

  Serial.println(msg);
  return msg;
}

void activatePump() {
  if (moisterSensorValue > trockenGrenze) {
    Serial.println("🌵 Gießen startet!");
    Blynk.virtualWrite(V99, "🌵 Gießen startet!");
    digitalWrite(relaisPin, LOW);   // Relais EIN (aktiv LOW)
    delay(3000);                    // 5 Sekunden gießen
    digitalWrite(relaisPin, HIGH);  // Relais AUS
    Serial.println("✅ Gießen beendet.");
    Blynk.virtualWrite(V99, "✅ Gießen beendet.");
  }
}
