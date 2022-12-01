#ifndef MultizoneRegulator_h
#define MultizoneRegulator_h

class MultizoneRegulator;
#include "AbstractBoiler.h"
#include "Float88DataType.h"

#define MULTI_ZONES_COUNT 5

class MultizoneRegulator
{

  class ZoneBoilerInterface : public AbstractBoiler
  {
    public:    
      ZoneBoilerInterface() {};
      ZoneBoilerInterface(MultizoneRegulator *parent, byte num) : _parent(parent), _zoneNum(num) {};
      void setParentAndNum(MultizoneRegulator *parent, byte num);

      void enableHeating();
      void disableHeating();
      void setBoilerTemperature(float88 newTemperature);
      
    private:
      MultizoneRegulator *_parent;
      byte _zoneNum;
  };

  public:
    MultizoneRegulator();
    void attachBoiler(AbstractBoiler &boilerToAttach);
    ZoneBoilerInterface& getZoneBoilerInterface(int i);

  private:
    void evaluateChanges();
  
    AbstractBoiler *_boiler;
    ZoneBoilerInterface _zoneBoilerInterfaces[MULTI_ZONES_COUNT];
    bool zoneEnabled[MULTI_ZONES_COUNT];
    float88 zoneTemperature[MULTI_ZONES_COUNT];
    bool boilerEnabled = false;
    float88 boilerTemperature = 0.0;
   
};
#endif
