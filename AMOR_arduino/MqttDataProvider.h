#ifndef MqttDataProvider_h
#define MqttDataProvider_h

#include "Float88DataType.h"
#include "DataListener.h"
#include "DataPublisher.h"
class MqttDataProvider;
#include "AbstractCommunicator.h"

class MqttDataProvider : public DataPublisher
{
  public:
    //MqttDataProvider();
    MqttDataProvider(AbstractCommunicator &communicator);
    void setTopic(byte dataID, const char *mqttTopic);
    //MqttDataProvider(AbstractCommunicator &communicator, byte dataID);
    void newValue(const char *value);

  private:
    byte _dataID;
    const char *_mqttTopic;
    AbstractCommunicator *_communicator;
};
#endif
