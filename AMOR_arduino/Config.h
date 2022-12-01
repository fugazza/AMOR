#ifndef config_h
#define config_h

/* 
 * language --------------------------------------------------------------------------------------------------------------
 * only two languages are available at the moment
 * uncomment your preferred one and comment the other
 */
#include "lang_en.h"      // english
//#include "lang_cs.h"      // czech

/* 
 *  debugging info -------------------------------------------------------------------------------------------------------
 */
/* omment this line to disable output of debug messages to Serial */
//#define DEBUG  

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif

/*
 * constants for module "OpenThermAdapter" ---------------------------------------------------------------------------------
 */

/* definition of pins, to which the OpenTherm Shield is connected */
/* Arduino UNO */
#define THERMOSTAT_IN 2
#define THERMOSTAT_OUT 4
#define BOILER_IN 3
#define BOILER_OUT 5

/* Wemos D1 R1 */
// #define THERMOSTAT_IN 16
// #define THERMOSTAT_OUT 4
// #define BOILER_IN 5
// #define BOILER_OUT 14

/*
 * constants for module "SingleRegulator" -------------------------------------------------------------------------------------
 */
 
/* period to evaluate the regulation -  60.000 ms = 1 min - sufficient enough for heating 
 * default value 60000l
 */
// #define SR_REGULATION_PERIOD 60000l

/* how old can be the room temperature to belive it is really up to date - 300.000ms = 5 min
 * if the last update of the room temperature is older than this constant, then the regulator switches the heating off
 * default value 300000l
 */
// #define SR_MAX_TEMPERATURE_AGE 300000l

/* name of MQTT topics, under which the regulator stores its actual status and data
 * default values:
 *   SR_REGULATOR_ON_OFF          = "boilerState"
 *   SR_ROOM_TEMPERATURE          = "temperature"
 *   SR_ROOM_TEMPERATURE_SETPOINT = "setpoint"
 *   SR_TARGET_BOILER_TEMPERATURE = "boilerTemperature" 
 */
// #define SR_REGULATOR_ON_OFF          "boilerState"
// #define SR_ROOM_TEMPERATURE          "temperature"
// #define SR_ROOM_TEMPERATURE_SETPOINT "setpoint"
// #define SR_TARGET_BOILER_TEMPERATURE "boilerTemperature" 

/* PID regulator constants */
/* Kc = proportional constant
 * boiler water temperature, when difference between actual and target room temperature is 1°C
 * e.g. room temperature is 18°C, target temperature is 21°C -> difference is 3°C -> boiler is targeted to heat the heating water to 3xKc = 3x25 = 75°C
 *      (or maximum allowable boiler temperature, see consatnts below)
 *      think - if you want to have temperature in your room 3°C more than you actually have, your boiler will have to heat on full power for several hours
 * default value 25.0
 */
// #define SR_PID_KC 25.0

/* Tau_I = integral constant
 * time (in seconds), after which the 'Kc' degrees are added for boiler water, if the difference between actual and target room temperature is 1°C
 * e.g. your boiler is already heating at 40°C, your room temperature is still 0.2°C below your target and it did not change for last 500 seconds (ca 8 minutes)
 *      your proportional constant does not raise the boiler temperature any more, it is already included in acutal 40°C of boiler water, but it is necessary
 *      to add the temperature to reach the target - the integral constant adds another 25 * 0.2 = 5°C, so that for next period the boiler water temperature is 45°C
 *      it means that each 500 seconds another 5°C are added to the boiler water until the room temperature raises or max. boiler water is reached
 * default value 500.0
 */
// #define SR_PID_TAU_I 500.0

/* Tau_D = derivative constant
 * sec - if the room temperature drops at velocity of 1°C per time 'tauD', then the boiler water will be heated up another 'Kc' degrees
 *       if the room temperature drops 2°C per 'tauD', then it heates 2x'Kc'
 *       if the room temperature drops 1°C per 4x'tauD', then it heates 1/4x'Kc'
 * default value 1200.0           
 */
// #define SR_PID_TAU_D 1200.0

/* Minimum and maximum allowable temperatures for boiler water
 * Should the boiler water temperature calclulated in PID regulator exceed these bounds, the output temperature is limited
 * so that the boiler water temperature never exceeds these limits
 * dafault values:
 *   SR_PID_MIN_WATER_TEMP = 20°C
 *   SR_PID_MAX_WATER_TEMP = 55°C
 */
