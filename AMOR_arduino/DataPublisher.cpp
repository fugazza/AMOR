#include "DataPublisher.h"

void DataPublisher::registerDataListener(DataListener &newListener) {
  registerDataListener(newListener, DATA_ID_ALL);
}

void DataPublisher::registerDataListener(DataListener &newListener, byte _dataIdRequired) {
  for (int i=0; i<DATA_LISTENERS_SIZE; i++) {
    if (dataListeners[i].assigned && dataListeners[i].listener == &newListener) {
      return;
    }
  }

  for (int i=0; i<DATA_LISTENERS_SIZE; i++) {
    if (!dataListeners[i].assigned) {
      dataListeners[i].assigned = true;
      dataListeners[i].dataIdRequired = _dataIdRequired;
      dataListeners[i].listener = &newListener;
      return;
    }
  } 
}

void DataPublisher::fireDataUpdate(byte dataID, const char *value) {
  for (int i=0; i<DATA_LISTENERS_SIZE; i++) {
    if (dataListeners[i].assigned
        && (dataListeners[i].dataIdRequired == dataID || dataListeners[i].dataIdRequired == DATA_ID_ALL)) {
          
      dataListeners[i].listener->dataUpdated(dataID, value);
      
    }
  }  
}

void DataPublisher::fireDataUpdate(byte dataID, float88 value) {
  for (int i=0; i<DATA_LISTENERS_SIZE; i++) {
    if (dataListeners[i].assigned
        && (dataListeners[i].dataIdRequired == dataID || dataListeners[i].dataIdRequired == DATA_ID_ALL)) {
          
      dataListeners[i].listener->dataUpdated(dataID, value);
      
    }
  }  
}
