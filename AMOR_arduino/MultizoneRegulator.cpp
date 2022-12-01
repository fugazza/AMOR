#include <Arduino.h>
#include "MultizoneRegulator.h"
#include "AbstractBoiler.h"
#include "Float88DataType.h"

MultizoneRegulator::MultizoneRegulator() {
  for (int i=0; i<MULTI_ZONES_COUNT; i++) {
    _zoneBoilerInterfaces[i].setParentAndNum(this, i);
    zoneEnabled[i] = false;
    zoneTemperature[i] = 0.0;  
  }
}

void MultizoneRegulator::ZoneBoilerInterface::setParentAndNum(MultizoneRegulator *parent, byte num) {
  _parent = parent;
  _zoneNum = num;
}

void MultizoneRegulator::attachBoiler(AbstractBoiler &boilerToAttach) {
  _boiler = &boilerToAttach;
}

void MultizoneRegulator::ZoneBoilerInterface::enableHeating()  {
  _parent->zoneEnabled[_zoneNum] = true;
  _parent->evaluateChanges();
}

void MultizoneRegulator::ZoneBoilerInterface::disableHeating() {
  _parent->zoneEnabled[_zoneNum] = false;
  _parent->evaluateChanges();
}

void MultizoneRegulator::ZoneBoilerInterface::setBoilerTemperature(float88 newTemperature) {
  _parent->zoneTemperature[_zoneNum] = newTemperature;
  _parent->evaluateChanges();  
}


MultizoneRegulator::ZoneBoilerInterface& MultizoneRegulator::getZoneBoilerInterface(int i) {
  return _zoneBoilerInterfaces[i];
}

void MultizoneRegulator::evaluateChanges() {
  bool newEnabled = false;
  float88 newTemperature = 0.0;
  for (int i=0; i<MULTI_ZONES_COUNT; i++) {
    newEnabled = newEnabled || zoneEnabled[i];
    if (zoneEnabled[i]) {
      newTemperature = max(newTemperature, zoneTemperature[i]);
    }
  }

  if (newEnabled != boilerEnabled) {
    boilerEnabled = newEnabled;
    (boilerEnabled) ? _boiler->enableHeating() : _boiler->disableHeating();
  }

  if (newTemperature != boilerTemperature) {
    boilerTemperature = newTemperature;
    _boiler->setBoilerTemperature(newTemperature);
  }
}
