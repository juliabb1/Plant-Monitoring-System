/* 
  Connections:
  Soil Moisture Sensor: A0
  Relay (Pump):         D3
  DHT11 Sensor:         D2
*/

#include "secrets.h"
#include "webpage.h"
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ================= Configuration =================
#define DHT_PIN            2
#define RELAY_PIN          3
#define SOIL_SENSOR_PIN    0
#define DHT_TYPE           DHT11
#define SOIL_DRY_THRESHOLD 350

#define PUMP_DURATION_MS   2000      // 2 seconds

#define VPIN_TEMP          V0
#define VPIN_HUMIDITY      V1
#define VPIN_SOIL          V2
#define VPIN_MANUAL_BUTTON V3
#define VPIN_LOG           V99

// ================= WiFi & MQTT =================
WiFiServer server(8081);
WiFiClient mqttWiFiClient;
PubSubClient mqttClient(mqttWiFiClient);

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

// MQTT Configuration
const char* mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;
const char* mqtt_client_id = "plant_monitor";
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_pass = MQTT_PASS;

// MQTT Topics
//  for publishing
const char* topic_temp = "plant/temperature";
const char* topic_humidity = "plant/humidity";
const char* topic_soil = "plant/soil";
const char* topic_status = "plant/status";
const char* topic_pump = "plant/pump";
// for subscription
const char* topic_pump_sub = "plant/pump/set";

// ================= State Variables =================
unsigned long lastSerialLogTime = 0;
unsigned long lastMqttPublishTime = 0;

const unsigned long SERIAL_LOG_INTERVAL = 5000L; // 5Sek
const unsigned long MQTT_PUBLISH_INTERVAL = 5000L; // 10 seconds

int soilValue = 0;
float temperature = 0.0;
float humidity = 0.0;
int relayState = HIGH;
bool pumpActive = false;

DHT dht(DHT_PIN, DHT_TYPE);


// ================= Setup =================
void setup() {
  Serial.begin(9600);
  scanWiFiNetworks();
  connectToWiFi();
  setupMQTT();
  prepareWebServer();


  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayState);

  dht.begin();
}

void loop() {
   // call poll() regularly to allow the library to send MQTT keep alive -> avoids being disconnected by the broker
  if (!mqttClient.connected()){
    reconnectMQTT();
  }
  mqttClient.loop();
  refreshWebServer();

  // Read Sensor Data and Log to Serial Monitor
  if (millis() - lastSerialLogTime >= SERIAL_LOG_INTERVAL) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    soilValue = analogRead(SOIL_SENSOR_PIN);
    logToSerial();
    activatePumpIfNeeded();
    lastSerialLogTime = millis();
  }

    // Publish sensor data to MQTT
  if (millis() - lastMqttPublishTime >= MQTT_PUBLISH_INTERVAL) {
    publishSensorData();
    lastMqttPublishTime = millis();
  }
}


// ================= Serial Log Function  =================
void logToSerial() {
  Serial.println("===== 🌱 Sensor Readings (Serial Log) =====");
  Serial.println("Soil Moisture: " + String(soilValue));
  Serial.println("Temperature:   " + String(temperature) + " °C");
  Serial.println("Humidity:      " + String(humidity) + " %");
  evaluateSoilMoisture();
  Serial.println("===========================================");
}

// ================= Evaluation Helpers =================
String evaluateSoilMoisture() {
  String msg;
  if (soilValue < SOIL_DRY_THRESHOLD - 30) {
    msg = "💧 Too wet – Skip watering";
  } else if (soilValue > SOIL_DRY_THRESHOLD) {
    msg = "🌵 Too dry – Water now!";
  } else {
    msg = "🌿 Soil moisture is optimal";
  }
  Serial.println(msg);
  return msg;
}

// ================= Pump Control =================
void activatePumpIfNeeded() {
  if (soilValue > SOIL_DRY_THRESHOLD){ 
    activatePump();
    delay(PUMP_DURATION_MS);
    closePump();
  }
}

