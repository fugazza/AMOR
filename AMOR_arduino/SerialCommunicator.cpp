#include <Arduino.h>
#include <opentherm.h>
#include <TimeLib.h>
#include "Config.h"
#include "AbstractCommunicator.h"
#include "SerialCommunicator.h"
#include "MqttDataProvider.h"

void SerialCommunicator::communicateMessage(const OpenthermData &requestMessage, const OpenthermData &answerMessage){
  char message[64];
  char value[16];
  bool customSend = false;
  bool printMessage = false;

  printMessageCode(requestMessage);
  Serial.print(F(" -> "));
  printMessageCode(answerMessage);
  Serial.print(F(" : "));

  switch (requestMessage.id) {
    case OT_MSGID_STATUS: // ID = 0, R - special
      customSend = true;

      const static char descHB_0[] PROGMEM  = S_MSG_0_CH_ENABLED;
      const static char descHB_1[] PROGMEM  = S_MSG_0_DHW_ENABLED; 
      const static char descHB_2[] PROGMEM  = S_MSG_0_COOLING_ENABLED; 
      const static char descHB_3[] PROGMEM  = S_MSG_0_OTC_ENABLED; 
      const static char descHB_4[] PROGMEM  = S_MSG_0_CH2_ENABLED;  
      const static char *const descriptions_HB[] PROGMEM = {descHB_0, descHB_1, descHB_2, descHB_3, descHB_4};
      Serial.print(F(S_MSG_0_THERMOSTAT_STATUS));
      printBitMaskSerial(descriptions_HB, requestMessage.valueHB, 5);
      Serial.println(); 

      const static char descLB_0[] PROGMEM  = S_MSG_0_FAULTS;
      const static char descLB_1[] PROGMEM  = S_MSG_0_CH_ON; 
      const static char descLB_2[] PROGMEM  = S_MSG_0_DHW_ON; 
      const static char descLB_3[] PROGMEM  = S_MSG_0_FLAME_ON; 
      const static char descLB_4[] PROGMEM  = S_MSG_0_COOLING_ON;              
      const static char descLB_5[] PROGMEM  = S_MSG_0_CH2_ON;
      const static char descLB_6[] PROGMEM  = S_MSG_0_DIAG_EVENT;

      const static char *const descriptions_LB[] PROGMEM = {descLB_0, descLB_1, descLB_2, descLB_3, descLB_4, descLB_5, descLB_6};

      Serial.print(F(S_MSG_0_BOILER_STATUS));
      printBitMaskSerial(descriptions_LB, answerMessage.valueLB, 7);          
      Serial.println();                                      
      break;

    case OT_MSGID_CH_SETPOINT: // ID = 1, W
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_1_CH_SETPOINT));
      dtostrf(requestMessage.f88(), 3, 1, value);
      break;

    case OT_MSGID_MASTER_CONFIG: // ID = 2, W
      customSend = true;
      byte masterConfig;
      masterConfig = requestMessage.valueHB;
      byte masterMemberID;
      masterMemberID = requestMessage.valueLB;       
       
      Serial.print(F(S_MSG_2_MASTER_CONFIG));
      Serial.print(masterConfig);
      Serial.print(F(S_MSG_2_MASTER_MEMBER_ID));
      Serial.println(masterMemberID);
      break;    

    case OT_MSGID_SLAVE_CONFIG: // ID = 3, R
      customSend = true;

      const static char desc3HB_0[] PROGMEM  = S_MSG_3_HAS_DHW;
      const static char desc3HB_1[] PROGMEM  = S_MSG_3_REGULATION_TYPE; 
      const static char desc3HB_2[] PROGMEM  = S_MSG_3_HAS_COOLING; 
      const static char desc3HB_3[] PROGMEM  = S_MSG_3_DHW_TYPE; 
      const static char desc3HB_4[] PROGMEM  = S_MSG_3_PUMP_CONTROL;         
      const static char desc3HB_5[] PROGMEM  = S_MSG_3_HAS_CH2;
      
      const static char *const descriptions_3HB[] PROGMEM = {desc3HB_0, desc3HB_1, desc3HB_2, desc3HB_3, desc3HB_4, desc3HB_5};

      byte slaveMemberID;
      slaveMemberID = answerMessage.valueLB;       
       
      Serial.print(F(S_MSG_3_SLAVE_CONFIG));
      printBitMaskSerial(descriptions_3HB, answerMessage.valueHB, 6);
      Serial.print(F(S_MSG_3_SLAVE_MEMBER_ID));
      Serial.println(slaveMemberID);
      break;     

    case OT_MSGID_FAULT_FLAGS: // ID = 5, R
      customSend = true;

      const static char desc5HB_0[] PROGMEM  = S_MSG_5_SERVICE_REQUEST;
      const static char desc5HB_1[] PROGMEM  = S_MSG_5_LOCKOUT_RESET; 
      const static char desc5HB_2[] PROGMEM  = S_MSG_5_WATER_LOW_PRESSURE; 
      const static char desc5HB_3[] PROGMEM  = S_MSG_5_FLAME_ERROR; 
      const static char desc5HB_4[] PROGMEM  = S_MSG_5_AIR_PRESSSURE_ERROR;         
      const static char desc5HB_5[] PROGMEM  = S_MSG_5_WATER_TEMPERATURE_TOO_HIGH;

      const static char *const descriptions_5HB[] PROGMEM = {desc5HB_0, desc5HB_1, desc5HB_2, desc5HB_3, desc5HB_4, desc5HB_5};

      byte OEMerrorCode;
      OEMerrorCode = answerMessage.valueLB;       
       
      Serial.print(F(S_MSG_5_FAULT_FLAGS));
      printBitMaskSerial(descriptions_5HB, answerMessage.valueHB, 6);
      Serial.print(F(S_MSG_5_ERROR_CODE));
      Serial.println(OEMerrorCode);
      break;    
      
    case OT_MSGID_CH_SETPOINT_OVERRIDE: // ID = 9, W
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_9_REMOTE_OVERRIDE_ROOM_SETPOINT));
      dtostrf(requestMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_TSP_COUNT: // ID = 10, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_10_TSP_COUNT));
      sprintf(value, "%d", answerMessage.valueHB);
      break;      

    case OT_MSGID_MAX_MODULATION_LEVEL: // ID = 14, W 
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_14_MAX_MODULATION));
      dtostrf(requestMessage.f88(), 3, 1, value);
      break; 
        
    case OT_MSGID_ROOM_SETPOINT: // ID = 16, W
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_16_ROOM_SETPOINT));
      dtostrf(requestMessage.f88(), 3, 1, value);
      break; 
      
    case OT_MSGID_MODULATION_LEVEL: // ID = 17, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_17_MODULATION_LEVEL));
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
    
    case OT_MSGID_CH_WATER_PRESSURE: // ID = 18, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_18_WATER_PRESSURE));
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_DHW_FLOW_RATE: // ID = 19, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_19_DHW_FLOW_RATE));
      dtostrf(answerMessage.f88(), 3, 1, value);      
      break;
              
    case OT_MSGID_ROOM_TEMP: // ID = 24, W
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_24_ROOM_TEMPERATURE));
      dtostrf(requestMessage.f88(), 3, 1, value);
      break;
                      
    case OT_MSGID_FEED_TEMP: // ID = 25, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_25_FEED_TEMP));
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_DHW_TEMP: // ID = 26, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_26_DHW_TEMP));      
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_OUTSIDE_TEMP: // ID = 27, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_27_OUTSIDE_TEMPERATURE));
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_RETURN_WATER_TEMP: // ID = 28, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_28_RETURN_WATER_TEMPERATURE));
      dtostrf(answerMessage.f88(), 3, 1, value);
      break;
      
    case OT_MSGID_EXHAUST_TEMP: // ID = 33, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_33_EXHAUST_TEMPERATURE));
      sprintf(value, "%d", answerMessage.s16());
      break;

    case OT_MSGID_DHW_BOUNDS: // ID = 48, R
      customSend = true;
      int8_t upperBound;
      upperBound = answerMessage.valueHB;
      int8_t lowerBound;
      lowerBound = answerMessage.valueLB;       
       
      Serial.print(F(S_MSG_48_DHW_BOUNDS));
      Serial.print(lowerBound);
      Serial.print(F(" ... "));
      Serial.println(upperBound);
    break;       
      
    case OT_MSGID_CH_BOUNDS: // ID = 49, R
      customSend = true;
      int8_t CHupperBound;
      CHupperBound = answerMessage.valueHB;
      int8_t CHlowerBound;
      CHlowerBound = answerMessage.valueLB;       
       
      Serial.print(F(S_MSG_49_CH_BOUNDS"Povoleny rozsah nastaveni teploty topeni: "));
      Serial.print(CHlowerBound);
      Serial.print(F(" ... "));
      Serial.println(CHupperBound);
      break;   
    
    case OT_MSGID_DHW_SETPOINT: // ID = 56, R/W
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_56_DHW_SETPOINT));
      if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
        dtostrf(answerMessage.f88(), 3, 1, value);
      } else {
        dtostrf(requestMessage.f88(), 3, 1, value);
      }
      break;

    case OT_MSGID_MAX_CH_SETPOINT: // ID = 57, R/W
      printMessage = true;      
      strcpy_P(message,PSTR(S_MSG_57_MAX_CH_SETPOINT));
      if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
        dtostrf(answerMessage.f88(), 3, 1, value);
      } else {
        dtostrf(requestMessage.f88(), 3, 1, value);
      }
      break;
      
    case 99: // ID = 99, R
      printMessage = true;     
      strcpy_P(message,PSTR(S_MSG_99));
      if (requestMessage.type == OT_MSGTYPE_READ_DATA) {
        sprintf(value, "%d", answerMessage.u16());
      } else {
        sprintf(value, "%d", requestMessage.u16());
      }      
      
      break;   

    case OT_MSGID_OVERRIDE_FUNC: // ID = 100, R
      printMessage = true;
      customSend = true;
      byte manualPrio;
      manualPrio   = answerMessage.valueLB & 1;
      byte programPrio;
      programPrio   = (answerMessage.valueLB >> 1) & 1;
      Serial.print(F(S_MSG_100_MANUAL_CHANGE_PRIORITY));
      Serial.print(manualPrio);
      Serial.print(F(S_MSG_100_PROGRAM_CHANGE_PRIORITY));
      Serial.println(programPrio);
      break;

    case OT_MSGID_OEM_DIAGNOSTIC: // ID = 115, R
      printMessage = true;
      strcpy_P(message,PSTR(S_MSG_115_OEM_DIAGNOSTIC_CODE));
      sprintf(value, "%d", answerMessage.u16());
      break;       
  } // end switch                  


  if (!customSend && printMessage) {
    Serial.print(message);
    Serial.print(F(": "));
    Serial.println(value);
  }  
}


