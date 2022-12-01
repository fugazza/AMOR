#ifndef OpenthermAdapter_h
#define OpenthermAdapter_h

#include <opentherm.h>

#include "AbstractOTchannel.h"
#include "Config.h"

// pin numbers, where OpenTherm Shield is connected
// must be defined in "Config.h"
// here commented and should stay commented only as a backup info, which constants are used by this module
// Arduino UNO
// #define THERMOSTAT_IN 2
// #define THERMOSTAT_OUT 4
// #define BOILER_IN 3
// #define BOILER_OUT 5

#define MODE_LISTEN_MASTER 0
#define MODE_LISTEN_SLAVE 1


class OpenthermAdapter : public AbstractOTchannel
{
  public:
    OpenthermAdapter();
    void loop();
    void boilerMessageReceived(OpenthermData &boilerMessage);
    void thermostatMessageReceived(OpenthermData &thermostatMessage);
    void sendToThermostat(OpenthermData &messageForThermostat);
    void sendToBoiler(OpenthermData &messageForBoiler); 
    void throwError();   

  private:
    int mode = MODE_LISTEN_MASTER;
    OpenthermData message;
};
#endif
