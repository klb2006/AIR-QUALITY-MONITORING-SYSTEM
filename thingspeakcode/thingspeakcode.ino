#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"

// ===== WiFi Credentials =====
const char* ssid = "JioFi3_259D0E";
const char* password = "k61m12m0u5";



// ===== ThingSpeak Config =====
const char* writeAPIKey = "0O9MP9ZMIQL9PS5M";
const char* server = "http://api.thingspeak.com/update";  // or "https://api.thingspeak.com/update"
unsigned long channelNumber = 3025984;

// ===== SDS011 PM Sensor =====
SoftwareSerial sdsSerial(16, 17);
SDS011 my_sds;
float pm2_5 = 0.0, pm10 = 0.0;
int sdsErr = -1;

// ===== AHT10 Sensor =====
Adafruit_AHTX0 aht;
float temperature = 0.0, humidity = 0.0;

// ===== Sound Sensor =====
#define SoundSensorPin 36
#define VREF 3.3
float voltageValue = 0.0, dbValue = 0.0;

// ===== Timing Variables =====
unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // 5 seconds

// ===== Connect to WiFi =====
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

// ===== Read SDS011 =====
void readSDS() {
  sdsErr = my_sds.read(&pm2_5, &pm10);
  if (!sdsErr) {
    Serial.print("PM2.5: ");
    Serial.print(pm2_5);
    Serial.print(" µg/m³ | PM10: ");
    Serial.print(pm10);
    Serial.println(" µg/m³");
  } else {
    Serial.println("Failed to read from SDS011 sensor!");
    pm2_5 = pm10 = 0.0;
  }
}

// ===== Read AHT10 =====
void readAHT() {
  sensors_event_t humidityEvent, tempEvent;
  aht.getEvent(&humidityEvent, &tempEvent);
  if (!isnan(tempEvent.temperature) && !isnan(humidityEvent.relative_humidity)) {
    temperature = tempEvent.temperature;
    humidity = humidityEvent.relative_humidity;
    Serial.printf("Temp: %.1f °C | Humidity: %.1f%%\n", temperature, humidity);
  } else {
    Serial.println("Failed to read AHT10.");
    temperature = humidity = 0.0;
  }
}

// ===== Read Sound Sensor =====
void readSound() {
  voltageValue = analogRead(SoundSensorPin) / 4096.0 * VREF;
  dbValue = voltageValue * 50.0;
  Serial.print("Sound Level: ");
  Serial.print(dbValue, 1);
  Serial.println(" dBA");
}

// ===== Post to ThingSpeak via HTTP =====
void postToThingSpeakHTTP() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + writeAPIKey +
                 "&field1=" + String(pm2_5) +
                 "&field2=" + String(pm10) +
                 "&field3=" + String(temperature) +
                 "&field4=" + String(humidity) +
                 "&field5=" + String(dbValue);

    http.begin(url);  // Send HTTP GET request
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("ThingSpeak Response Code: ");
      Serial.println(httpCode);
      if (httpCode == 200) {
        Serial.println("ThingSpeak POST Success");
      }
    } else {
      Serial.print("HTTP POST failed: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Skipping POST.");
  }
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(SoundSensorPin, INPUT);
  analogReadResolution(12);

  connectWiFi();

  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 Sensor Initialized.");

  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 sensor not found!");
  } else {
    Serial.println("AHT10 initialized.");
  }

  delay(3000);  // Stability delay
}

// ===== Loop =====
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");
      connectWiFi();
    }

    readSDS();
    readAHT();
    readSound();
    postToThingSpeakHTTP();
  }
}