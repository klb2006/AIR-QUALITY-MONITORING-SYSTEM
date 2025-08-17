#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"
#include "time.h"

// ===== Wi-Fi Configuration =====
const char* ssid = "leelabhavani";
const char* password = "12345678";

// ===== OM2M Server Configuration =====
const char* om2mServer = "http://10.2.143.103:8080/~/in-cse/cnt-496337131";
const char* om2mAe = "admin:admin";

// ===== LED Pin Assignments =====
#define POWER_LED 4  // Red LED for power status (D4)
#define WIFI_LED 5   // Green LED for Wi-Fi and data status (D5)

// ===== Time Configuration =====
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // IST (UTC+5:30)
const int daylightOffset_sec = 0;

// ===== Sensor Setup =====
SoftwareSerial sdsSerial(16, 17);  // SDS011 TX=17, RX=16
SDS011 my_sds;
float pm25, pm10;
int err;

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // LED Setup
  pinMode(POWER_LED, OUTPUT);
  pinMode(WIFI_LED, OUTPUT);

  // Turn ON Power LED
  digitalWrite(POWER_LED, HIGH);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  // Blink Green LED while connecting
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WIFI_LED, HIGH);
    delay(300);
    digitalWrite(WIFI_LED, LOW);
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  // Turn ON Green LED for 1 second to show connected
  digitalWrite(WIFI_LED, HIGH);
  delay(1000);
  digitalWrite(WIFI_LED, LOW);

  // Setup time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Initialize SDS011
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 initialized.");

  // Initialize AHT10
  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 not found!");
    while (1); // Halt
  }
  Serial.println("AHT10 initialized.");
}

// Get current epoch time
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

// Send data to OM2M
void postToOM2M(String content) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping data post.");
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
    Serial.print("POST successful. Code: ");
    Serial.println(code);
    Serial.println(http.getString());

    // Blink green LED quickly to indicate data sent
    digitalWrite(WIFI_LED, HIGH);
    delay(200);
    digitalWrite(WIFI_LED, LOW);
  } else {
    Serial.print("POST failed. Error: ");
    Serial.println(code);
  }

  http.end();
}

void loop() {
  // Check Wi-Fi connection status
  if (WiFi.status() != WL_CONNECTED) {
    // Blink green LED continuously if disconnected
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
    Serial.print("PM2.5: ");
    Serial.print(pm25);
    Serial.print(" µg/m³ | PM10: ");
    Serial.println(pm10);
  }

  // Read AHT10
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  float temperature = temp.temperature;
  float humid = humidity.relative_humidity;

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.println(humid);

  // Get time
  String epoch = getEpochTime();

  // Prepare payload
  String payload = "Time=" + epoch +
                   ",PM2.5=" + String(pm25, 1) +
                   ",PM10=" + String(pm10, 1) +
                   ",Temp=" + String(temperature, 1) +
                   ",Humidity=" + String(humid, 1);

  // Send data
  postToOM2M(payload);

  delay(10000); // Delay 10s
}