// #define SR_PID_MIN_WATER_TEMP 20
// #define SR_PID_MAX_WATER_TEMP 55

 
/*
 * constants for module "DHWregulator" -------------------------------------------------------------------------------------
 */

/* period to evaluate the regulation -  60.000 ms = 1 min - sufficient enough for heating of DHW (domestic home water)
 * default value 60000
 */
// #define DHWR_REGULATION_PERIOD 60000l

/* minimum allowed temperature for DHW - temperatures below are discarded as invalid
 * default value 0.0
 */
// #define DHWR_MIN_TEMPERATURE 0.0

/* maximum allowed temperature for DHW - temperatures above are discarded as invalid
 * default value 70.0
 */
// #define DHWR_MAX_TEMPERATURE 70.0

/*
 * constants for module "DataPublisher" -------------------------------------------------------------------------------------
 */
 
/* 
 * size of internal storage in "DataPublisher" for "DataListeners" - meaning to how many DataListeners one Publisher can provide data
 * set smaller values if you need to save SRAM memory
 * set bigger if any of your DataPublishers serves more listeners - typically more listeners are getting
 * outer temperature from thermostat viwOTlooper
 * default value 3
 */
// #define DATA_LISTENERS_SIZE 3


/*
 * constants for module "EspMqttCommunicator" -------------------------------------------------------------------------------------
 */
/* name of the base topic, under which all MQTT messages are published
 * default value "boiler/"
 */
// #define MQTT_BASE_TOPIC "boiler/"

/* to reduce the amount of messages to MQTT server, the messages coming from boiler are cached and are published only if they change
 * or if they expire - here you can specify the expiration period
 * default value 300.000 = 300 s = 5 min
 */
// #define MIN_UPDATE_TIME 300000 

/* this value sets the size of the cache
 * reduce if you need more bytes in SRAM
 * increase if your MQTT server gets spammed with identic values too often (like each 10 seconds)
 * default value 20
 */
// #define OLD_VAL_SIZE 20

/*
 * constants common for modules "EspMqttCommunicator" and "SerialCommunicator" ----------------------------------------
 */
/* this is the size of array, where "MqttDataProvider"s are stored to get announced when communicator receives
 * new message in subscribed topis
 * reduce if you need more bytes in SRAM
 * increase if you use more "MqttDataProvider" instances with the communicator
 * default value 10
 */
// #define PROVIDERS_LIST_SIZE 10


/*
 * constants for module "OTlooper" -------------------------------------------------------------------------------------
 */
/* pause between OpenTherm messages, which are generated in endless loop by "OTlooper"
 * according to specification pause from last answer to next command should be at least 100ms
 * but max. total time between two messages is 1.15s (1150ms)
 * default value 500
 */
// #define PAUSE_BETWEEN_MESSAGES 500

/* minimum allowable temperature on the output from boiler - lower values are discarded and not taken into account
 * default value 0.0
 */
// MIN_BOILER_TEMPERATURE 0.0

/* maximum allowable temperature on the output from boiler - bigger values are discarded and not taken into account
 * default value 100.0
 */
// MAX_BOILER_TEMPERATURE 100.0

/*
 * constants for module "Programmer" -------------------------------------------------------------------------------------
 */
/* the room setpoint temperature, which is provided by the programmer in cases, when the program does not use the setpoint
 * e.g. when the program is of the type PROGRAM_TYPE_OFF or PROGRAM_TYPE_FIXED_SETPOINT
 * default value 10.0
 */
// #define PROGRAM_DEFAULT_TEMPERATURE 10.0

/* how many programs each programmer can have
 * decrese if you need more SRAM and if you do not use much programs within one programmer
 * the memory consumption of one program is considerable, although it is optimized down to 8 bytes per program
 * if PROGRAMS_COUNT=5 and you have 4 programmers, then they occupy 5 * 4 * 8 = 160 bytes
 * if PROGRAMS_COUNT=10 and you have 6 programmers, then they occupy 10 * 6 * 8 = 480 bytes, which is a lot from 
 * 2048 bytes available on Arduino UNO
 * default value 5
 */
// #define PROGRAMS_COUNT   5

#endif
