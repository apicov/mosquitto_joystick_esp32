#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "private_data.h"

int LED_BUILTIN = 2;

#define RED_BUTTON 34
#define UP 35
#define DOWN 32
#define LEFT 33
#define RIGHT 25

#define N_PINS 5
/*
#define SSID "ssid"
#define PASSWORD "password"

const char* mqtt_broker = "ip";
const int mqtt_port = port_n;
*/


WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_topic = "xcarve_ctl/joystick/cmd";
const char* joystick_cmds[] = {"Button.red", "Key.up", "Key.down", "Key.left", "Key.right"};
int cont  = 0;


const uint8_t joystick_pins[N_PINS] = {RED_BUTTON, UP, DOWN, LEFT, RIGHT};
uint8_t joystick_status[N_PINS] = {0,0,0,0,0};
void read_joystick(uint8_t* status_array);
void print_array(uint8_t* array);


void mqtt_connect();
void initWiFi();

void setup() {

  Serial.begin(9600);

  pinMode (LED_BUILTIN, OUTPUT);

  pinMode(RED_BUTTON, INPUT );
  pinMode(UP, INPUT );
  pinMode(DOWN, INPUT );
  pinMode(LEFT, INPUT );
  pinMode(RIGHT, INPUT );


  // Connect to Wi-Fi
  /*WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");*/

  initWiFi();
  Serial.println("Connected to the WiFi network");

  // Set the MQTT server to the client
  client.setServer(mqtt_broker, mqtt_port);
  
  mqtt_connect();
}

void loop() {

  //connect to network in case there is no conection
    if (WiFi.status() != WL_CONNECTED){
      initWiFi();
      Serial.print("wifi connected.");
    }
  
  if (!client.connected()) {
    mqtt_connect();
  }

  read_joystick(joystick_status);
  //print_array(joystick_status);

  //check joystick status, publish first one found in array

  //check if button was pressed
  if (joystick_status[0] == 1){
    //publish message
    client.publish(mqtt_topic, joystick_cmds[0]);
    delay(5000); //wait a while for avoiding  mechanical bouncing of button
  }
  else { //check the others
    for(int i=1; i<N_PINS; i++){
      if (joystick_status[i] == 1){
        // Publish a message
        client.publish(mqtt_topic, joystick_cmds[i]);
        Serial.print(joystick_cmds[i]);
        break;
      }
    }
  }
 delay(70);

}


void mqtt_connect() {
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


void read_joystick(uint8_t* status_array){
  for(int i=0; i<N_PINS; i++ ){
    status_array[i] = !digitalRead(joystick_pins[i]);
  }

}

void print_array(uint8_t* array){
  for(int i=0; i<N_PINS; i++){
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println("");
}


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print('.');
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  Serial.println(WiFi.localIP());
}
