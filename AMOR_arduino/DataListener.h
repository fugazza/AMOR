#ifndef DataListener_h
#define DataListener_h

#include "Float88DataType.h"

#define DATA_ID_ROOM_TEMPERATURE            1
#define DATA_ID_ROOM_TEMPERATURE_SETPOINT   2
#define DATA_ID_OUTSIDE_TEMPERATURE         4
#define DATA_ID_PROGRAM                   128
#define DATA_ID_ALL                       255

class DataListener
{
  public:
    virtual void dataUpdated(byte dataID, const char *value) = 0;
    virtual void dataUpdated(byte dataID, float88 value) = 0;
};
#endif
