/*
 * Example #1 - Gateway
 * version: v1.0
 * (AMOR = Arduino Modular Opentherm Regulator)
 * 
 * Acts as simple man-in-the-middle between boiler and thermostat. It does not modify the OpenTherm messages
 * sent from the thermostat to the boiler and back. Just prints them.
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - data from boiler displayed on serial port
 * 
 * ---
 * how to make this work
 * - copy everything in this file and paste it in the main project file "AMOR_arduino.ino" - replace everything there
 * - check the "Config.h", if the pin numbers for OpenTherm shield are correct, eventually change the language
 * 
 */
 
#include "Config.h"
#include "OpenthermAdapter.h"
#include "SerialCommunicator.h"
#include "MonitorOTchannel.h"
#include "LoopbackOTchannel.h"

OpenthermAdapter boiler;
SerialCommunicator myCommunicator;
MonitorOTchannel myMonitor(myCommunicator);
LoopbackOTchannel myLoopTherm;

void setup() {
  Serial.begin(115200);
  delay(50);

  DPRINTLN(F("AMOR: Example 1 - Gateway (v1.0)"));
  DPRINTLN(F("--------------------------------"));

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
