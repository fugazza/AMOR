#ifndef OTlooper_h
#define OTlooper_h

#include <CircularBuffer.h>
#include <opentherm.h>

#include "AbstractOTchannel.h"
#include "AbstractBoiler.h"
#include "Float88DataType.h"

class OTlooper;
#include "DataPublisher.h"

// pause between messages, according to specification pause from last answer to next command should be at least 100ms
// but max. total time between two messages is 1.15s (1150ms)
#ifndef PAUSE_BETWEEN_MESSAGES
  #define PAUSE_BETWEEN_MESSAGES 500
#endif

#ifndef MIN_BOILER_TEMPERATURE
  #define MIN_BOILER_TEMPERATURE 0.0
#endif

#ifndef MAX_BOILER_TEMPERATURE
  #define MAX_BOILER_TEMPERATURE 100.0
#endif

class OTlooper : public AbstractOTchannel, public DataPublisher
{

  class CHBoilerInterface : public AbstractBoiler
  {
    public:
      OTlooper *_parent;
      CHBoilerInterface(OTlooper *parent): _parent(parent) {};
      void enableHeating();
      void disableHeating();
      void setBoilerTemperature(float88 newBoilerTemperature);      
  };


  class DHWBoilerInterface : public AbstractBoiler
  {
    public:    
      OTlooper *_parent;
      DHWBoilerInterface(OTlooper *parent) : _parent(parent) {};
      void enableHeating();
      void disableHeating();
      void setBoilerTemperature(float88 newBoilerTemperature);      
  };
      
  public:
    OTlooper();
    void loop();

    void boilerMessageReceived(OpenthermData &boilerMessage);
    void thermostatMessageReceived(OpenthermData &thermostatMessage);
    void sendToThermostat(OpenthermData &messageForThermostat);
    void sendToBoiler(OpenthermData &messageForBoiler); 
    void throwError(); 

    CHBoilerInterface& getCHBoilerInterface();
    DHWBoilerInterface& getDHWBoilerInterface();  

  private:
    unsigned long nextMessageTime;
    boolean lastCommandWasFromRepeatedQueue = false;
    CircularBuffer<byte,16> singleUseCommands;
    CircularBuffer<byte,16> repeatedCommands; 
    float88 boilerWaterTemperature;
    float88 _targetBoilerTemperature;
    float88 outsideTemperature;
    boolean _CHenabled = false;
    boolean _DHWenabled = false;
    byte _boilerStatus = 0;
    uint16_t _boilerFaultFlags = 0;
    uint16_t _boilerOEMDiagnosticCode = 0;
    float88 _targetDHWtemperature;
    byte memberID = 56;

    unsigned long oneMinuteTickerTime;  
    unsigned long fiveMinuteTickerTime;  

    OpenthermData buildOTmessage(int messageID);
    OpenthermData buildAnswerForThermostat(OpenthermData thermostatRequestMessage);
    byte composeMasterStatusByte();
    boolean isCHenabled();
    float getCHsetpoint();
    boolean isBoilerTemperatureValid(float boilerTemperature);
    boolean isDHWenabled(); 
    float getDHWsetpoint();
    float getBoilerWaterTemperature();

    void oneMinuteCallback();
    void fiveMinutesCallback();  

    CHBoilerInterface _CHBoilerInterface;
    DHWBoilerInterface _DHWBoilerInterface;
  
};
#endif
