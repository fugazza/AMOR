#include <Arduino.h>
#include <opentherm.h>
#include "OpenthermAdapter.h"

/*
 * constructor
 */
OpenthermAdapter::OpenthermAdapter()
{
  pinMode(THERMOSTAT_IN, INPUT);
  digitalWrite(THERMOSTAT_IN, HIGH); // pull up
  digitalWrite(THERMOSTAT_OUT, HIGH);
  pinMode(THERMOSTAT_OUT, OUTPUT); // low output = high current, high output = low current
  pinMode(BOILER_IN, INPUT);
  digitalWrite(BOILER_IN, HIGH); // pull up
  digitalWrite(BOILER_OUT, HIGH);
  pinMode(BOILER_OUT, OUTPUT); // low output = high voltage, high output = low voltage
}

/*
 * main loop, where all communication with OpenthermShield happens
 */

void OpenthermAdapter::loop()
{

  // loop for listening to incoming messages and to drive the switching between the boiler and the thermostat

  // "thermostat" section
  if (mode == MODE_LISTEN_MASTER) {
    // listening to the thermostat    
    if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError()) {
      OPENTHERM::listen(THERMOSTAT_IN);
    }
    else if (OPENTHERM::getMessage(message)) {
      // it the thermostat send a message, pass it to the parent module
      parentOT->thermostatMessageReceived(message);
    }
  }
  // "boiler" section
  else if (mode == MODE_LISTEN_SLAVE) {
    if (OPENTHERM::isSent()) {
      OPENTHERM::listen(BOILER_IN, 800); // response need to be send back by boiler within 800ms
    }
    else if (OPENTHERM::getMessage(message)) {
      mode = MODE_LISTEN_MASTER;
      parentOT->boilerMessageReceived(message);
    }
    else if (OPENTHERM::isError()) {
      mode = MODE_LISTEN_MASTER;
      parentOT->throwError();
    }
  }
}

void OpenthermAdapter::boilerMessageReceived(OpenthermData &boilerMessage) {
  // do nothing, the only messages that this module can receive are the ones received from OpenThermShield
}


void OpenthermAdapter::thermostatMessageReceived(OpenthermData &thermostatMessage) {
  // do nothing, the only messages that this module can receive are the ones received from OpenThermShield
}

void OpenthermAdapter::sendToThermostat(OpenthermData &messageForThermostat) {
  OPENTHERM::send(THERMOSTAT_OUT, messageForThermostat); // send message to the thermostat
  mode = MODE_LISTEN_MASTER;
}

void OpenthermAdapter::sendToBoiler(OpenthermData &messageForBoiler) {
  OPENTHERM::send(BOILER_OUT, messageForBoiler); // sennd message to the boiler
  mode = MODE_LISTEN_SLAVE;
}

void OpenthermAdapter::throwError() {
  // just forward the error to the parent
  parentOT->throwError();
}
