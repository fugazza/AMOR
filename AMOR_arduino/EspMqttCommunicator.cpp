#include <Arduino.h>
#include <opentherm.h>
#include <TimeLib.h>
#include "Config.h"
#include "AbstractCommunicator.h"
#include "EspMqttCommunicator.h"

EspMqttCommunicator::EspMqttCommunicator() {
  for (int i=0; i<OLD_VAL_SIZE; i++) {
    oldValues[i].id = 255;
  }
}

void EspMqttCommunicator::communicateMessage(const OpenthermData &requestMessage, const OpenthermData &answerMessage){
  unsigned long const timeNow = millis();
  uint16_t theValue;
  OldVal oldValue;

  char topic[64];
  char value[16];
  boolean customSend = false;
  boolean printMessage = false;
  
  if (requestMessage.id == OT_MSGID_STATUS) {
    theValue = (requestMessage.valueHB << 8) | answerMessage.valueLB;
  } else if (requestMessage.type == OT_MSGTYPE_WRITE_DATA) {
    theValue = requestMessage.u16();
  } else {
    theValue = answerMessage.u16();
  }
  oldValue = getOldValue(requestMessage.id);
  
  if ((oldValue.id == requestMessage.id) && (oldValue.value == theValue) && ((timeNow - oldValue.time) < MIN_UPDATE_TIME) ) {
    // do not flush the MQTT server with values that are the same as last value,
    // if there is not too long time since last update
    printMessage = false; 
  } else {
    printMessage = true;
    strcpy(topic, baseTopic);
    //Serial.print(OTmessage.id);
    //Serial.print(": ");
    setOldValue(requestMessage.id, theValue, timeNow);
  }  

  if (printMessage) {    
    switch (requestMessage.id) {
      case OT_MSGID_STATUS: // ID = 0, R - special
        customSend = true;

        const static char topicHB_0[] PROGMEM = "master_status/CH_enable";
        const static char topicHB_1[] PROGMEM = "master_status/DHW_enable";
        const static char topicHB_2[] PROGMEM = "master_status/cooling_enable";
        const static char topicHB_3[] PROGMEM = "master_status/OTC_active";
        const static char topicHB_4[] PROGMEM = "master_status/CH2_enable";
        const static char *const topics_HB[] PROGMEM = {topicHB_0, topicHB_1, topicHB_2, topicHB_3, topicHB_4};
        printBitMaskMQTT(topics_HB, requestMessage.valueHB, 5);
        
        const static char topicLB_0[] PROGMEM = "slave_status/fault_indication";
        const static char topicLB_1[] PROGMEM = "slave_status/CH_mode";
        const static char topicLB_2[] PROGMEM = "slave_status/DHW_mode";
        const static char topicLB_3[] PROGMEM = "slave_status/flame";
        const static char topicLB_4[] PROGMEM = "slave_status/cooling_status";
        const static char topicLB_5[] PROGMEM = "slave_status/CH2_mode";
        const static char topicLB_6[] PROGMEM = "slave_status/diagnostics_status";
        const static char *const topics_LB[] PROGMEM = {topicLB_0, topicLB_1, topicLB_2, topicLB_3, topicLB_4, topicLB_5, topicLB_6};
        printBitMaskMQTT(topics_LB, answerMessage.valueLB, 7);
        break;

      case OT_MSGID_CH_SETPOINT: // ID = 1, W
        printMessage = true;
        strcat_P(topic,PSTR("CH_setpoint"));
        dtostrf(requestMessage.f88(), 3, 1, value);
        break;

      case OT_MSGID_MASTER_CONFIG: // ID = 2, W
        customSend = true;
        byte masterConfig;
        masterConfig = requestMessage.valueHB;
        byte masterMemberID;
        masterMemberID = requestMessage.valueLB;       
         
        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("master_config/config"));
        Serial.print("=");
        Serial.println(masterConfig);

        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("master_config/memberID"));
        Serial.print("=");
        Serial.println(masterMemberID); 
        break; 
                
      case OT_MSGID_SLAVE_CONFIG: // ID = 3, R
        customSend = true;

        const static char topic3HB_0[] PROGMEM = "slave_config/DHW_present";
        const static char topic3HB_1[] PROGMEM = "slave_config/control_type";
        const static char topic3HB_2[] PROGMEM = "slave_config/cooling";
        const static char topic3HB_3[] PROGMEM = "slave_config/DHW_config";
        const static char topic3HB_4[] PROGMEM = "slave_config/pump_control";
        const static char topic3HB_5[] PROGMEM = "slave_config/CH2present";
        const static char *const topics_3HB[] PROGMEM = {topic3HB_0, topic3HB_1, topic3HB_2, topic3HB_3, topic3HB_4, topic3HB_5};

        byte slaveMemberID;
        slaveMemberID = answerMessage.valueLB;       
       
        printBitMaskMQTT(topics_3HB, answerMessage.valueHB, 6);

        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("slave_config/memberID"));
        Serial.print("=");
        Serial.println(slaveMemberID); 
        break;     

      case OT_MSGID_FAULT_FLAGS: // ID = 5, R
        customSend = true;

        const static char topic5HB_0[] PROGMEM = "fault/service_request";
        const static char topic5HB_1[] PROGMEM = "fault/lockout_reset";
        const static char topic5HB_2[] PROGMEM = "fault/low_water_pressure";
        const static char topic5HB_3[] PROGMEM = "fault/flame";
        const static char topic5HB_4[] PROGMEM = "fault/air_pressure";
        const static char topic5HB_5[] PROGMEM = "fault/water_over_temp";
        const static char *const topics_5HB[] PROGMEM = {topic5HB_0, topic5HB_1, topic5HB_2, topic5HB_3, topic5HB_4, topic5HB_5};

        byte OEMerrorCode;
        OEMerrorCode = answerMessage.valueLB;       
         
        printBitMaskMQTT(topics_5HB, answerMessage.valueHB, 6);

        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("fault/OEM_error_code"));
        Serial.print("=");
        Serial.println(OEMerrorCode); 
        break;    
        
      case OT_MSGID_CH_SETPOINT_OVERRIDE: // ID = 9, W
        printMessage = true;
        strcat_P(topic,PSTR("CH_setpoint_override"));
        dtostrf(requestMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_TSP_COUNT: // ID = 10, R
        printMessage = true;
        strcat_P(topic,PSTR("TSP_count"));
        sprintf(value, "%d", answerMessage.valueHB);
        break;      

      case OT_MSGID_MAX_MODULATION_LEVEL: // ID = 14, W 
        printMessage = true;
        strcat_P(topic,PSTR("max_modulation_level"));
        dtostrf(requestMessage.f88(), 3, 1, value);
        break; 
          
      case OT_MSGID_ROOM_SETPOINT: // ID = 16, W
        printMessage = true;
        strcat_P(topic,PSTR("room_setpoint"));
        dtostrf(requestMessage.f88(), 3, 1, value);
        break; 
        
      case OT_MSGID_MODULATION_LEVEL: // ID = 17, R
        printMessage = true;
        strcat_P(topic,PSTR("modulation"));
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
      
      case OT_MSGID_CH_WATER_PRESSURE: // ID = 18, R
        printMessage = true;
        strcat_P(topic,PSTR("CH_water_pressure"));
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_DHW_FLOW_RATE:  // ID = 19, R
        printMessage = true;
        strcat_P(topic,PSTR("DHW_flow"));
        dtostrf(answerMessage.f88(), 3, 1, value);      
        break;
                
      case OT_MSGID_ROOM_TEMP: // ID = 24, W
        printMessage = true;
        strcat_P(topic,PSTR("room_temperature"));
        dtostrf(requestMessage.f88(), 3, 1, value);
        break;
                        
      case OT_MSGID_FEED_TEMP: // ID = 25, R
        printMessage = true;
        strcat_P(topic,PSTR("boiler_water_temperature"));
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_DHW_TEMP: // ID = 26, R
        printMessage = true;
        strcat_P(topic,PSTR("DHW_temperature"));     
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_OUTSIDE_TEMP: // ID = 27, R
        printMessage = true;
        strcat_P(topic,PSTR("outside_temperature"));
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_RETURN_WATER_TEMP: // ID = 28, R
        printMessage = true;
        strcat_P(topic,PSTR("return_water_temperature"));
        dtostrf(answerMessage.f88(), 3, 1, value);
        break;
        
      case OT_MSGID_EXHAUST_TEMP: // ID = 33, R
        printMessage = true;
        strcat_P(topic,PSTR("exhaust_temperature"));
        sprintf(value, "%d", answerMessage.s16());
        break;

      case OT_MSGID_DHW_BOUNDS: // ID = 48, R
        customSend = true;
        int8_t upperBound;
        upperBound = answerMessage.valueHB;
        int8_t lowerBound;
        lowerBound = answerMessage.valueLB;       
         
        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("DHW_setpoint_upper"));
        Serial.print("=");
        Serial.println(upperBound);

        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("DHW_setpoint_lower"));
        Serial.print("=");
        Serial.println(lowerBound); 
        break;       
        
      case OT_MSGID_CH_BOUNDS: // ID = 49, R
        customSend = true;
        int8_t CHupperBound;
        CHupperBound = answerMessage.valueHB;
        int8_t CHlowerBound;
        CHlowerBound = answerMessage.valueLB;       
         
        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("CH_setpoint_upper"));
        Serial.print("=");
        Serial.println(CHupperBound);

        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("CH_setpoint_lower"));
        Serial.print("=");
        Serial.println(CHlowerBound); 
        break;  
         
      case OT_MSGID_DHW_SETPOINT: // ID = 56, R/W
        printMessage = true;
        strcat_P(topic,PSTR("DHW_setpoint"));
        if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
          dtostrf(answerMessage.f88(), 3, 1, value);
        } else {
          dtostrf(requestMessage.f88(), 3, 1, value);
        }
        break;

      case OT_MSGID_MAX_CH_SETPOINT: // ID = 57, R/W 
        printMessage = true;    
        strcat_P(topic,PSTR("CH_setpoint_max"));
        if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
          dtostrf(answerMessage.f88(), 3, 1, value);
        } else {
          dtostrf(requestMessage.f88(), 3, 1, value);
        }  
        break;
        
      case 99:  // ID = 99, R
        printMessage = true;     
        strcat_P(topic,PSTR("command_99"));
        if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
          sprintf(value, "%d", answerMessage.u16());
        } else {
          sprintf(value, "%d", requestMessage.u16());
        }  
        break;   

      case OT_MSGID_OVERRIDE_FUNC: // ID = 100, R
        printMessage = true;
        customSend = true;
        Serial.print(F("#P:"));
        Serial.print(baseTopic);
        Serial.print(F("remote_override_function"));
        Serial.print("=");
        Serial.println(answerMessage.valueHB);
        break;

      case OT_MSGID_OEM_DIAGNOSTIC: // ID = 115, R
        printMessage = true;
        strcat_P(topic,PSTR("OEM_diagnostic_code"));
        sprintf(value, "%d", answerMessage.u16());
        break;       
    } // end switch                  
  } // end if (printMessage)

  if (!customSend && printMessage) {
    Serial.print(F("#P:"));
    Serial.print(topic);
    Serial.print("=");
    Serial.println(value);
  }  
}

