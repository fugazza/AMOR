/*
 * AMOR = Arduino Modular Opentherm Regulator
 * build your own customized thermostat to regulate our OpenTherm boiler
 * version: v1.0
 * 
 * example of customized single regulator driven by date-time dependent temperature programs
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - own PID regulator for heating
 * - own regulator for DHW (domestic hot water)
 * - programmed to heat only during day, different times for workday and weekend
 * - thermostat used only to get the target temperature and to display boiler status 
 * - data from boiler sent to the MQTT server
 * 
 * ---
 * this is the main project sketch
 * - replace it with sketches from "/examples" folder to get other regulator setups, and/or modify it to fit your needs
 * - only this file "AMOR_arduino.ino" and "Config.h" need to be modified, all other files are only defining
 *   objects, modules, build block from which you build your regulators
 * 
 */
 
#include "Config.h"
#include "OpenthermAdapter.h"
#include "EspMqttCommunicator.h"
#include "MonitorOTchannel.h"
#include "OTlooper.h"
#include "SingleRegulator.h"
#include "DHWregulator.h"
#include "Programmer.h"

OpenthermAdapter boiler;
EspMqttCommunicator myCommunicator;
MonitorOTchannel mqttMonitorTherm(myCommunicator);
OTlooper loopGenerator;
const static char mainRegulatorName[] PROGMEM = "livingroom";
SingleRegulator mainRegulator(mainRegulatorName);
Programmer myCHprogrammer;
DHWregulator myDHWregulator;
Programmer myDHWprogrammer;

void setup() {
  Serial.begin(115200);
  delay(50);

  DPRINTLN(F("AMOR = Arduino Modular Opentherm Regulator v1.0 - PID regulator example"));
  DPRINTLN(F("-----------------------------------------------------------------------"));

  //connect the MQTT module in between "boiler" and "loop generator"- MQTT module reports to the MQTT server
  // all the data from OpenTherm massages passing through it
  mqttMonitorTherm.attachOTchannel(boiler);
  
  // OpenTherm protocol requires that thermostat (master) send every second some OpenTherm message to the boiler (slave)
  // - module "loopGenerator" does this job, generates these messages every second
  // "loopGenerator" is also responsible to answer OpenTherm messages incoming from the thermostat interface on OpenTherm shield
  // and to harvest any necessary data from it
  loopGenerator.attachOTchannel(mqttMonitorTherm);
  
  // "mainRegulator" is a PID regulator, that calculates every minute the temperature of heating water, which is necessary
  // to reach the target room temperature
  mainRegulator.attachBoiler(loopGenerator.getCHBoilerInterface());
  mainRegulator.setCommunicator(myCommunicator); // necessary if you want the regulator to publish its data to MQTT server
  loopGenerator.registerDataListener(mainRegulator); // loopGenerator serves the target temperature from the thermostat
  myCHprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WORKDAY,6,30,22, 0));
  myCHprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WEEKEND,8, 0,22, 0));
  mainRegulator.setProgrammer(myCHprogrammer);
  
  // "myDHWregulator" is responsible to tell the loopGenerator the data when to heat the "Domestic Hot Water" (DWH) and to which temperature
  myDHWregulator.attachBoiler(loopGenerator.getDHWBoilerInterface());
  myDHWprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WORKDAY,6, 0,22, 0));
  myDHWprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WEEKEND,7,30,22, 0));
  myDHWregulator.setProgrammer(myDHWprogrammer);
}

/*
 * main loop - only calls repeatedly the loop() functions of modules to ensure their proper work
 */
void loop() {
  myCommunicator.loop();
  boiler.loop();
  loopGenerator.loop();
  mainRegulator.loop();
  myDHWregulator.loop();
}
