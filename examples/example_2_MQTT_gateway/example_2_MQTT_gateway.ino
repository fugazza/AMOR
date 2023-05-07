/*
 * Example #2 - MQTT Gateway
 * version: v1.0
 * (AMOR = Arduino Modular Opentherm Regulator)
 * 
 * Acts as simple man-in-the-middle between boiler and thermostat. It does not modify the OpenTherm messages
 * sent from the thermostat to the boiler and back. Does not print messages as how Example #1 does, but expects
 * ESP8266 wifi module with "esp8266_mqtt_firmware" loaded, which sends MQTT messages to the predefined MQTT server.
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - ESP8266 wifi module connected to the serial line of arduino
 * - "esp8266_mqtt_firmware" configured and uploaded to the ESP8266 module
 * - data from boiler passed through serial line to teh ESP8266 module, which sends them to the MQTT server
 * 
 * ---
 * how to make this work
 * - prepare ESP8266 wifi module - see readme attached to this example and project wiki for more details, here you are
 *   only brief summary
 *   - prepare MQTT server - either your own like Mosquitto on Raspberry PI; or use any web MQTT service
 *   - configure "esp8266_mqtt_firmware" - put there your wifi login info and info of the used MQTT server
 *   - upload the "esp8266_mqtt_firmware" to the ESP8266 wifi module
 * - prepare arduino
 *   - copy everything in this file and paste it in the main project file "AMOR_arduino.ino" - replace everything there
 *   - check the "Config.h", generally settings from Example #1 should work; for this example there are only few new
 *     config options related to the MQTT communication, from which the only really important is "MQTT_BASE_TOPIC"
 * 
 */
 
#include "Config.h"
#include "OpenthermAdapter.h"
#include "EspMqttCommunicator.h"
#include "MonitorOTchannel.h"
#include "LoopbackOTchannel.h"

OpenthermAdapter boiler;
// note the difference comparing to the Example #1 - it is only the following line,
// where "SerialCommunicator" is replaced by "EspMqttCommunicator", everything else is identic
EspMqttCommunicator myCommunicator;
MonitorOTchannel myMonitor(myCommunicator);
LoopbackOTchannel myLoopTherm;

void setup() {
  Serial.begin(115200);
  delay(50);

  DPRINTLN(F("AMOR: Example 2 - MQTT Gateway (v1.0)"));
  DPRINTLN(F("-------------------------------------"));

  // connect the MQTT module in between "boiler" and "loopback"- Serial Communicator prints contents of
  // all the data from OpenTherm massages passing through it
  myMonitor.attachOTchannel(boiler);

  // place the loopback module on top of monitor module, so that it forwards all messages from thermostat
  // to the boiler and answers in reverse direction
  myLoopTherm.attachOTchannel(myMonitor);
}

/*
 * main loop - only calls repeatedly the loop() functions of modules to ensure their proper work
 */
void loop() {
  boiler.loop();
}
