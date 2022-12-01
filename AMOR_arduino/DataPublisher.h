#ifndef DataPublisher_h
#define DataPublisher_h

#include "Float88DataType.h"

#include "DataListener.h"

#ifndef DATA_LISTENERS_SIZE
  #define DATA_LISTENERS_SIZE 3
#endif

struct DataListenerInfo {
  DataListener *listener;
  bool assigned = false;
  byte dataIdRequired = DATA_ID_ALL;
};

class DataPublisher
{
  public:
    void registerDataListener(DataListener &newListener);
    void registerDataListener(DataListener &newListener, byte _dataIdRequired);
    void fireDataUpdate(byte dataID, const char *value);
    void fireDataUpdate(byte dataID, float88 value);

  private:
    DataListenerInfo dataListeners[DATA_LISTENERS_SIZE];
};
#endif
