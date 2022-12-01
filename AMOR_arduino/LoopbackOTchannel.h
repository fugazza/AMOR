#ifndef LoopbackOTchannel_h
#define LoopbackOTchannel_h

#include <opentherm.h>

#include "AbstractOTchannel.h"

class LoopbackOTchannel : public AbstractOTchannel
{
  public:
    LoopbackOTchannel();
    void boilerMessageReceived(OpenthermData &boilerMessage);
    void thermostatMessageReceived(OpenthermData &thermostatMessage);
    void sendToThermostat(OpenthermData &message);
    void sendToBoiler(OpenthermData &message);
    void throwError();

  private:
  
};
#endif