void activatePump(){
    Serial.println("🌵 Starting watering...");
    digitalWrite(RELAY_PIN, LOW);  // Relay ON (active LOW)
    pumpActive = true;

    // Publish pump status to MQTT
    if (mqttClient.connected()) {
      mqttClient.publish(topic_pump, "ON");
    }
}

void closePump() {
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF
    Serial.println("✅ Watering finished.");
    pumpActive = false;

    // Publish pump status to MQTT
    if (mqttClient.connected()) {
      mqttClient.publish(topic_pump, "OFF");
    }
}

// ================= Network Functions =================
void publishSensorData() {
  mqttClient.publish(topic_temp, String(temperature, 1).c_str());
  mqttClient.publish(topic_humidity, String(humidity, 1).c_str());
  mqttClient.publish(topic_soil, String(soilValue).c_str());
  mqttClient.publish(topic_status, evaluateSoilMoisture().c_str());
}


/**
 * callback
 */
void callbackMQTT(char* topic, byte* payload, unsigned int length)
{
  // Callback-Funktion für MQTT-Subscriptions
  String messagePump;

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
 
  // Message byteweise verarbeiten und auf Konsole ausgeben
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    messagePump += (char)payload[i];
  }
  Serial.println();
  // Behandlung bestimmter Topics
  if (String(topic) == topic_pump_sub) {
    if (messagePump == "ON") {
      Serial.print(topic_pump_sub);
      activatePump();
    }
    else if (messagePump == "OFF") {
      Serial.print(topic_pump_sub);
      closePump();
    }
  }
  /* else if (topic == anderes_topic) { ... } */
}


void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callbackMQTT);
  Serial.println("🔧 MQTT wifiClient configured on ");
  Serial.print(mqtt_server);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("📡 Attempting MQTT connection...");
    
    if (mqttClient.connect(mqtt_client_id, mqtt_username, mqtt_pass)) {
      Serial.println(" connected!");
      // Publish initial status
      mqttClient.subscribe(topic_pump_sub);
      mqttClient.publish(topic_status, "🌱 Plant monitor connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void connectToWiFi() {
  Serial.print("🔌 Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  // Warte bis verbunden
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
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

// ================= Create Webserver Functions =================
void prepareWebServer() {
  server.begin();     // start the web server on port 80
  Serial.println("✅ Started Web Server on Port 8080!");
}


void refreshWebServer() {
  WiFiClient client = server.available();  // ✅ use a local variable
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.startsWith("GET ")) {
          request = line;
        }
        if (line == "\r") break;
      }
    }

    Serial.print("<< ");
    Serial.println(request);

    String buttonLabel;
    // Handle the /water request
    if (request.indexOf("GET /water") >= 0) {
      if (pumpActive) {
        closePump();
      } else {
        activatePump();
      }
      buttonLabel = pumpActive ? "❌ Stop" : "💦 Water";
      String json = "{";
      json += "\"buttonLabel\":\"" + buttonLabel + "\"";
      json += "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/plain");
      client.println("Connection: close");
      client.println();
      client.println(json);

    } else if (request.indexOf("GET /data") >= 0) {
      String json = "{";
      json += "\"temperature\":\"" + String(temperature) + " °C\",";  // Added quotes around the value
      json += "\"humidity\":\"" + String(humidity) + " %\",";  // Added quotes around the value
      json += "\"soil\":" + String(soilValue) + ",";  // No quotes needed for numerical values
      json += "\"status\":\"" + evaluateSoilMoisture() + "\"";  // Added quotes around the value
      json += "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-type: application/json");
      client.println("Connection: close");
      client.println();
      client.println(json);
    } else {
      // Serve the main HTML page
      String htmlPage = HTML_CONTENT;
      buttonLabel = pumpActive ? "❌ Stop" : "💦 Water";
      htmlPage.replace("{{TEMP}}", String(temperature) + " °C");
      htmlPage.replace("{{HUMIDITY}}", String(humidity) + " %");
      htmlPage.replace("{{SOIL}}", String(soilValue));
      htmlPage.replace("{{STATUS}}", evaluateSoilMoisture());
      htmlPage.replace("{{BUTTON_LABEL}}", buttonLabel);

      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();

      client.print(htmlPage);
    }

    client.flush();
    delay(10);
    client.stop();
  }
}

