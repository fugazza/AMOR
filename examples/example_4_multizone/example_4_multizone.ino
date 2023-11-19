/*
 * Example #4 - Multizone thermostat
 * version: v1.0
 * (AMOR = Arduino Modular Opentherm Regulator)
 * 
 * Example of multizone regulator. Three single thermostats are combined together and they control
 * the room temperature in three different rooms independently.
 * 
 * Hardware configuration:
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - ESP8266 wifi module connected to the serial line of arduino
 * - livingroom: radiator with wifi thermoelectric valve
 * - livingroom: floor heating with smart plug
 * - bedroom: radiator with wifi thermoelectric valve + wifi thermometer
 * 
 * Features - what this example does:
 * - three regulators combined into one multi-zone regulator
 * - each zone can be controlled and commandede independently
 * - programms can be modified via MQTT messages
 * 
 * How to make this work
 * - prepare zones - thermometers, thermoelectric valves and smart plugs - see the README for this example for more details
 * - prepare ESP8266 wifi module - You can keep the ESP8266 the same as in the examples #2 and #3.
 * - prepare arduino
 *   - copy the contents of this example and paste it in the main project file "AMOR_arduino.ino" (replace everything there)
 *   - check the "Config.h", keep settings from Example #3. Default values should work fine.
 *   - compile and upload to the arduino
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
#include "MqttDataProvider.h"
#include "MultizoneRegulator.h"

OpenthermAdapter boiler;
EspMqttCommunicator myCommunicator;
MonitorOTchannel mqttMonitorTherm(myCommunicator);
OTlooper loopGenerator;
MultizoneRegulator multiRegulator;

const static char mainRegulatorName[] PROGMEM = "livingroom";
SingleRegulator mainRegulator(mainRegulatorName);
Programmer myCHprogrammer;
MqttDataProvider CHprogramProvider(myCommunicator);

const static char floorRegulatorName[] PROGMEM = "floor";
SingleRegulator floorRegulator(floorRegulatorName);
Programmer floorProgrammer;
MqttDataProvider floorProgramProvider(myCommunicator);

const static char bedroomRegulatorName[] PROGMEM = "bedroom";
SingleRegulator bedroomRegulator(bedroomRegulatorName);
Programmer bedroomProgrammer;
MqttDataProvider bedroomProgramProvider(myCommunicator);
MqttDataProvider bedroomTemperatureProvider(myCommunicator);

DHWregulator myDHWregulator;
Programmer myDHWprogrammer;
MqttDataProvider DHWprogramProvider(myCommunicator);


void setup() {
  Serial.begin(115200);
  delay(50);

  DPRINTLN(F("AMOR: Example 4 - Multizone thermostat (v1.0)"));
  DPRINTLN(F("---------------------------------------------"));

  // connection of boiler, MQTT monitor and loop generator is identic to example #3
  mqttMonitorTherm.attachOTchannel(boiler);
  loopGenerator.attachOTchannel(mqttMonitorTherm);

  // new is the connection of the multizone regulator
  multiRegulator.attachBoiler(loopGenerator.getCHBoilerInterface());


  // ------------------------------------------------------------------------------------
  // setup of "mainRegulator" = the zone for radiator in the livingroom
  mainRegulator.attachBoiler(multiRegulator.getZoneBoilerInterface(0));  // this is how to connect the zone regulator to the multi-zone regulator
  mainRegulator.setCommunicator(myCommunicator); // necessary if you want the regulator to publish its data to MQTT server
  loopGenerator.registerDataListener(mainRegulator); // loopGenerator serves the target temperature from the thermostat
  myCHprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WORKDAY,6,30,22, 0));
  myCHprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WEEKEND,8, 0,22, 0));
  mainRegulator.setProgrammer(myCHprogrammer);
  const static char CHprogramTopic[] PROGMEM = "heating_programmer";
  CHprogramProvider.setTopic(DATA_ID_PROGRAM, CHprogramTopic);
  CHprogramProvider.registerDataListener(myCHprogrammer);
  // next two lines make the connection to the wifi thermoelectric valve - to close the heating not to overheat the zone
  const static char livingroomValveTopic[] PROGMEM = "livingroom/valve/set";
  mainRegulator.setActuator(livingroomValveTopic);  

  
  // ------------------------------------------------------------------------------------
  // floor zone in the livingroom
  floorRegulator.attachBoiler(multiRegulator.getZoneBoilerInterface(1));
  floorRegulator.setCommunicator(myCommunicator);
  // floor zone gets the temperature and setpoint from the room thermostat, which is accessible through "loopRegulator"
  loopGenerator.registerDataListener(floorRegulator, DATA_ID_ROOM_TEMPERATURE | DATA_ID_ROOM_TEMPERATURE_SETPOINT);
  floorProgrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 40.0},PROGRAM_DAY_WORKDAY,5,0,6,30));
  floorProgrammer.setProgram(2, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 40.0},PROGRAM_DAY_WEEKEND,7,0,8,30));
  floorProgrammer.setProgram(3, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WORKDAY,6,30,22,0));
  floorProgrammer.setProgram(4, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WEEKEND,8,30,22,0));
  floorRegulator.setProgrammer(floorProgrammer);
  const static char floorProgramTopic[] PROGMEM = "floor_programmer";
  floorProgramProvider.setTopic(DATA_ID_PROGRAM, floorProgramTopic);
  floorProgramProvider.registerDataListener(floorProgrammer);
  const static char floorSmartplugTopic[] PROGMEM = "floor/smartPlug/set";
  floorRegulator.setActuator(floorSmartplugTopic);   

    
  // ------------------------------------------------------------------------------------
  // bedroom zone
  bedroomRegulator.attachBoiler(multiRegulator.getZoneBoilerInterface(2));
  bedroomRegulator.setCommunicator(myCommunicator);
  bedroomProgrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 40.0},PROGRAM_DAY_WORKDAY,6,30,8,30));
  bedroomProgrammer.setProgram(2, Program({.type = PROGRAM_TYPE_PID, .temperature = 19.0},PROGRAM_DAY_ALL,8,30,22,00));
  bedroomRegulator.setProgrammer(bedroomProgrammer);
  const static char bedroomProgramTopic[] PROGMEM = "bedroom_programmer";
  bedroomProgramProvider.setTopic(DATA_ID_PROGRAM, bedroomProgramTopic);
  bedroomProgramProvider.registerDataListener(bedroomProgrammer);
  const static char bedroomValveTopic[] PROGMEM = "bedroom/valve/set";
  bedroomRegulator.setActuator(bedroomValveTopic); 
  // following three lines connect the wifi/MQTT thermometer that will provide the temperature in the bedroom to the bedroom regulator
  const static char bedroomTemperatureTopic[] PROGMEM = "bedroom_temperature";
  bedroomTemperatureProvider.setTopic(DATA_ID_ROOM_TEMPERATURE, bedroomTemperatureTopic);  
  bedroomTemperatureProvider.registerDataListener(bedroomRegulator);
  

    
  // ------------------------------------------------------------------------------------
  // DHW regulator setup is identic to the example #3
  myDHWregulator.attachBoiler(loopGenerator.getDHWBoilerInterface());
  myDHWprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WORKDAY,6, 0,22, 0));
  myDHWprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WEEKEND,7,30,22, 0));
  myDHWregulator.setProgrammer(myDHWprogrammer);
  const static char DHWprogramTopic[] PROGMEM = "DHW_programmer";
  DHWprogramProvider.setTopic(DATA_ID_PROGRAM, DHWprogramTopic);
  DHWprogramProvider.registerDataListener(myDHWprogrammer);  
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
  floorRegulator.loop();
  bedroomRegulator.loop();
}
