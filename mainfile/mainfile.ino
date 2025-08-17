#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"

// ===== CONFIGURATION =====
const char* ssid = "leelabhavani";    // Your WiFi SSID
const char* password = "12345678";           // Your WiFi password

const char* om2mServer = "http://10.2.143.103:8080/~/in-cse/cnt-496337131";
const char* om2mAe = "admin:admin";  // Default OM2M credentials

// ===== Sensors =====
SoftwareSerial sdsSerial(16, 17);  // SDS011 RX, TX
SDS011 my_sds;
float pm25, pm10;
int err;

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println(WiFi.localIP());

  // Init SDS011
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 initialized.");

  // Init AHT10
  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT10 not found!");
    while (1);
  }
  Serial.println("AHT10 initialized.");
}

void postToOM2M(String content) {
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
    Serial.print("POST to OM2M successful. Response code: ");
    Serial.println(code);
    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.print("POST failed. Error: ");
    Serial.println(code);
  }

  http.end();
}

void loop() {
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

  // Combine JSON string
  String payload = "PM2.5=" + String(pm25, 1) +
                   ",PM10=" + String(pm10, 1) +
                   ",Temp=" + String(temperature, 1) +
                   ",Humidity=" + String(humid, 1);

  // Send to OM2M
  postToOM2M(payload);

  delay(10000);  // Every 10 seconds
}