void SerialCommunicator::printBitMaskSerial(const char *const *descriptionsArray, byte value, byte numberOfBits) {

  char buffer[50];
  byte bitValue;
  
  for (int i = 0; i < numberOfBits; i++) {
    strcpy_P(buffer, (char *)pgm_read_word(&(descriptionsArray[i])));   
    bitValue = (value >> i) & 1;

    Serial.print(buffer);
    Serial.print(F(" = "));
    Serial.print(bitValue);
    Serial.print(F("; "));    
  } 
}

void SerialCommunicator::loop() {
  if (Serial.available()) {
    char c;
    while(Serial.available()) {
      c = Serial.read();

      if (c == '=') {
        haveType = true;
      } else if (c == '\r') {
        haveCR = true;
      } else if (c == '\n') {
        if (haveCR) {
          messageReceived = true;
          haveType = false;
          haveCR = false;

          break;
        } else {
          DPRINTLN(F(D_COMMUNICATOR_NL_WITHOUT_CR));
        }
        
      } else if (haveType) {
        strncat(commandValue,&c, 1);
      } else {
        strncat(commandType,&c, 1);
      }
    }
  }

  if (messageReceived) {
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
      DPRINT(F(D_SERCOM_NEW_DATETIME));
      DPRINT(day()); DPRINT(".");
      DPRINT(month()); DPRINT(".");
      DPRINT(year()); DPRINT(" ");
      DPRINT(hour()); DPRINT(":");
      DPRINT(minute()); DPRINT(":");
      DPRINTLN(second());       
      
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
    
    messageReceived = false;
    commandType[0] = '\0';    
    commandValue[0] = '\0';    
  }
}

