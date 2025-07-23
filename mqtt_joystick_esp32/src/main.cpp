/**
 * @file main.cpp
 * @brief Main firmware for ESP32 joystick MQTT publisher.
 */
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "private_data.h"

/**
 * @brief Built-in LED pin number for ESP32.
 */
int LED_BUILTIN = 2;

/**
 * @brief Pin number for the red button.
 */
#define RED_BUTTON 34
/**
 * @brief Pin number for the up direction.
 */
#define UP 35
/**
 * @brief Pin number for the down direction.
 */
#define DOWN 32
/**
 * @brief Pin number for the left direction.
 */
#define LEFT 33
/**
 * @brief Pin number for the right direction.
 */
#define RIGHT 25

/**
 * @brief Number of joystick pins.
 */
#define N_PINS 5
/**
 * @brief Length of the status array (N_PINS + 1 for stop command).
 */
#define STATUS_ARRAY_LENGTH (N_PINS+1)

/*
#define SSID "ssid"
#define PASSWORD "password"

const char* mqtt_broker = "ip";
const int mqtt_port = port_n;
*/


/**
 * @brief MQTT client for ESP32.
 */
WiFiClient espClient;
/**
 * @brief PubSub MQTT client using espClient.
 */
PubSubClient client(espClient);

/**
 * @brief MQTT topic for joystick commands.
 */
const char* mqtt_topic = "joystick/cmd";
/**
 * @brief Array of joystick command strings.
 */
const char* joystick_cmds[] = {"Button.red", "Key.up", "Key.down", "Key.left", "Key.right","Key.stop"};
/**
 * @brief Counter variable (unused).
 */
int cont  = 0;

/**
 * @brief Index of last status sent.
 */
int previous_status_idx = -1;
/**
 * @brief Flag to enable (1) or disable (0) joystick command sending.
 */
int joystick_enabled_flg = 1;

/**
 * @brief Array of joystick pin numbers.
 */
const uint8_t joystick_pins[N_PINS] = {RED_BUTTON, UP, DOWN, LEFT, RIGHT};
/**
 * @brief Array holding the status of each joystick pin and stop command.
 */
uint8_t joystick_status[STATUS_ARRAY_LENGTH] = {0,0,0,0,0,0}; //stop command (last position in status array) when all pins are zeros
/**
 * @brief Reads the status of the joystick pins and updates the status array.
 * @param status_array Pointer to the array to update with pin statuses.
 */
void read_joystick(uint8_t* status_array);
/**
 * @brief Prints the status array to the serial monitor.
 * @param array Pointer to the array to print.
 */
void print_status_array(uint8_t* array);


/**
 * @brief Connects to the MQTT broker.
 */
void mqtt_connect();
/**
 * @brief Initializes the WiFi connection.
 */
void initWiFi();

/**
 * @brief Arduino setup function. Initializes serial, pins, WiFi, and MQTT.
 */
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

/**
 * @brief Arduino main loop. Handles WiFi/MQTT reconnection and joystick command publishing.
 */
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


/**
 * @brief Connects to the MQTT broker, retrying until successful.
 */
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


/**
 * @brief Callback for MQTT subscription. Enables/disables joystick based on payload.
 * @param topic The topic of the incoming message.
 * @param payload The payload of the incoming message.
 * @param length The length of the payload.
 */
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


/**
 * @brief Reads the status of the joystick pins and updates the status array.
 * @param status_array Pointer to the array to update with pin statuses.
 */
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

/**
 * @brief Prints the status array to the serial monitor.
 * @param array Pointer to the array to print.
 */
void print_status_array(uint8_t* array){
  for(int i=0; i<=N_PINS; i++){
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println("");
}


/**
 * @brief Initializes the WiFi connection and waits until connected.
 */
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
