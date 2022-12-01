#ifndef MonitorOTchannel_h
#define MonitorOTchannel_h

#include <opentherm.h>

#include "AbstractOTchannel.h"
#include "AbstractCommunicator.h"

#define OT_MSGTYPE_TIMEOUT_ERROR B011

class MonitorOTchannel : public AbstractOTchannel
{
  public:
    MonitorOTchannel(AbstractCommunicator &communicator);
    void boilerMessageReceived(OpenthermData &boilerMessage);
    void thermostatMessageReceived(OpenthermData &thermostatMessage);
    void sendToThermostat(OpenthermData &message);
    void sendToBoiler(OpenthermData &message);
    void throwError();

  private:
    AbstractCommunicator *_communicator;
    OpenthermData _requestMessage;
    const OpenthermData errorMessage = {.type = OT_MSGTYPE_TIMEOUT_ERROR, .id = 255, .valueHB = 0, .valueLB = 0};
};
#endif
