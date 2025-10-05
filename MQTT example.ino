#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "N-08";
const char* password = "n08labs";

// MQTT broker details (HiveMQ Public Broker)
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// DHT sensor setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    String clientId = "NodeMCU-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println(" connected ✅");
      client.publish("nodemcu/status", "Device Connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Create a JSON-style message
  String payload = "{ \"temperature\": ";
  payload += String(temperature);
  payload += ", \"humidity\": ";
  payload += String(humidity);
  payload += " }";

  // Publish data
  client.publish("nodemcu/sensors", payload.c_str());
  Serial.println("📤 Data sent: " + payload);

  delay(5000); // Send every 5 seconds
}
