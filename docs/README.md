# AMOR
AMOR = **A**rduino **M**odular **O**penTherm **R**egulator

AMOR is a framework which enables you to build your own HVAC regulator on Arduino and customize it to your needs.

## What you can make with AMOR
- **gateway**, man in the middle, message sniffer to see, how the thermostat is communicating with the boiler
- **MQTT gateway** which will send the data about boiler and thermostat to your MQTT server, from which you can visualise them or use them further in your *smart home* software like [OpenHAB](https://www.openhab.org) or [Home Assistant](https://www.home-assistant.io)
- **your own thermostat / regulator** - in case you are not satisfied with the thermostat provided by your boiler manufacturer. You can make several different temperature *programs* based on day and time. You can choose between *PID* and *fixed temperature* regulators. You can decide, if the room temperature is obtained from *thermostat*, or from configurable *MQTT topic*.
- **multizone thermostat** - you can combine multiple simple regulators, each using its own programmer and source of temperature, in order to heat each room of your home independently

## Hardware you will need
- Boiler capable to communicate by OpenTherm protocol
- Arduino UNO
- [OpenTherm shield](https://www.tindie.com/products/jiripraus/opentherm-gateway-arduino-shield)
- Optional: ESP8266 wifi module - optimal is to have an [Arduino UNO clone with ESP8266 chip on single bord](https://www.laskakit.cz/arduino-uno-wifi-atmega328p-esp8266-4mb)
- Optional: Room thermostat communicating by OpenTherm protocol - usually provided together with boiler

## Main components
- **AMOR_arduino** is the sketch for Arduino UNO equipped by OpenTherm shield. This is the main component, where all magic happens.
- **esp8266_mqtt_firmware** is the sketch for ESP8266 chip. You will need it in case that you intend to send and receive data from wifi. This firmware is responsible for connection to wifi, connection to the MQTT server, for publishing MQTT messages and for listening to subscribed MQTT topics. Important function of this firmware is also to serve an exact date-time to Arduino. Communication between ESP8266 and Arduino is made through serial port using simple protocol.

## Key features
- **modularity**
  - You can use only the modules you really need.
  - You can modify the module that does not fit your needs or write your own module, without need to take care of other modules that work well.
- **independency**
  - Home heating is "critical" area, you want to work almost under any conditions. Sure you want it to run even if your wifi goes down and servers collapse. Therefore AMOR is designed to run on single independent device (Arduino), containing all its configuration and programmes in its memory. It can integrate with *smart home* (using MQTT), but does not depend on it.
- **programmes**
  - Each regulator (thermostat) can be programmed in a way you know from standard thermostats - you can provide different target temperature for different days and time.
- **optimized memory footprint**
  - AMOR was written with special care to fit in Arduino UNO's SRAM memory, which has only 2kb, even in relative large multizone regulator setup, still leaving enough memory free, in order to ensure long term stability

## Arduino libraries you need to install
AMOR uses internally following Arduino libraries, that you have to install:
- **OpenTherm** <opentherm.h> by Jiri Praus - v1.1.0
- **Time** <TimeLib.h> by Paul Stoffregen - v1.6.1
- **CircularBuffer** <CircularBuffer.h> by Roberto Lo Giacco - v1.3.3

and ESP8266_firmware uses also:
- **PubSubClient** <PubSubClient.h> by Nick O'Leary - v2.8.0
- **TickTwo** <TickTwo.h> by StefanStaub - v4.4.0 
- **NTPclient** <NTPClient.h> by Fabrice Weinberg - v3.2.1

## Example
Here you are medium-complexity example, how you can define and customize your regulator. 
```
/*
 * AMOR example - own thermostat
 * - Arduino UNO with OpenTherm shield placed in between boiler and thermostat, acting as gateway / man in the middle
 * - own PID regulator for heating
 * - own regulator for DHW (domestic hot water)
 * - programmed to heat only during day, different times for workday and weekend
 * - thermostat used only to get the target temperature and to display boiler status 
 * - data from boiler sent to the MQTT server
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

  DPRINTLN(F("AMOR example - own regulator"));
  DPRINTLN(F("----------------------------"));
  
  // connect the MQTT module in between "boiler" and "loop generator"- MQTT module reports to the MQTT server all the data from OpenTherm massages passing through it
  mqttMonitorTherm.attachOTchannel(boiler);
  // OpenTherm protocol requires that thermostat (master) send every second some OpenTherm message to the boiler (slave) - module "loopGenerator" does this job, generates these messages every second
  // "loopGenerator" is also responsible to answer OpenTherm messages incoming from the thermostat interface on OpenTherm shield and to harvest any necessary data from it
  loopGenerator.attachOTchannel(mqttMonitorTherm);
  
  // "mainRegulator" is a PID regulator, that calculates every minute the temperature of heating water, which is necessary to reach the target room temperature
  mainRegulator.attachBoiler(loopGenerator.getCHBoilerInterface());
  mainRegulator.setCommunicator(myCommunicator); // necessary if you want the regulator to publish its data to MQTT server
  loopGenerator.registerDataListener(mainRegulator); // loopGenerator serves the target temperature from the thermostat
  myCHprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WORKDAY,6,30,22,0));
  myCHprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE, .temperature = 21.0},PROGRAM_DAY_WEEKEND,8,30,22,0));
  mainRegulator.setProgrammer(myCHprogrammer);
  
  // "mainRegulator" is responsible to tell the loopGenerator the data when to heat the "Domestic Hot Water" (DWH) and to which temperature
  myDHWregulator.attachBoiler(loopGenerator.getDHWBoilerInterface());
  myDHWprogrammer.setProgram(0, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WORKDAY,6, 0,22,0));
  myDHWprogrammer.setProgram(1, Program({.type = PROGRAM_TYPE_FIXED_SETPOINT, .temperature = 45.0},PROGRAM_DAY_WEEKEND,7,30,22,0));
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
```
