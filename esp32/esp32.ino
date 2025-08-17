#include <SDS011.h>              // Include the library for SDS011 dust sensor
#include <SoftwareSerial.h>      // Include SoftwareSerial library to use custom serial pins

float pm2_5, pm10;               // Variables to store PM2.5 and PM10 values
int err;                         // Variable to store the result of sensor reading

// Initialize SoftwareSerial for SDS011 on GPIO 16 (RX) and GPIO 17 (TX)
SoftwareSerial sdsSerial(16, 17);  // sdsSerial is used to communicate with SDS011 sensor
SDS011 my_sds;                     // Create an SDS011 sensor object

void setup() {
    Serial.begin(115200);         // Initialize the default Serial monitor (for debugging) at 115200 baud
    sdsSerial.begin(9600);        // Initialize SoftwareSerial communication with SDS011 at 9600 baud
    my_sds.begin(16, 17);         // Initialize SDS011 using the same RX and TX pins

    Serial.println("SDS011 Sensor Initialized.");  // Print initialization message to Serial monitor
}

void loop() {
    // Try to read PM2.5 and PM10 values from the sensor
    err = my_sds.read(&pm2_5, &pm10);  // Read data from SDS011 and store it in pm2_5 and pm10

    if (!err) {  // If no error (err == 0), print the sensor readings
        Serial.print("PM2.5: " + String(pm2_5) + " µg/m³");  // Print PM2.5 value
        Serial.println("PM10:  " + String(pm10) + " µg/m³"); // Print PM10 value
    } else {
        Serial.println("Failed to read from SDS011 sensor!"); // Print error message if reading fails
    }

    delay(2000);  // Wait for 2 seconds before the next reading
}