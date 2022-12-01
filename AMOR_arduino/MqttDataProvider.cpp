#include "MqttDataProvider.h"
#include "Float88DataType.h"
#include "DataListener.h"
#include "AbstractCommunicator.h"



MqttDataProvider::MqttDataProvider(AbstractCommunicator &communicator) : _communicator(&communicator) {
}

void MqttDataProvider::setTopic(byte dataID, const char *mqttTopic) {
  _dataID = dataID;
  _mqttTopic = mqttTopic;

  _communicator->subscribeTopic(*this,_mqttTopic);  
}

void MqttDataProvider::newValue(const char *value) {
  fireDataUpdate(_dataID, value);
}