void EspMqttCommunicator::printBitMaskMQTT(const char *const *topicsArray, byte value, byte numberOfBits) {

  char buffer[50];
  byte bitValue;
  
  for (int i = 0; i < numberOfBits; i++) {
    strcpy_P(buffer, (char *)pgm_read_word(&(topicsArray[i])));   
    bitValue = (value >> i) & 1;

    Serial.print(F("#P:"));
    Serial.print(baseTopic);
    Serial.print(buffer);
    Serial.print(F("="));
    Serial.println(bitValue);
  } 
}

void EspMqttCommunicator::loop() {
  if (Serial.available()) {
    char c;
    while(Serial.available()) {
      c = Serial.read();

      if (c == '=') {
        setFlag(EMC_FLAG_HAVE_TYPE, true);
      } else if (c == '\r') {
        setFlag(EMC_FLAG_HAVE_CR, true);
      } else if (c == '\n') {
        if (isFlag(EMC_FLAG_HAVE_CR)) {
          setFlag(EMC_FLAG_MESSAGE_RECEIVED, true);
          setFlag(EMC_FLAG_HAVE_TYPE, false);
          setFlag(EMC_FLAG_HAVE_CR, false);

          break;
        } else {
          DPRINTLN(F(D_COMMUNICATOR_NL_WITHOUT_CR));
        }
        
      } else if (isFlag(EMC_FLAG_HAVE_TYPE)) {
        strncat(commandValue,&c, 1);
      } else {
        strncat(commandType,&c, 1);
      }
    }
  }

  if (isFlag(EMC_FLAG_MESSAGE_RECEIVED)) {
    if (strcmp(commandType,"T") == 0) {
      // synchronize time
      int timeField[6];
      int i=0;
      char * pch;
      pch = strtok (commandValue," .:/");
      while (pch != NULL && i<6)
      {
        timeField[i] = atoi(pch);
        pch = strtok (NULL, " .:/");
        i++;
      }

      // format = "day.month.year hour:minute:second"
      setTime(timeField[3],timeField[4],timeField[5],timeField[0],timeField[1],timeField[2]);

      Serial.print(F("#P:"));
      Serial.print(baseTopic);
      Serial.print(F("time="));
      Serial.print(day()); Serial.print(F("."));
      Serial.print(month()); Serial.print(F("."));
      Serial.print(year()); Serial.print(F(" "));
      Serial.print(hour()); Serial.print(F(":"));
      Serial.print(minute()); Serial.print(F(":"));
      Serial.println(second());  
      
    } else if (strcmp(commandType,"V") == 0) {
      // some value received -> dispatch it
      char *_topic;
      char *_value;
      _topic = strtok (commandValue,":");
      _value = strtok (NULL,":");

      DPRINT(F(D_COMMUNICATOR_VALUE_RECEIVED));
      DPRINT(commandValue);
      DPRINT(F(D_COMMUNICATOR_TOPIC));
      DPRINT(_topic);
      DPRINT(F(D_COMMUNICATOR_VALUE));
      DPRINT(_value);  
      DPRINTLN(F("'"));    
      
      for (int i=0; i<PROVIDERS_LIST_SIZE; i++) {
        if (providers[i].used && strcmp_P(_topic, providers[i].topic) == 0) {
          DPRINT(F(D_COMMUNICATOR_PROVIDER_FOUND));
          DPRINT(_topic);
          DPRINT(F(D_COMMUNICATOR_SENDING_VALUE));
          DPRINTLN(_value);
          providers[i].provider->newValue(_value);
        }
      } 
    }
    
    setFlag(EMC_FLAG_MESSAGE_RECEIVED, false);
    commandType[0] = '\0';    
    commandValue[0] = '\0';    
  }

  // pravidelně obnovit subscribe u providerů
  if ((millis() - providersLastSubscribeTime) > 300000) {
    char topicString[64];
    providersLastSubscribeTime = millis();
    for (int i=0; i<PROVIDERS_LIST_SIZE; i++) {
        if (providers[i].used) {    
          strcpy_P(topicString, providers[i].topic);
          Serial.print(F("#S:"));
          Serial.println(topicString);
        }
      }     
  }
}

