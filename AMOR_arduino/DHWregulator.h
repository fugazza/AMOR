#ifndef DHWregulator_h
#define DHWregulator_h

class DHWregulator;
#include "OTlooper.h"
#include "Programmer.h"

// period to evaluate the regulation -  60.000 ms = 1 min - sufficient enough for heating of DHW (domestic home water)
#ifndef DHWR_REGULATION_PERIOD
  #define DHWR_REGULATION_PERIOD 60000
#endif
#ifndef DHWR_MIN_TEMPERATURE
  #define DHWR_MIN_TEMPERATURE 0.0
#endif
#ifndef DHWR_MAX_TEMPERATURE
  #define DHWR_MAX_TEMPERATURE 70.0
#endif

class DHWregulator
{
  public:
    attachBoiler(AbstractBoiler &boilerToAttach);
    
    void loop();
    void setProgrammer(Programmer &_programmer);

  private:
    AbstractBoiler *_boiler;
    unsigned long nextRegulationTime = 0;
    Programmer *programmer;
    float targetDHWtemperature = 45.0;

    boolean isDHWTempValid();
    float getTargetDHWtemperature();
   
};
#endif
