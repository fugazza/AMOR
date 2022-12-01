#include <Arduino.h>
#include <TimeLib.h>
#include "Config.h"
#include "DHWregulator.h"
#include "AbstractBoiler.h"

DHWregulator::attachBoiler(AbstractBoiler &boilerToAttach) {
  _boiler = &boilerToAttach;
}

void DHWregulator::loop() {
  unsigned long tNow = millis();
 
  if (tNow > nextRegulationTime) {
    DPRINT(day()); DPRINT(".");
    DPRINT(month()); DPRINT(".");
    DPRINT(year()); DPRINT(" ");
    DPRINT(hour()); DPRINT(":");
    DPRINT(minute()); DPRINT(":");
    DPRINT(second());
    DPRINT(F(D_DHW_REGULATOR_EVALUATING));

    ProgramData activeProgramData = programmer->getActiveProgramData();
    DPRINT(F(D_DHW_PROGRAM_TYPE));
    DPRINT(activeProgramData.type);
    DPRINT(F(" - "));

    switch (activeProgramData.type & PROGRAM_TYPE_MASK) {
      
      case PROGRAM_TYPE_FIXED_SETPOINT:
        targetDHWtemperature = activeProgramData.temperature;
        if (isDHWTempValid()) {
            _boiler->enableHeating();
            _boiler->setBoilerTemperature(getTargetDHWtemperature());  
            DPRINT(F(D_DHW_ENABLED));  
            DPRINTLN(getTargetDHWtemperature());  
          } else {
            DPRINTLN(F(D_DHW_DISABLED_INVALID_TEMPERATURE));
            _boiler->disableHeating();
          }     
        break;
      
      default:
        _boiler->disableHeating();
        DPRINTLN(F(D_DHW_DISABLED_NO_PROGRAM));
        break;
    } 

    nextRegulationTime = tNow + DHWR_REGULATION_PERIOD;
  }
}

boolean DHWregulator::isDHWTempValid() {
  return getTargetDHWtemperature() > DHWR_MIN_TEMPERATURE && getTargetDHWtemperature() < DHWR_MAX_TEMPERATURE;
}

float DHWregulator::getTargetDHWtemperature() {
  return targetDHWtemperature;
}

void DHWregulator::setProgrammer(Programmer &_programmer) {
  programmer = &_programmer;
}
