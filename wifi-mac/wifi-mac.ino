#include <WiFi.h>  // For ESP32

const char* ssid = "leelabhavani";         // 🔁 Replace with your Wi-Fi SSID
const char* password = "12345678"; // 🔁 Replace with your Wi-Fi password

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Connected!
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Print MAC Address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Nothing here
}
