#ifndef SingleRegulator_h
#define SingleRegulator_h

class SingleRegulator;
#include "AbstractBoiler.h"
#include "AbstractCommunicator.h"
#include "Programmer.h"
#include "Float88DataType.h"
#include "DataListener.h"

// period to evaluate the regulation -  60.000 ms = 1 min - sufficient enough for heating
#ifndef SR_REGULATION_PERIOD
  #define SR_REGULATION_PERIOD 60000l
#endif

// how old can be the room temperature to be trustworthy - 300.000ms = 5 min
#ifndef SR_MAX_TEMPERATURE_AGE
  #define SR_MAX_TEMPERATURE_AGE 300000l
#endif

#ifndef SR_REGULATOR_ON_OFF
  #define SR_REGULATOR_ON_OFF          "boilerState"
#endif
#ifndef SR_ROOM_TEMPERATURE
  #define SR_ROOM_TEMPERATURE          "temperature"
#endif
#ifndef SR_ROOM_TEMPERATURE_SETPOINT
  #define SR_ROOM_TEMPERATURE_SETPOINT "setpoint"
#endif
#ifndef SR_TARGET_BOILER_TEMPERATURE
  #define SR_TARGET_BOILER_TEMPERATURE "boilerTemperature" 
#endif

#ifndef SR_PID_KC
  #define SR_PID_KC 25.0
#endif
#ifndef SR_PID_TAU_I
  #define SR_PID_TAU_I 500.0
#endif
#ifndef SR_PID_TAU_D
  #define SR_PID_TAU_D 1200.0
#endif

#ifndef SR_PID_MAX_WATER_TEMP
  #define SR_PID_MAX_WATER_TEMP 55
#endif
#ifndef SR_PID_MIN_WATER_TEMP
  #define SR_PID_MIN_WATER_TEMP 20
#endif

class SingleRegulator: public DataListener
{
  public:
    SingleRegulator(const char *regulatorName) : _name(regulatorName) {};
    attachBoiler(AbstractBoiler &boilerToAttach);
    
    void  loop();
    float getRoomTemperature();
    float getRoomSetpoint();
    float getRoomSetpoint(ProgramData activeProgram);
    void  setProgrammer(Programmer &_programmer);
    void  dataUpdated(byte dataID, const char *value) {};
    void  dataUpdated(byte dataID, float88 value);
    void  setCommunicator(AbstractCommunicator &newCommunicator);

  private:
    const char *_name;
    AbstractBoiler *_boiler;
    AbstractCommunicator *_communicator = nullptr;
    float88 roomTemperature=-100.0;
    float88 lastRoomTemperature=-100.0;
    float88 _roomSetpointFromThermostat = 21.0;
    float88 _outsideTemperature;
    unsigned long nextRegulationTime = 0;
    unsigned long lastRegulationTime = 0;
    unsigned long lastTempUpdateTime = 0;
    float integralError = 0.0;
    float PP = 0.0;
    float DD = 0.0;
    Programmer *programmer;   

    boolean isRoomTempValid();
    float pid(float sp, float pv, float pv_last, float& ierr, float dt);
    float getTargetTemperature();
    void disableHeating();
    void resetPID();
    void publishData(const char *dataName, float value);
    void publishData(const char *dataName, bool value); 
   
};
#endif
