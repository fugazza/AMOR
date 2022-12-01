#ifndef AbstractCommunicator_h
#define AbstractCommunicator_h

#include <opentherm.h>

class AbstractCommunicator;
#include "MqttDataProvider.h"
#include "Float88DataType.h"

class AbstractCommunicator
{
  public:
    virtual void loop() = 0;
    virtual void communicateMessage(const OpenthermData &requestMessage, const OpenthermData &answerMessage) = 0;
    virtual void publishTopic(const char *mqttTopic, const char *value) = 0;
    virtual void subscribeTopic(MqttDataProvider &provider, const char *mqttTopic) = 0;
   
};
#endif
