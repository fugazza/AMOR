#include <Arduino.h>
#include <opentherm.h>
#include "Config.h"
#include "MonitorOTchannel.h"
#include "AbstractCommunicator.h"

/*
 * constructor
 */
MonitorOTchannel::MonitorOTchannel(AbstractCommunicator &communicator)
{
  _communicator = &communicator;
}

void MonitorOTchannel::boilerMessageReceived(OpenthermData &boilerMessage) {
  //DPRINT(F("monitor: boiler message received ..."));
  _communicator->communicateMessage(_requestMessage, boilerMessage);
  //DPRINT(F("monitor: forwarding to parent ..."));
  parentOT->boilerMessageReceived(boilerMessage);
  //DPRINTLN(F("monitor: done"));
}


void MonitorOTchannel::thermostatMessageReceived(OpenthermData &thermostatMessage) {
  parentOT->thermostatMessageReceived(thermostatMessage);
}

void MonitorOTchannel::sendToThermostat(OpenthermData &message) {
  childOT->sendToThermostat(message);
}

void MonitorOTchannel::sendToBoiler(OpenthermData &message) {
  //_communicator->communicateMessage(message);
  _requestMessage = message;
  childOT->sendToBoiler(message);
}

void MonitorOTchannel::throwError() {
  _communicator->communicateMessage(_requestMessage, errorMessage);
  parentOT->throwError();
}
