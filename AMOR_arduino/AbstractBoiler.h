#ifndef AbstractBoiler_h
#define AbstractBoiler_h

#include "Float88DataType.h"

class AbstractBoiler
{
  public:
    virtual void enableHeating() = 0;
    virtual void disableHeating() = 0;
    virtual void setBoilerTemperature(float88 newBoilerTemperature) = 0;
   
};
#endif
