#include <Adafruit_AHTX0.h>

// Create with custom I2C PINs
#define SDA_PIN 21
#define SCL_PIN 22
TwoWire customWire = TwoWire(0);  // Use I2C bus 0

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  customWire.begin(SDA_PIN, SCL_PIN);
  
  if (!aht.begin(&customWire)) {  // Pass the custom Wire object
    Serial.println("Sensor not  found");
    while (1) delay(10);
  }
}

void loop() {
  // Same reading code as above
}