void EspMqttCommunicator::setOldValue(byte _id, uint16_t _value, unsigned long _time) {
  unsigned long oldestTime = millis();
  byte index = 0;
  
  for (int i=0; i<OLD_VAL_SIZE; i++) {
    if (   (oldValues[i].id == _id)
        || (oldValues[i].id == 255)
        || ((millis() - oldValues[i].time) > MIN_UPDATE_TIME) ) {
      index = i;
      break;
    } else if (oldValues[i].time < oldestTime) {
      oldestTime = oldValues[i].time;
      index = i;
    }
  }  

  DPRINT(F("Writing old value for id "));
  DPRINT(_id);
  DPRINT(F(" to index "));
  DPRINTLN(index);
  
  oldValues[index].id = _id;
  oldValues[index].value = _value;
  oldValues[index].time = _time;
}

OldVal EspMqttCommunicator::getOldValue(byte _id) {
  for (int i=0; i<OLD_VAL_SIZE; i++) {
    if (oldValues[i].id == _id) {
      DPRINT(F(D_ESPMQTT_OLD_VALUE_FOUND));
      DPRINT(_id);
      DPRINT(F(D_ESPMQTT_ON_INDEX));
      DPRINTLN(i);      
      return oldValues[i];
    }
  }

  DPRINT(F(D_ESPMQTT_OLD_VALUE_NOT_FOUND));
  DPRINTLN(_id); 
  return {.id = 255, .value = 0, .time = 0};
}

void EspMqttCommunicator::setFlag(byte flag, bool value) {
  flags = (flags & (~(1 << flag))) | (value << flag);
}

bool EspMqttCommunicator::isFlag(byte flag) {
  return (flags >> flag) & 1;
}

void EspMqttCommunicator::subscribeTopic(MqttDataProvider &_provider, const char *mqttTopic) {
  char topicString[64];
  
  for (int i=0; i<PROVIDERS_LIST_SIZE; i++) {
    if (!providers[i].used) {
      providers[i].used = true;
      providers[i].provider = &_provider;
      providers[i].topic = mqttTopic;

      strcpy_P(topicString, providers[i].topic);
      Serial.print(F("#S:"));
      Serial.println(topicString);
      break;
    }
  } 
}

void EspMqttCommunicator::publishTopic(const char *mqttTopic, const char *value) {
  Serial.print(F("#P:"));
  Serial.print(mqttTopic);
  Serial.print(F("="));
  Serial.println(value);
}
