#include <Arduino.h>
#include <TimeLib.h>
#include "Config.h"
#include "SingleRegulator.h"
#include "AbstractBoiler.h"
#include "Programmer.h"

SingleRegulator::attachBoiler(AbstractBoiler &boilerToAttach) {
  _boiler = &boilerToAttach;
}

void SingleRegulator::loop() {
  unsigned long tNow = millis();
  bool boilerState = false;
  float88 targetBoilerTemperature = 0.0;
  if (tNow > nextRegulationTime) {
    //setRoomTemperature(20.8);
    DPRINT(day()); DPRINT(".");
    DPRINT(month()); DPRINT(".");
    DPRINT(year()); DPRINT(" ");
    DPRINT(hour()); DPRINT(":");
    DPRINT(minute()); DPRINT(":");
    DPRINT(second());    
    DPRINT(F(D_CH_REGULATOR_EVALUATING));

    ProgramData activeProgram = programmer->getActiveProgramData();

    switch (activeProgram.type & PROGRAM_TYPE_MASK) {
      
      case PROGRAM_TYPE_FIXED_SETPOINT:
        resetPID();
        targetBoilerTemperature = activeProgram.temperature;
        boilerState = true;
        _boiler->enableHeating();
        _boiler->setBoilerTemperature(targetBoilerTemperature);  
        DPRINT(F(D_SR_FIXED_SETPOINT_REGULATION_ENABLED));
        DPRINTLN(targetBoilerTemperature);    
        break;
      
      case PROGRAM_TYPE_PID:
      case PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE:
      
        if (isRoomTempValid()) {
          // we do have an up-to-date room temperature -> we can calculate the target value for heating water
          if (lastRoomTemperature < -90.0) {
            lastRoomTemperature = roomTemperature;
          }
          targetBoilerTemperature = pid(getRoomSetpoint(activeProgram), roomTemperature, lastRoomTemperature, integralError, (tNow-lastRegulationTime)/1000.0);
          DPRINT(targetBoilerTemperature);
          lastRoomTemperature = roomTemperature;
          if (targetBoilerTemperature > (roomTemperature+5)) {
            DPRINTLN(F(D_SR_PID_REGULATION_ENABLED));
            boilerState = true;
            _boiler->enableHeating();
            _boiler->setBoilerTemperature(targetBoilerTemperature);
          } else {
            DPRINTLN(F(D_SR_PID_REGULATION_DISABLED));
            disableHeating();
          }
          
        } else {
          // we do NOT have an up-to-date room temperature -> we have no feedback, we cannot regulate the boiler
          DPRINTLN(F(D_SR_DISABLED_INVALID_TEMP));
          resetPID();
          disableHeating();
        }      
        break;

      case PROGRAM_TYPE_OFF:
      default:
        disableHeating();
        resetPID();
        DPRINTLN(F(D_SR_DISABLED_PROGRAM_OFF));
        break;
    }

    publishData(PSTR(SR_REGULATOR_ON_OFF),          boilerState);
    publishData(PSTR(SR_ROOM_TEMPERATURE),          roomTemperature);
    publishData(PSTR(SR_ROOM_TEMPERATURE_SETPOINT), getRoomSetpoint(activeProgram));
    publishData(PSTR(SR_TARGET_BOILER_TEMPERATURE), targetBoilerTemperature);
    
    lastRegulationTime = tNow;
    nextRegulationTime = tNow + SR_REGULATION_PERIOD;
  }
}

boolean SingleRegulator::isRoomTempValid() {
  DPRINT(F(D_SR_TEMPERATURE_VALIDATOR));
  DPRINT(roomTemperature);
  DPRINT(F(D_SR_TEMP_VALIDATOR_NOW));
  DPRINT(millis());
  DPRINT(F(D_SR_TEMP_VALIDATOR_LAST_TIME));
  DPRINT(lastTempUpdateTime);
  DPRINT(F(D_SR_TEMP_VALIDATOR_RESULT));
  DPRINTLN((millis() - lastTempUpdateTime) < SR_MAX_TEMPERATURE_AGE);
  return ((millis() - lastTempUpdateTime) < SR_MAX_TEMPERATURE_AGE) && (roomTemperature > -99.9);
}

/*
 * sp = SetPoint = target room temperature
 * pv = actual room temperature
 * pv_last = room temperature at last time evaluation
 * ierr = integral error
 * dt =   time between measurements
 */

