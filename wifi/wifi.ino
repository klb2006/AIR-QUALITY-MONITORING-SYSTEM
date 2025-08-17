#include <WiFi.h>  // Use <ESP8266WiFi.h> if using ESP8266

const char* ssid = "leelabhavani";       // Replace with your WiFi name
const char* password = "12345678";   // Replace with your WiFi password

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Your code here
}
