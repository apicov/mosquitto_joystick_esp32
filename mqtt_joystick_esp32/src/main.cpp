#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "private_data.h"

/*
#define SSID "ssid"
#define PASSWORD "password"

const char* mqtt_broker = "ip";
const int mqtt_port = port_n;
*/


WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_topic = "xcarve_ctl/joystick/cmd";
const char* joystick_cmds[] = {"Key.up","Key.down", "Key.right", "Key.left"};
int cont  = 0;

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
  if (cont  < 4){
    // Publish a message
    client.publish(mqtt_topic, joystick_cmds[cont]);
    cont++;
  }
  else{
    cont = 0;
  }

  delay(5000);
}
