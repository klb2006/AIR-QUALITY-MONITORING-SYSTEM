# AIR-QUALITY-MONITORING-SYSTEM
An IoT-based Air Quality Monitoring System built using ESP32, SDS011 Dust Sensor, AHT10 (Temperature &amp; Humidity Sensor), Sound Sensor, and LED Indicators. The system monitors PM2.5, PM10, temperature, humidity, and noise levels in real-time and uploads data to ThingSpeak and CTOP platforms for visualization and analysis.

📌 Features
✅ Real-time monitoring of air quality (PM2.5 & PM10)
✅ Tracks temperature, humidity, and noise levels
✅ Data logging & visualization on ThingSpeak & CTOP
✅ LED indicators for WiFi & data transmission status
✅ Low-cost, scalable IoT solution

🛠️ Components Used
ESP32 – WiFi-enabled microcontroller
SDS011 Dust Sensor – PM2.5 & PM10 detection
AHT10 Sensor – Temperature & humidity
Sound Sensor – Noise detection
LED Indicators – WiFi & Data transmission status
Breadboard & Jumper wires

⚡ System Architecture
   Sensors (SDS011, AHT10, Sound Sensor)
        ↓
   ESP32 (Data Processing + WiFi)
        ↓
   ThingSpeak & CTOP (Cloud Platforms)
        ↓
   Real-time Monitoring & Visualization


   ## 🚀 Getting Started  

### 🔹 1. Clone the Repository  
```bash
git clone https://github.com/YourUsername/AirQualityMonitoringSystem.git
cd AirQualityMonitoringSystem
```

### 🔹 2. Install Required Libraries (Arduino IDE)  
- ESP32 board package  
- SDS011 sensor library  
- AHT10 library  
- WiFi library  

### 🔹 3. Upload Code to ESP32  
1. Open `code/air_quality_monitoring.ino` in Arduino IDE.  
2. Select **ESP32 board** and correct **COM port**.  
3. Upload the code.  

## 📌 Future Improvements  
- Integration with **Mobile App / Web Dashboard**  
- SMS/Email alerts for poor air quality  
- Solar-powered version for outdoor use  

---

## 🤝 Contributing  
Contributions are welcome! Please fork the repository and create a pull request.  

---

## 📜 License  
This project is licensed under the **MIT License** – feel free to use and modify.  