void SerialCommunicator::printMessageCode(const OpenthermData &message) {
  if (message.type == OT_MSGTYPE_READ_DATA) {
    Serial.print(F("RD"));
  } else if (message.type == OT_MSGTYPE_READ_ACK) {
    Serial.print(F("RA"));
  } else if (message.type == OT_MSGTYPE_WRITE_DATA) {
    Serial.print(F("WD"));
  } else if (message.type == OT_MSGTYPE_WRITE_ACK) {
    Serial.print(F("WA"));
  } else if (message.type == OT_MSGTYPE_INVALID_DATA) {
    Serial.print(F("ID"));
  } else if (message.type == OT_MSGTYPE_DATA_INVALID) {
    Serial.print(F("DI"));
  } else if (message.type == OT_MSGTYPE_UNKNOWN_DATAID) {
    Serial.print(F("UI"));
  } else {
    Serial.print(F("ET"));
  }
  Serial.print(F(" "));
  Serial.print(message.id);
  Serial.print(F(" "));
  Serial.print(message.valueHB, HEX);
  Serial.print(F(" "));
  Serial.print(message.valueLB, HEX);
}

void SerialCommunicator::subscribeTopic(MqttDataProvider &_provider, const char *mqttTopic) {
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

void SerialCommunicator::publishTopic(const char *mqttTopic, const char *value) {
  Serial.print(F("#P:"));
  Serial.print(mqttTopic);
  Serial.print(F("="));
  Serial.println(value);
}
