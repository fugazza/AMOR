#ifndef SerialCommunicator_h
#define SerialCommunicator_h

#include <opentherm.h>

class SerialCommunicator;
#include "AbstractCommunicator.h"
#include "MqttDataProvider.h"

//#define SC_FLAG_HAVE_TYPE 0

#ifndef PROVIDERS_LIST_SIZE
  #define PROVIDERS_LIST_SIZE 10
#endif

struct ProviderInfo {
  bool used = false;
  MqttDataProvider *provider;
  const char *topic;
};

class SerialCommunicator : public AbstractCommunicator
{
  public:

    void loop();
    void communicateMessage(const OpenthermData &requestMessage, const OpenthermData &answerMessage);
    void subscribeTopic(MqttDataProvider &provider, const char *mqttTopic);
    void publishTopic(const char *mqttTopic, const char *value);

  private:
    boolean haveType = false;
    boolean haveCR = false;
    boolean messageReceived = false;
    char commandType[4];    
    char commandValue[64];  
    ProviderInfo providers[PROVIDERS_LIST_SIZE]; 
  
    void printBitMaskSerial(const char *const *descriptionsArray, byte value, byte numberOfBits);
    void printMessageCode(const OpenthermData &message);
    
};
#endif
