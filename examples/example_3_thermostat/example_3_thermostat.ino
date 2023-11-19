/*
 * Example #3 - Thermostat / single PID regulator
 * version: v1.0
 * (AMOR = Arduino Modular Opentherm Regulator)
 * 
 * Example of customized single regulator driven by date-time dependent temperature programs
 * 
 * Hardware configuration is the same as for example #2:
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - ESP8266 wifi module connected to the serial line of arduino
 * 
 * Features - what this example does:
 * - own PID regulator for heating
 * - own regulator for DHW (domestic hot water)
 * - programmed to heat only during day, different times for workday and weekend
 * - thermostat used only to get the room temperatur, target (setpoint) temperature and to display boiler status 
 * - data from boiler sent to the MQTT server 
 * - programms can be modified via MQTT messages
 * 
 * How to make this work
 * - prepare ESP8266 wifi module - see example #2, readme attached to this example and project wiki for more details,
 * - prepare arduino
 *   - copy everything in this file and paste it in the main project file "AMOR_arduino.ino" - replace everything there
 *   - check the "Config.h", keep settings from Example #2. Default values for other config options should work fine
 *     for the first try. Else you are free to modify them, there are verbose comments in the config file that will
 *     guide you.
 *   - compile and upload
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

OpenthermAdapter boiler;
EspMqttCommunicator myCommunicator;
MonitorOTchannel mqttMonitorTherm(myCommunicator);
OTlooper loopGenerator;
const static char mainRegulatorName[] PROGMEM = "livingroom";
SingleRegulator mainRegulator(mainRegulatorName);
Programmer myCHprogrammer;
DHWregulator myDHWregulator;
Programmer myDHWprogrammer;
MqttDataProvider DHWprogramProvider(myCommunicator);
MqttDataProvider CHprogramProvider(myCommunicator);

void setup() {
  Serial.begin(115200);
  delay(50);

  DPRINTLN(F("AMOR: Example 3 - Thermostat / single PID regulator (v1.0)"));
  DPRINTLN(F("----------------------------------------------------------"));

  // connect the MQTT module in between "boiler" and "loop generator"- MQTT module reports to the MQTT server
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
  /* 3 following lines connect the heating programmer to the MQTT topic, then you can modify the program by sending the MQTT message
   * at any time. It expects at the moment a little bit stupid string of 8 numbers separated by columns, whose meaning is following:
   * example: send text "0,130,22,4,6,0,18,30" to the MQTT topic "heating_programmer"
   * which means - modify program 0, set it to PID regulator, it is temporary program (after passing, switch it off to never repeat)
   *              heat to room temperature 22˚C, on Tuesday from 6:00 (6 am) to 18:30 (6:30 pm)
   * number #1 - number of programm - each programmer can have 5 independent programms (numbered 0-4) by default. Can be modified in config.h by
   *             changing the option PROGRAMS_COUNT.
   * number #2 - program type:
   *             0 = OFF - If active, the boiler is switched off. Number #3 is ignored with this program.
   *             1 = fixed temperature of heating water, the boiler will heat the heating water to temperatere given by number #3
   *             2 = PID regulator - boiler will regulate the heating so that the room temperature reaches and keeps the temperature given
   *                 by number #3
   *             3 = PID regulator - the setpoint temperature is taken dynamically from the room thermostat.
   *                  Number #3 is ignored with this program.
   *             +128 = temporary program - if you add 128 to your program, this will be deleted after it finishes. Using this you can
   *                  for example heat up the radiator temporarily for one hour.
   *                  eg. 129 = temporary fixed temperature; 130 = temporary PID regulator
   * number #3 - target temperature, setpoint, different for different program types, see number #2                 
   * number #4 - day - 1 = Sunday, 2 = Monday, 4 = Tuesday, 8 = Wednesday, 16 = Thursday, 32 = Friday, 64 = Saturday
   *              you can combine them - 65 = 1+64 = weekend (Sunday + Saturday); 62 (32 + 16 + 8 + 4 + 2) = all working days
   * number #5 + #6 - hour and minute to start the program      
   * number #7 + #8 - hour and minute to stop the program
   */
  const static char CHprogramTopic[] PROGMEM = "heating_programmer";
  CHprogramProvider.setTopic(DATA_ID_PROGRAM, CHprogramTopic);
  CHprogramProvider.registerDataListener(myCHprogrammer);
  
  // "myDHWregulator" is responsible to tell the loopGenerator the data when to heat the "Domestic Hot Water" (DWH) and to which temperature
  myDHWregulator.attachBoiler(loopGenerator.getDHWBoilerInterface());
  myDHWprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WORKDAY,6, 0,22, 0));
  myDHWprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WEEKEND,7,30,22, 0));
  myDHWregulator.setProgrammer(myDHWprogrammer);
  // 3 following lines connect the programmer for DHW with the MQTT topic which you can use to modify the programm
  // the strategy is identic to what is written at heating programmer few lines above
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
}
