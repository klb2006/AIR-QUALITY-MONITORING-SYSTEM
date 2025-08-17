#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "leelabhavani";
const char* password = "12345678";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

// Function to post data to the server
void postData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("https://ctop.iiit.ac.in/api/nodes/create-cin/15");  // Use https for secure connection
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer fdb2dc21155ac365c38a7875e6dfddc3");
        
        DynamicJsonDocument jsonDoc(1024);  // Create a JSON document
        jsonDoc["pm2.5"] = "25";
jsonDoc["pm10"] = "2.5";
jsonDoc["humidity"] = "45.5";
jsonDoc["temperature"] = "34";
jsonDoc["noise"] = "90";

        
        String requestBody;  // Serialize JSON to a string
        serializeJson(jsonDoc, requestBody);
        int httpResponseCode = http.POST(requestBody);  // Send POST request
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();  // End the HTTP connection
    } else {
        Serial.println("WiFi not connected. Cannot send data.");
    }
}

void loop() {
    postData();
    delay(3000); // Delay between POST requests
}