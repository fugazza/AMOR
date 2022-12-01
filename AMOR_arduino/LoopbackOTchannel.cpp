#include <Arduino.h>
#include <opentherm.h>
#include "LoopbackOTchannel.h"

/*
 * constructor
 */
LoopbackOTchannel::LoopbackOTchannel()
{

}

void LoopbackOTchannel::boilerMessageReceived(OpenthermData &boilerMessage) {
  childOT->sendToThermostat(boilerMessage);
}


void LoopbackOTchannel::thermostatMessageReceived(OpenthermData &thermostatMessage) {
  childOT->sendToBoiler(thermostatMessage);
}

void LoopbackOTchannel::sendToThermostat(OpenthermData &message) {
  // do nothing, LoopbackOTchannel cannot be attached as a child to another channel
}

void LoopbackOTchannel::sendToBoiler(OpenthermData &message) {
  // do nothing, LoopbackOTchannel cannot be attached as a child to another channel
}

void LoopbackOTchannel::throwError() {
  // do nothing, do not care about errors
}
