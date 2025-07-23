# mosquitto_joystick_esp32

## Overview

This project provides a complete solution for controlling devices via MQTT using an ESP32-based joystick. It includes:
- **ESP32 firmware** (PlatformIO/Arduino) for reading joystick/button inputs and publishing commands to an MQTT broker.
- **Python notebooks** for publishing and subscribing to MQTT topics, useful for testing and integration.

## Project Structure

```
mosquitto_joystick_esp32/
├── mqtt_joystick_esp32/         # ESP32 firmware (PlatformIO project)
│   ├── include/                 # Header files
│   ├── lib/                     # Private libraries
│   ├── src/
│   │   ├── main.cpp             # Main firmware source
│   │   └── temp                 # Alternate/test firmware
│   ├── test/                    # Unit tests (PlatformIO)
│   └── platformio.ini           # PlatformIO configuration
├── moquitto_publisher.ipynb     # Python MQTT publisher example
├── mosquitto_subscriber.ipynb   # Python MQTT subscriber example
└── README.md                    # This file
```

## ESP32 Firmware

- **Location:** `mqtt_joystick_esp32/`
- **Board:** ESP32 Dev Module (or compatible)
- **Framework:** Arduino
- **MQTT Library:** [PubSubClient](https://github.com/knolleary/pubsubclient)

### Features
- Reads joystick and button states via GPIO
- Publishes commands to an MQTT topic (e.g., `joystick/cmd`)
- Subscribes to enable/disable commands via MQTT
- WiFi and MQTT auto-reconnect

### Building and Uploading
1. **Install [PlatformIO](https://platformio.org/):**
   - VSCode extension or CLI
2. **Open the project folder:** `mqtt_joystick_esp32/`
3. **Configure WiFi and MQTT credentials:**
   - Edit `private_data.h` (not included in repo) to define `SSID`, `PASSWORD`, `mqtt_broker`, and `mqtt_port`.
4. **Build and upload:**
   - In VSCode: click the PlatformIO build/upload buttons
   - Or CLI:
     ```sh
     pio run
     pio run -t upload
     ```
5. **Monitor serial output:**
   - In VSCode: PlatformIO Serial Monitor
   - Or CLI:
     ```sh
     pio device monitor
     ```

## Python MQTT Notebooks

- **moquitto_publisher.ipynb:** Example of publishing MQTT messages using `paho-mqtt`.
- **mosquitto_subscriber.ipynb:** Example of subscribing and printing MQTT messages.

### Requirements
- Python 3.x
- `paho-mqtt` library (`pip install paho-mqtt`)

### Usage
Open the notebooks in Jupyter and run the cells. Adjust broker address and topics as needed.

## Directory Conventions
- `include/` — Project header files
- `lib/` — Private libraries (see README in each for details)
- `test/` — PlatformIO unit tests

## Documentation
- Source code is documented with Doxygen comments.
- To generate HTML docs:
  1. Install [Doxygen](https://www.doxygen.nl/)
  2. Run `doxygen` in the firmware directory (after creating a `Doxyfile`)

## License
Specify your license here (e.g., MIT, Apache 2.0, etc.)

## References
- [PlatformIO ESP32 Arduino Guide](https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html)
- [PubSubClient MQTT Library](https://github.com/knolleary/pubsubclient)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
