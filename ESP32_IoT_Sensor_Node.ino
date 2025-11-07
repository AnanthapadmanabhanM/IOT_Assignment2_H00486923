#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// --------------------------------------------------------------------------------------------
//        CONFIGURATION OF ENVIRONMENT
// --------------------------------------------------------------------------------------------
// --- HiveMQ Cloud Credentials ---
const char* mqtt_server = "2bcacdc6c78e4b73a575478294c5953b.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;  // Secure TLS port
const char* mqtt_user = "Ananthapadmanabhan_Manoj";
const char* mqtt_pass = "Padmanabham@23";

// --- MQTT Topics (Use your unique ID) ---
const char* telemetry_topic = "b31ot-manoj/iot/env/telemetry";
const char* command_topic   = "b31ot-manoj/iot/env/cmd";

// --- WiFi Credentials ---
const char* ssid = "Sam21 FE";
const char* password = "msdhoni7";

// --- Sensor and LED Pins ---
#define RGB_PIN 5
#define DHT_PIN 4
#define DHTTYPE DHT11
#define NEOPIXEL_TYPE NEO_GRB + NEO_KHZ800  // Fixed: GRB order for WS2812B

// --- Temperature Ranges for LED Colors (Auto Mode) ---
#define ALARM_COLD 0.0
#define ALARM_HOT  30.0
#define WARN_COLD  10.0
#define WARN_HOT   25.0

// --------------------------------------------------------------------------------------------
//        OBJECT DECLARATIONS
// --------------------------------------------------------------------------------------------
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, RGB_PIN, NEOPIXEL_TYPE);
DHT dht(DHT_PIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --------------------------------------------------------------------------------------------
//        CALLBACK: Handle Incoming MQTT Commands
// --------------------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Parse JSON command
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Manual color control: {"r":255, "g":0, "b":0} or {"color":"red"}
  if (doc.containsKey("r") && doc.containsKey("g") && doc.containsKey("b")) {
    uint8_t r = doc["r"];
    uint8_t g = doc["g"];
    uint8_t b = doc["b"];
    pixel.setPixelColor(0, r, g, b);
    pixel.show();
    Serial.println("LED set via RGB values");
  }
  else if (doc.containsKey("color")) {
    String color = doc["color"] | "";
    color.toLowerCase();
    if (color == "red")    pixel.setPixelColor(0, 255, 0, 0);
    else if (color == "green") pixel.setPixelColor(0, 0, 255, 0);
    else if (color == "blue")  pixel.setPixelColor(0, 0, 0, 255);
    else if (color == "white") pixel.setPixelColor(0, 255, 255, 255);
    else if (color == "off")   pixel.setPixelColor(0, 0, 0, 0);
    else {
      Serial.println("Unknown color command");
      return;
    }
    pixel.show();
    Serial.println("LED set via color name");
  }
}

// --------------------------------------------------------------------------------------------
//        WiFi SETUP
// --------------------------------------------------------------------------------------------
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// --------------------------------------------------------------------------------------------
//        MQTT RECONNECT (with Subscribe)
// --------------------------------------------------------------------------------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected to HiveMQ!");
      client.subscribe(command_topic);
      Serial.print("Subscribed to: ");
      Serial.println(command_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// --------------------------------------------------------------------------------------------
//        SETUP
// --------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  setup_wifi();

  // TLS: Disable cert verification for testing (remove in production)
  espClient.setInsecure();

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  // Enable command receiving

  // Initialize sensors and LED
  dht.begin();
  pixel.begin();
  pixel.setBrightness(100);
  pixel.show();
}

// --------------------------------------------------------------------------------------------
//        MAIN LOOP
// --------------------------------------------------------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish >= 10000) {  // Every 10 seconds
    lastPublish = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // --- Auto LED Color Based on Temperature ---
    uint8_t r = (t >= ALARM_HOT) ? 255 : ((t > WARN_HOT) ? 150 : 0);
    uint8_t g = (t > ALARM_COLD && t <= WARN_HOT) ? 255 : ((t < ALARM_HOT) ? 150 : 0);
    uint8_t b = (t < WARN_COLD) ? 150 : 0;
    pixel.setPixelColor(0, r, g, b);
    pixel.show();

    // --- Create JSON Payload ---
    StaticJsonDocument<100> doc;
    doc["temperature"] = round(t * 10) / 10.0;  // 1 decimal
    doc["humidity"]    = round(h * 10) / 10.0;
    doc["timestamp"]   = millis() / 1000;      // seconds since boot
    char buffer[128];
    serializeJson(doc, buffer);

    // --- Measure Publish Latency ---
    unsigned long startTime = millis();
    bool success = client.publish(telemetry_topic, buffer);
    unsigned long latency = millis() - startTime;

    if (success) {
      Serial.println("Data published:");
      Serial.println(buffer);
      Serial.print("Publish latency: ");
      Serial.print(latency);
      Serial.println(" ms");
    } else {
      Serial.println("Publish failed");
    }
  }
}