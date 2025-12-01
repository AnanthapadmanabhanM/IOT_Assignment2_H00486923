B31OT Individual Assignment: Secure IoT Environmental Monitoring and Control System

This repository contains the source code and configuration files for the ESP32-based IoT Environmental Monitor. This project demonstrates a secure, full-stack pipeline (Edge, Network, Application) designed to fulfill the requirements of the B31OT IoT module individual assignment.

🌟 Project Overview and Architecture

The system establishes a secure data pipeline using MQTT over TLS/SSL for communication, Node-RED for orchestration and visualization, and MongoDB Atlas for long-term data persistence.

Key Features:

Secure Communication (MQTTS): Uses TLS/SSL encryption (Port 8883) for all communication with the cloud broker, addressing critical security concerns.

Data Persistence: Logs time-series sensor data directly into a scalable MongoDB Atlas database (IOT.sensor_data).

Bidirectional Control: Remotely controls the NeoPixel LED status via MQTT commands published from the Node-RED dashboard.

Performance: Achieved an average publish latency of 280 ms over the secure connection.

🛠️ Configuration Details

1. ESP32 Firmware (esp32_firmware_secure_final.ino)

The firmware uses the secure HiveMQ Cloud broker and requires authentication.

Parameter

Role

Value

mqtt_server

Secure Broker URL

2bcacdc6c78e4b73a575478294c5953b.s1.eu.hivemq.cloud

mqtt_port

Secure TLS Port

8883

mqtt_user

Authentication Username

Ananthapadmanabhan_Manoj

mqtt_pass

Authentication Password

Padmanabham@23

TELEMETRY_TOPIC

Sensor Data (Publish)

b31ot-manoj/iot/env/telemetry

COMMAND_TOPIC

LED Control (Subscribe)

b31ot-manoj/iot/env/cmd

2. Cloud Persistence (MongoDB Atlas)

The storage flow (mongodb_storage_flow_final.json) is configured to connect to your cluster.

Cluster: cluster0.wau2vik.mongodb.net

Database: IOT

Collection: sensor_data

Required Node-RED Library: node-red-contrib-mongodb4

📁 Repository Contents

File

Description

Assignment Part

esp32_firmware_secure_final.ino

C++ Sketch: Full firmware implementing MQTTS, sensor read, JSON publication, and command callback.

1, 2, 3

node_red_flow.json

Node-RED Flow: Visualization dashboard (Gauges, Charts, Control Buttons).

3

mongodb_storage_flow_final.json

Node-RED Flow: Logic to subscribe to telemetry and persist data into MongoDB Atlas.

4 (Persistence)

data_analysis.py

Python Script: Subscribes to MQTT, logs data to CSV, and generates statistical analysis and plots.

4 (Analytics)

IOT_Assign.2_H00486923-6.pdf

Final IEEE Report: Submitted documentation and critical evaluation.

N/A

🚀 Getting Started (Quick Setup Guide)

Prerequisites:

Arduino IDE with ESP32 Board Support installed.

Arduino Libraries: PubSubClient, DHT, Adafruit_NeoPixel, ArduinoJson, and WiFiClientSecure.

Running Node.js/Node-RED instance.

Steps:

Flash ESP32: Compile and upload esp32_firmware_secure_final.ino after confirming library dependencies are met.

Import Flows: In the Node-RED editor, import both .json flow files.

Configure Node-RED Broker: Double-click the MQTT broker node in both imported flows and ensure the Server (2bcacdc6c78e...), Port (8883), and Security Credentials match the firmware.

Configure MongoDB: Double-click the MongoDB node and paste the correct connection URI for the IOT database.

Deploy & Run: Deploy all flows and navigate to the dashboard UI to verify live data stream and control.

Author: Ananthapadmanabhan Manoj
Student ID: H00486923
Module: B31OT

