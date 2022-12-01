#ifndef EspMqttCommunicator_h
#define EspMqttCommunicator_h

#include <opentherm.h>

#include "AbstractCommunicator.h"

#ifndef OLD_VAL_SIZE
  #define OLD_VAL_SIZE 20
#endif
#ifndef MIN_UPDATE_TIME
  #define MIN_UPDATE_TIME 300000  //300.000 = 300 s = 5 min
#endif
#define EMC_FLAG_HAVE_TYPE 0
#define EMC_FLAG_HAVE_CR   1
#define EMC_FLAG_MESSAGE_RECEIVED 2

#ifndef PROVIDERS_LIST_SIZE
  #define PROVIDERS_LIST_SIZE 10
#endif

#ifndef MQTT_BASE_TOPIC
  #define MQTT_BASE_TOPIC "boiler/"
#endif

struct ProviderInfo {
  bool used = false;
  MqttDataProvider *provider;
  const char *topic;
};

struct OldVal {
  byte id;
  uint16_t value;
  unsigned long time;
};

class EspMqttCommunicator : public AbstractCommunicator
{
  public:
    EspMqttCommunicator();
    void loop();
    void communicateMessage(const OpenthermData &requestMessage, const OpenthermData &answerMessage);
    void subscribeTopic(MqttDataProvider &provider, const char *mqttTopic);
    void publishTopic(const char *mqttTopic, const char *value);    

  private:
    const char *baseTopic = MQTT_BASE_TOPIC;   
    OldVal oldValues[OLD_VAL_SIZE];
    byte flags = 0;
    char commandType[4];    
    char commandValue[64]; 
    ProviderInfo providers[PROVIDERS_LIST_SIZE]; 
    unsigned long providersLastSubscribeTime = 0l;     
  
    void printBitMaskMQTT(const char *const *topicsArray, byte value, byte numberOfBits);
    void setOldValue(byte _id, uint16_t _value, unsigned long _time);
    OldVal getOldValue(byte _id);
    void setFlag(byte flag, bool value);
    bool isFlag(byte flag);
    
};
#endif