float SingleRegulator::pid(float sp, float pv, float pv_last, float& ierr, float dt) {
  float Kc = SR_PID_KC; // K - boiler water temperature, when difference between actual and target room temperature is 1°C
  float tauI = SR_PID_TAU_I; // sec - time, after which the 'Kc' degrees are added for boiler water, if the difference between actual and target room temperature is 1°C
  float tauD = SR_PID_TAU_D; // sec - if the room temperature drops at velocity of 1°C per time 'tauD', then the boiler water will be heated up another 'Kc' degrees
                      //       if the room temperature drops 2°C per 'tauD', then it heates 2x'Kc'
                      //       if the room temperature drops 1°C per 4x'tauD', then it heates 1/4x'Kc'
  int PavgCount = 10;
  int DavgCount = 20;
  
  // PID coefficients
  float KP = Kc;
  float KI = Kc / tauI;
  float KD = Kc*tauD; 
  // upper and lower bounds on heater level
  float ophi = SR_PID_MAX_WATER_TEMP;
  float oplo = SR_PID_MIN_WATER_TEMP;
  // calculate the error
  float error = sp - pv;
  // calculate the integral error
  ierr = ierr + KI * error * dt;  
  // calculate the measurement derivative
  float dpv = (pv - pv_last) / dt;
  // calculate the PID output
  float P = KP * error / PavgCount + PP * (PavgCount-1) / PavgCount; //proportional contribution - with averaging
  float I = ierr; //integral contribution
  float D = -KD * dpv / DavgCount + DD * (DavgCount-1) / DavgCount; //derivative contribution - with averaging
  float op = pv + P + I + D;
  // implement anti-reset windup
  if ((op < oplo) || (op > ophi)) {
    I = I - KI * error * dt;
    // clip output
    op = max(oplo, min(ophi, op));
  }
  ierr = I; 
  PP = P;
  DD = D;
  DPRINTLN("sp="+String(sp) + " pv=" + String(pv) + " dt=" + String(dt) + " op=" + String(op) + " P=" + String(P) + " I=" + String(I) + " D=" + String(D));
  return op;
}

void SingleRegulator::disableHeating() {
  _boiler->disableHeating();
  _boiler->setBoilerTemperature(0.0);
}

void SingleRegulator::resetPID() {
  lastRoomTemperature = -100.0;
  integralError = 0.0;  
}

void SingleRegulator::setProgrammer(Programmer &_programmer) {
  programmer = &_programmer;
}

float SingleRegulator::getRoomTemperature() {
  return (isRoomTempValid()) ? (float) roomTemperature : 0.0;
}

float SingleRegulator::getRoomSetpoint() {
  return getRoomSetpoint(programmer->getActiveProgramData());
}

float SingleRegulator::getRoomSetpoint(ProgramData activeProgram) {
  switch (activeProgram.type & PROGRAM_TYPE_MASK) {
    case PROGRAM_TYPE_OFF:
    case PROGRAM_TYPE_FIXED_SETPOINT:
      return PROGRAM_DEFAULT_TEMPERATURE;
      break;
    case PROGRAM_TYPE_PID:
      return activeProgram.temperature;
      break;
    case PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE:
      return _roomSetpointFromThermostat;
      break;
    default: 
      return PROGRAM_DEFAULT_TEMPERATURE;
      break;    
  }
}

void SingleRegulator::dataUpdated(byte dataID, float88 value) {
  switch (dataID) {
    case DATA_ID_ROOM_TEMPERATURE:
      roomTemperature = value;
      lastTempUpdateTime = millis();
      DPRINT(F(D_SR_ROOM_TEMPERATURE_UPDATED));
      DPRINTLN(roomTemperature);    
      break;
    case DATA_ID_ROOM_TEMPERATURE_SETPOINT:
      _roomSetpointFromThermostat = value;
      break;
    case DATA_ID_OUTSIDE_TEMPERATURE:
      _outsideTemperature = value;
      break;
  }
  
}

void SingleRegulator::setCommunicator(AbstractCommunicator &newCommunicator) {
  _communicator = &newCommunicator;
}

void SingleRegulator::publishData(const char *dataName, float value) {
  if (_communicator != nullptr) {
    char topicBuffer[64];
    char valueBuffer[16];
    strcpy_P(topicBuffer,_name);
    strcat_P(topicBuffer,PSTR("/"));
    strcat_P(topicBuffer,dataName);
    dtostrf(value,4,2,valueBuffer);
    _communicator->publishTopic(topicBuffer, valueBuffer);
  }
}

void SingleRegulator::publishData(const char *dataName, bool value) {
  if (_communicator != nullptr) {
    char topicBuffer[64];
    char valueBuffer[16];
    strcpy_P(topicBuffer,_name);
    strcat_P(topicBuffer,PSTR("/"));
    strcat_P(topicBuffer,dataName);
    strcpy(valueBuffer, value ? "1" : "0");
    _communicator->publishTopic(topicBuffer, valueBuffer);
  }
}
