#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"
#include "time.h"

// ====================== Wi-Fi Configuration ======================
const char* ssid = "leelabhavani";
const char* password = "12345678";

// ====================== OM2M Configuration ======================
const char* om2mServer = "http:// 10.2.143.103:8080/~/in-cse/cnt-496337131";
const char* om2mAe = "admin:admin";

// ====================== LED Pin Setup ============================
#define POWER_LED 4  // Red Power LED
#define WIFI_LED  5  // Blue WiFi/Data LED

// ====================== Sound Sensor Setup =======================
#define SoundSensorPin 36  // GPIO 36 (VP)
#define VREF 3.3
float voltageValue, dbValue;

// ====================== SDS011 Sensor Setup ======================
SoftwareSerial sdsSerial(16, 17);  // RX = 16, TX = 17
SDS011 my_sds;
float pm25, pm10;
int err;

// ====================== AHT10 Setup ==============================
Adafruit_AHTX0 aht;

// ====================== Time Setup ===============================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(POWER_LED, OUTPUT);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(SoundSensorPin, INPUT);

  digitalWrite(POWER_LED, HIGH);  // Power ON indicator

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED, HIGH);
    delay(300);
    digitalWrite(WIFI_LED, LOW);
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  // Blink Blue LED 1s to show connected
  digitalWrite(WIFI_LED, HIGH);
  delay(1000);
  digitalWrite(WIFI_LED, LOW);

  // Time setup
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // SDS011 init
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 initialized.");

  // AHT10 init
  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 not found!");
    while (1);
  }
  Serial.println("AHT10 initialized.");
}

String getEpochTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "0";
  }
  time(&now);
  return String(now);
}

float readNoiseDBA() {
  voltageValue = analogRead(SoundSensorPin) / 4096.0 * VREF;
  dbValue = voltageValue * 50.0;
  Serial.print("Sound Level: ");
  Serial.print(dbValue, 1);
  Serial.println(" dBA");
  return dbValue;
}

void postToOM2M(String content) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  HTTPClient http;
  http.begin(om2mServer);
  http.addHeader("X-M2M-Origin", om2mAe);
  http.addHeader("Content-Type", "application/json;ty=4");

  String body =
    "{"
    "\"m2m:cin\": {"
    "\"con\": \"" + content + "\","
    "\"cnf\": \"text\""
    "}"
    "}";

  int code = http.POST(body);
  if (code > 0) {
    Serial.print("POST Success. Code: ");
    Serial.println(code);
    digitalWrite(WIFI_LED, HIGH); delay(200); digitalWrite(WIFI_LED, LOW);
  } else {
    Serial.print("POST Failed. Error: ");
    Serial.println(code);
  }

  http.end();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED, HIGH);
    delay(300);
    digitalWrite(WIFI_LED, LOW);
    delay(300);
    Serial.println("WiFi Disconnected...");
    return;
  }

  // Read SDS011
  err = my_sds.read(&pm25, &pm10);
  if (!err) {
    Serial.print("PM2.5: "); Serial.print(pm25);
    Serial.print(" µg/m³ | PM10: "); Serial.println(pm10);
  }

  // Read AHT10
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  float temperature = temp.temperature;
  float humid = humidity.relative_humidity;
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" °C | Humidity: "); Serial.println(humid);

  // Read Sound
  float noiseDB = readNoiseDBA();

  // Get Time
  String epoch = getEpochTime();

  // Combine all sensor data
  String payload = "Time=" + epoch +
                   ",PM2.5=" + String(pm25, 1) +
                   ",PM10=" + String(pm10, 1) +
                   ",Temp=" + String(temperature, 1) +
                   ",Humidity=" + String(humid, 1) +
                   ",Noise=" + String(noiseDB, 1);

  // Post to OM2M
  postToOM2M(payload);

  delay(10000); // Wait 10 seconds
}

