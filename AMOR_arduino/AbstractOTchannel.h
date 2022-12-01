#ifndef AbstractOTchannel_h
#define AbstractOTchannel_h

#include <opentherm.h>

class AbstractOTchannel
{
  public:
    
    virtual void boilerMessageReceived(OpenthermData &boilerMessage) = 0;
    virtual void thermostatMessageReceived(OpenthermData &thermostatMessage) = 0;
    virtual void sendToThermostat(OpenthermData &message) = 0;
    virtual void sendToBoiler(OpenthermData &message) = 0;
    virtual void throwError() = 0;

    void attachOTchannel(AbstractOTchannel &childOTtoAttach);
    void setParrentOTchannel(AbstractOTchannel &parentOTchannelToAttach);

  protected:
    AbstractOTchannel *childOT;
    AbstractOTchannel *parentOT;
  
};
#endif
