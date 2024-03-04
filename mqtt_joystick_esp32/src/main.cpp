#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "private_data.h"

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_topic = "test/topic";

void setup() {

  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");


  // Set the MQTT server to the client
  client.setServer(mqtt_broker, mqtt_port);
  
  // Connect to MQTT Broker
  while (!client.connected()) {
      Serial.println("Connecting to MQTT Broker...");
      if (client.connect("ESP32Client")) {
          Serial.println("Connected to MQTT Broker");
      } else {
          Serial.print("Failed with state ");
          Serial.print(client.state());
          delay(5000);
      }
  }
}

void loop() {
  // Publish a message
  client.publish(mqtt_topic, "Hello from ESP32");
  delay(2000);
}
