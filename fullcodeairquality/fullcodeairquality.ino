#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>                      
#include <SDS011.h>
#include "Adafruit_AHTX0.h"
#include <ArduinoJson.h>
#include <time.h>
#include "ThingSpeak.h"  // 🔥 Added for ThingSpeak

// ===== WiFi Credentials ===== v
const char* ssid = "leelabhavani";
const char* password = "123456789";

// ===== CTOP API Config =====
const char* ctopApi = "https://ctop.iiit.ac.in/api/nodes/create-cin/15";
const char* authToken = "fdb2dc21155ac365c38a7875e6dfddc3";

// ===== SDS011 PM Sensor =====
SoftwareSerial sdsSerial(16, 17);
SDS011 my_sds;
float pm25, pm10;
int err;

// ===== AHT10 Sensor =====
Adafruit_AHTX0 aht;

// ===== Sound Sensor (Analog) =====
#define SOUND_SENSOR_PIN 36  // GPIO36 (VP)
#define VREF 3.3
#define AVERAGE_SAMPLES 10
float voltageValue, dbValue;

// ===== LED Pins =====
#define POWER_LED     4
#define WIFI_LED      5
#define SENSOR_ERR_LED 2
+
bool sensorsOk = true;

// ===== ThingSpeak Config =====
// WiFiClient client;  // 🔥 Needed for ThingSpeak
const char* writeAPIKey = "A014BLQBL09IL9AB";  // 👈 Your write key

// ===== Average ADC Read =====
int readAverageAnalog(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / samples;
}

// ===== Blink LED =====
void blinkLed(int pin, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayMs);
    digitalWrite(pin, LOW);
    delay(delayMs);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(POWER_LED, OUTPUT);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(SENSOR_ERR_LED, OUTPUT);
  digitalWrite(POWER_LED, HIGH);
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(SENSOR_ERR_LED, LOW);

  pinMode(SOUND_SENSOR_PIN, INPUT);
  analogReadResolution(12);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED, !digitalRead(WIFI_LED));
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  digitalWrite(WIFI_LED, LOW);

  // 🔥 ThingSpeak Begin
  ThingSpeak.begin(client);

  // ==== Sensor Initialization ====
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 initialized.");

  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 sensor not found!");
    sensorsOk = false;
  }

  if (sensorsOk) {
    Serial.println("AHT10 initialized.");
  } else {
    while (true) {
      digitalWrite(SENSOR_ERR_LED, HIGH);
      delay(300);
      digitalWrite(SENSOR_ERR_LED, LOW);
      delay(300);
    }
  }

  configTime(19800, 0, "pool.ntp.org");
}

bool postToCTOP(float pm25, float pm10, float temperature, float humidity, float noise) {
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
      http.end();
      return true;
    } else {
      Serial.print("CTOP POST Failed. Error: ");
      Serial.println(httpResponseCode);
      http.end();
      return false;
    }
  } else {
    Serial.println("WiFi not connected.");
    return false;
  }
}

// 🔥🔥🔥 ThingSpeak POST Function
bool postToThingSpeak(float pm25, float pm10, float temperature, float humidity, float noise) {
  ThingSpeak.setField(1, pm25);
  ThingSpeak.setField(2, pm10);
  ThingSpeak.setField(3, temperature);
  ThingSpeak.setField(4, humidity);
  ThingSpeak.setField(5, noise);

  int x = ThingSpeak.writeFields(/* channelNumber */ 3025414, writeAPIKey);
  if (x == 200) {
    Serial.println("ThingSpeak POST Success");
    return true;
  } else {
    Serial.print("ThingSpeak POST Failed. HTTP error code: ");
    Serial.println(x);
    return false;
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Trying to reconnect...");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      for (int i = 0; i < 10; i++) {
        if (WiFi.status() == WL_CONNECTED) break;
        digitalWrite(WIFI_LED, !digitalRead(WIFI_LED));
        delay(500);
      }
    }
    Serial.println("Reconnected to WiFi.");
    digitalWrite(WIFI_LED, LOW);
  }

  err = my_sds.read(&pm25, &pm10);
  if (!err) {
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.print(" µg/m³ | PM10: ");
    Serial.println(pm10);
  } else {
    Serial.println("Failed to read SDS011.");
  }

  sensors_event_t humidityEvent, tempEvent;
  aht.getEvent(&humidityEvent, &tempEvent);
  float temperature = tempEvent.temperature;
  float humid = humidityEvent.relative_humidity;
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.println(humid);

  int rawValue = readAverageAnalog(SOUND_SENSOR_PIN, AVERAGE_SAMPLES);
  voltageValue = (rawValue / 4095.0) * VREF;
  dbValue = voltageValue * 50.0;
  Serial.print("Sound Level: ");
  Serial.print(dbValue, 1);
  Serial.println(" dBA");

  time_t now = time(nullptr);
  Serial.print("Epoch Time: ");
  Serial.println(now);

  bool sentCTOP = postToCTOP(pm25, pm10, temperature, humid, dbValue);
  if (sentCTOP) blinkLed(WIFI_LED, 1, 200);

  bool sentTS = postToThingSpeak(pm25, pm10, temperature, humid, dbValue);  // 🔥 ThingSpeak send
  if (sentTS) blinkLed(WIFI_LED, 2, 100); // Double blink on ThingSpeak success

  delay(16000); // respect ThingSpeak rate limit (15 sec+)
}
