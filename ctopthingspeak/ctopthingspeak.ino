
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"
#include <ArduinoJson.h>
#include <time.h>

// ========== WiFi Credentials ==========
const char* ssid = "leelabhavani";
const char* password = "12345678";

// ========== CTOP Config ==========
const char* ctopApi = "https://ctop.iiit.ac.in/api/nodes/create-cin/15";
const char* authToken = "fdb2dc21155ac365c38a7875e6dfddc3";

// ========== ThingSpeak Config ==========
const char* thingspeakApiKey = "A014BLQBL09IL9AB";
  // Replace with your API Key
const char* thingspeakServer = "http://api.thingspeak.com/update";

// ========== SDS011 PM Sensor ==========
SoftwareSerial sdsSerial(16, 17);
SDS011 my_sds;
float pm25, pm10;
int err;

// ========== AHT10 Sensor ==========
Adafruit_AHTX0 aht;

// ========== Sound Sensor ==========
#define SOUND_SENSOR_PIN 36
#define VREF 3.3
#define AVERAGE_SAMPLES 10
float voltageValue, dbValue;

// ========== LED PINS ==========
#define POWER_LED 4
#define WIFI_LED 5

bool wifiInfoPrinted = false;

// ========== Average ADC ==========
int readAverageAnalog(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // LED
  pinMode(POWER_LED, OUTPUT);
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(POWER_LED, HIGH);

  // Sound
  pinMode(SOUND_SENSOR_PIN, INPUT);
  analogReadResolution(12);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED, !digitalRead(WIFI_LED));
    delay(500);
    Serial.print(".");
  }

  digitalWrite(WIFI_LED, HIGH);
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // SDS011
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 initialized.");

  // AHT10
  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 not found!");
    while (1);
  }
  Serial.println("AHT10 initialized.");

  // NTP Time
  configTime(19800, 0, "pool.ntp.org");
}

// ========== POST TO CTOP ==========
void postToCTOP(float pm25, float pm10, float temperature, float humidity, float noise) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(ctopApi);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(authToken));

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["pm2.5"] = String(pm25, 1);
    jsonDoc["pm10"] = String(pm10, 1);
    jsonDoc["humidity"] = String(humidity, 1);
    jsonDoc["temperature"] = String(temperature, 1);
    jsonDoc["noise"] = String(noise, 1);

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
      Serial.print("CTOP POST Success [");
      Serial.print(httpResponseCode);
      Serial.println("]");
      Serial.println(http.getString());
    } else {
      Serial.print("CTOP POST Failed. Error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send to CTOP.");
  }
}

// ========== POST TO THINGSPEAK ==========
void postToThingSpeak(float pm25, float pm10, float temperature, float humidity, float noise) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postData = "api_key=" + String(thingspeakApiKey) +
                      "&field1=" + String(pm25, 1) +
                      "&field2=" + String(pm10, 1) +
                      "&field3=" + String(temperature, 1) +
                      "&field4=" + String(humidity, 1) +
                      "&field5=" + String(noise, 1);

    http.begin(thingspeakServer);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      Serial.print("ThingSpeak POST Success [");
      Serial.print(httpResponseCode);
      Serial.println("]");
      Serial.println(http.getString());
    } else {
      Serial.print("ThingSpeak POST Failed. Error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send to ThingSpeak.");
  }
}

// ========== LOOP ==========
void loop() {
  // WiFi LED
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WIFI_LED, HIGH);
  } else {
    digitalWrite(WIFI_LED, !digitalRead(WIFI_LED));
    delay(500);
    return;
  }

  // SDS011
  err = my_sds.read(&pm25, &pm10);
  if (!err) {
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.print(" µg/m³ | PM10: ");
    Serial.println(pm10);
  }

  // AHT10
  sensors_event_t humidityEvent, tempEvent;
  aht.getEvent(&humidityEvent, &tempEvent);
  float temperature = tempEvent.temperature;
  float humid = humidityEvent.relative_humidity;
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.println(humid);

  // Sound Sensor
  int rawValue = readAverageAnalog(SOUND_SENSOR_PIN, AVERAGE_SAMPLES);
  voltageValue = (rawValue / 4095.0) * VREF;
  dbValue = voltageValue * 50.0;
  Serial.print("Sound Level: ");
  Serial.print(dbValue, 1);
  Serial.println(" dBA");

  // Time
  time_t now = time(nullptr);
  Serial.print("Epoch Time: ");
  Serial.println(now);

  // Post to CTOP
  postToCTOP(pm25, pm10, temperature, humid, dbValue);

  // Post to ThingSpeak
  postToThingSpeak(pm25, pm10, temperature, humid, dbValue);

  delay(15000); // Wait before next update
}
