# B31OT IoT Assignment – Environmental Monitoring System

Student: Ananthapadmanabhan Manoj  
ID: H00486923

## Features
- ESP32 + DHT11 + NeoPixel
- MQTT over TLS/SSL (port 8883)
- MongoDB Atlas cloud persistence
- Real-time Node-RED dashboard
- Remote LED control
- 280 ms average publish latency

## Live Links
- Video Demo: https://youtu.be/[YOUR_VIDEO_ID]
- MongoDB: `IOT.sensor_data`

## Wiring
- DHT11 Data  → GPIO4
- NeoPixel DIN → GPIO5
- 3.3V, GND

## Security
- TLS enabled
- Unique topics: `b31ot-manoj/iot/env/*`
