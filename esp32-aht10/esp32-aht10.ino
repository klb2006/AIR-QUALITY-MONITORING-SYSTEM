#include <Wire.h>
#include <SoftwareSerial.h>
#include <SDS011.h>
#include "Adafruit_AHTX0.h"

// SDS011 PM Sensor
SoftwareSerial sdsSerial(16, 17);   // SDS011 on GPIO16 (RX), GPIO17 (TX)
SDS011 my_sds;
float pm2_5, pm10;
int err;

// AHT10/AHT20 Temperature & Humidity Sensor
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for Serial to open

  // Init SDS011
  sdsSerial.begin(9600);
  my_sds.begin(16, 17);
  Serial.println("SDS011 Sensor Initialized.");

  // Init AHT10/AHT20
  Wire.begin(21, 22);  // I2C on ESP32: SDA=21, SCL=22
  if (!aht.begin()) {
    Serial.println("Couldn't find AHT10/AHT20 sensor. Check wiring!");
    while (1) delay(10);
  }
  Serial.println("AHT10/AHT20 Sensor Initialized.");
}

void loop() {
  // --- Read Air Quality ---
  err = my_sds.read(&pm2_5, &pm10);
  if (!err) {
    Serial.print("PM2.5: ");
    Serial.print(pm2_5);
    Serial.print(" µg/m³  |  PM10: ");
    Serial.print(pm10);
    Serial.println(" µg/m³");
  } else {
    Serial.println("Failed to read from SDS011 sensor!");
  }

  // --- Read Temperature & Humidity ---
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %");

  Serial.println("---------------------------------------------------");

  delay(5000);  // Delay between readings
}