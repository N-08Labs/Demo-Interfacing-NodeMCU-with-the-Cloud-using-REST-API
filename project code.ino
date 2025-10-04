//Create by N-08 LABS 
//https://github.com/N-08Labs
//instructables.com/member/N-08 Labs
//www.youtube.com/@N08labs
//instagram.com/n08labs 

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// ---- WiFi Credentials ----
const char* ssid = "N-08";
const char* password = "Nagarjuna1234";

// ---- Webhook.site URL ----
String webhookURL = "http://webhook.site/f5a90447xxxxxxxxxxxx"; //replace with your own URL

// ---- DHT Settings ----
#define DHTPIN 4        // GPIO2 (D4 on NodeMCU)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;     // ✅ Create WiFi client
    HTTPClient http;

    // Read DHT11 data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("❌ Failed to read from DHT sensor!");
      delay(2000);
      return;
    }

    // JSON data
    String jsonData = "{";
    jsonData += "\"temperature\": " + String(temperature, 1) + ",";
    jsonData += "\"humidity\": " + String(humidity, 1);
    jsonData += "}";

    // Begin request with client + URL ✅
    http.begin(client, webhookURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Data sent: ");
    Serial.println(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("❌ Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("⚠️ WiFi Disconnected!");
  }

  delay(10000); // send every 10 sec
}
