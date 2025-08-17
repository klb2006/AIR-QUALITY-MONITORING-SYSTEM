#include <WiFi.h>
#include "time.h"

// Replace with your network credentials
const char* ssid = "leelabhavani";
const char* password = "12345678";

// NTP server and timezone offset
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;    // India = GMT+5:30 = 19800 seconds
const int daylightOffset_sec = 0;

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Wait for time to be set
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println("Time initialized.");
}

void loop(){
  time_t now;
  time(&now);  // Get current epoch time
  Serial.print("Epoch Time: ");
  Serial.println(now);

  delay(1000); // Wait for 1 second
}
