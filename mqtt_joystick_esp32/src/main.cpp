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
#define STATUS_ARRAY_LENGTH (N_PINS+1)

/*
#define SSID "ssid"
#define PASSWORD "password"

const char* mqtt_broker = "ip";
const int mqtt_port = port_n;
*/


WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_subscriber_callback(char* topic, byte* payload, unsigned int length);

const char* mqtt_topic = "joystick/cmd";
const char* joystick_cmds[] = {"Button.red", "Key.up", "Key.down", "Key.left", "Key.right","Key.stop"};
int cont  = 0;

//keeps index of last status 
int previous_status_idx = -1;
//1, jostick is enabled to send commands, 0 is disabled
int joystick_enabled_flg = 1;

const uint8_t joystick_pins[N_PINS] = {RED_BUTTON, UP, DOWN, LEFT, RIGHT};
uint8_t joystick_status[STATUS_ARRAY_LENGTH] = {0,0,0,0,0,0}; //stop command (last position in status array) when all pins are zeros
void read_joystick(uint8_t* status_array);
void print_status_array(uint8_t* array);


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

  initWiFi();
  Serial.println("Connected to the WiFi network");

  // Set the MQTT server to the client
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqtt_subscriber_callback);
  
  mqtt_connect();
  client.subscribe("joystick/enable");
}

void loop() {

  //connect to wifi network in case there is no conection
    if (WiFi.status() != WL_CONNECTED){
      initWiFi();
      Serial.print("wifi connected.");
    }
  
  //connect to mosquitto server if not connected
  if (!client.connected()) {
    mqtt_connect();
    client.subscribe("joystick/enable");
  }

  //check for incomming messages
  client.loop();

  //only send commands when joystick is enabled
  if (joystick_enabled_flg){

      read_joystick(joystick_status);
      //print_status_array(joystick_status);

      //check joystick status, publish first number 1 found in array
      //check if red button was pressed, only sends message if new command is different from the last sent command
      if (joystick_status[0] == 1 && previous_status_idx != 0){
        //publish message
        client.publish(mqtt_topic, joystick_cmds[0]);
        delay(100); //wait a while for avoiding  mechanical bouncing of button
        previous_status_idx = 0;
      }
      else { //check other status flags
        for(int i=1; i<STATUS_ARRAY_LENGTH; i++){
          if (joystick_status[i] == 1 && previous_status_idx != i){
            // Publish a message only if new command is different from the last sent command
            client.publish(mqtt_topic, joystick_cmds[i]);
            Serial.println(joystick_cmds[i]);
            previous_status_idx = i;
            break;
          }
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


void mqtt_subscriber_callback(char* topic, byte* payload, unsigned int length){
  //receives ascii code of 1(49)enable and 0(48)disable joystick
  joystick_enabled_flg = (*payload)==48?0:1;
  if (joystick_enabled_flg){
    previous_status_idx = -1; //ensures current cmd will be sent
    Serial.println("joystick enabled ");
  }
  else{
    //send stop xcarve command  when disabled
    client.publish(mqtt_topic, joystick_cmds[STATUS_ARRAY_LENGTH-1]);
    Serial.println("joystick disabled ");
  }
}


void read_joystick(uint8_t* status_array){
  uint8_t pin_sum = 0;

  for(int i=0; i<N_PINS; i++ ){
    status_array[i] = !digitalRead(joystick_pins[i]);
    pin_sum += status_array[i];
  }

  //if no pins where activated status is stop
  if (!pin_sum){
    status_array[STATUS_ARRAY_LENGTH-1] = 1;
  }
  else{
    status_array[STATUS_ARRAY_LENGTH-1] = 0;
  }

}

void print_status_array(uint8_t* array){
  for(int i=0; i<=N_PINS; i++){
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
