#include <Arduino.h>
#include <opentherm.h>
#include <CircularBuffer.h>

#include "Config.h"
#include "OTlooper.h"
#include "SingleRegulator.h"

/*
 * constructor
 */
OTlooper::OTlooper() : 
_CHBoilerInterface(this), 
_DHWBoilerInterface(this)
{
  repeatedCommands.push(OT_MSGID_MASTER_CONFIG);      // master config - original thermostat send this message permanently - it seem that if this message is not sent, DHW does not work
  repeatedCommands.push(OT_MSGID_STATUS);             // status - key message - setup if CH is enabled, if DHW enabled; at the same time this is the request on boiler to report its status back
  repeatedCommands.push(OT_MSGID_CH_SETPOINT);        // target temperature of heating water (CH)
  repeatedCommands.push(OT_MSGID_MODULATION_LEVEL);   // modulation = how much the boiler is heating
  repeatedCommands.push(OT_MSGID_FEED_TEMP);          // actual temperature of boiler water on output from boiler
  repeatedCommands.push(OT_MSGID_DHW_SETPOINT);       // target temperature for DHW
  repeatedCommands.push(OT_MSGID_DHW_TEMP);           // actual temperature of DHW
  repeatedCommands.push(OT_MSGID_FAULT_FLAGS);        // read the fault flags
  nextMessageTime = millis() + PAUSE_BETWEEN_MESSAGES;

  oneMinuteTickerTime = millis();
  fiveMinuteTickerTime = millis();
}

/*
 * main loop, where all communication with OpenthermShield happens
 */

void OTlooper::loop()
{
  unsigned long t = millis();
  int commandID;
  // loop generating and sending the messages to the boiler
  // when it's time to send the message, then
  if (t > nextMessageTime) {
    if (!singleUseCommands.isEmpty() && lastCommandWasFromRepeatedQueue) {
      // send command from single-use commands queue, if not empty
      commandID = singleUseCommands.shift();
      lastCommandWasFromRepeatedQueue = false;
    } else {
      // else send the next message from circular buffer - the messages that repeat all the time in endless loop
      commandID = repeatedCommands.shift();
      // push the message back in the buffer, so that it does not empty and rotates forever
      repeatedCommands.push(commandID);
      lastCommandWasFromRepeatedQueue = true;
    }

    DPRINT(F(D_OT_LOOPER_SENDING_MESSAGE));
    DPRINTLN(commandID);
    OpenthermData message = buildOTmessage(commandID);
    if (message.type != OT_MSGTYPE_UNKNOWN_DATAID) {
      childOT->sendToBoiler(message);
      nextMessageTime = t + PAUSE_BETWEEN_MESSAGES;
    }
  }

  if ((millis() - oneMinuteTickerTime) > 60000l) {
    oneMinuteTickerTime = millis();
    oneMinuteCallback();
  }

  if ((millis() - fiveMinuteTickerTime) > 300000l) {
    fiveMinuteTickerTime = millis();
    fiveMinutesCallback();
  }  
}
    
void OTlooper::boilerMessageReceived(OpenthermData &boilerMessage) {
  // if message from boiler arrives -> translate it to the object model and inform all attached data listeners (usually regulator)
  //DPRINTLN(F("message from boiler"));
  switch (boilerMessage.id) {
    case OT_MSGID_STATUS:
      _boilerStatus = boilerMessage.valueLB;
      break;
    case OT_MSGID_FAULT_FLAGS:
      _boilerFaultFlags = boilerMessage.u16();
      break;
    case OT_MSGID_OUTSIDE_TEMP: // ID = 27, R
      outsideTemperature = boilerMessage.f88();
      break;
    case OT_MSGID_OEM_DIAGNOSTIC:
      _boilerOEMDiagnosticCode = boilerMessage.u16();
    case OT_MSGID_FEED_TEMP:
      boilerWaterTemperature = boilerMessage.f88();
      break;    
  }
  nextMessageTime = max(nextMessageTime, millis() + 150);
}


void OTlooper::thermostatMessageReceived(OpenthermData &thermostatMessage) {
  // messages from thermostat are received, processed
  // and answer to the thermostat is sent back
  OpenthermData answerMessage;
  DPRINT(F(D_OTLOOPER_THERMOSTAT_MESSAGE));
  DPRINTLN(thermostatMessage.id);
  delay(22); // if we answer too quickly (see protocol standardization), the thermostat gets confused and reports communication error
  answerMessage = buildAnswerForThermostat(thermostatMessage);
  childOT->sendToThermostat(answerMessage);
}

void OTlooper::sendToThermostat(OpenthermData &messageForThermostat) {
  // do nothing, OTlooper generates the messages itself
  // it should not have any parent of type AbstractOTchannel
}

void OTlooper::sendToBoiler(OpenthermData &messageForBoiler) {
  // do nothing, OTlooper generates the messages itself
  // it should not have any parent of type AbstractOTchannel
}

void OTlooper::throwError() {
  // error, timeout -> would be fine to process it in a smart way
}    

/*
 * build message that will be sent to the boiler
 * - either commanding it with WRITE messages
 * - or requesting info using READ messages
 */
OpenthermData OTlooper::buildOTmessage(int messageID) {
  OpenthermData message;
  message.id = messageID;

  switch (messageID) {
      case OT_MSGID_STATUS: // ID = 0, R - special
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = composeMasterStatusByte();
        message.valueLB = 0;
        break;
      case OT_MSGID_CH_SETPOINT: // ID = 1, W
        message.type = OT_MSGTYPE_WRITE_DATA;
        message.f88(getCHsetpoint());
        break;
      case OT_MSGID_MASTER_CONFIG: // ID = 2, W
        message.type = OT_MSGTYPE_WRITE_DATA;
        message.valueHB = 1;
        message.valueLB = memberID;
        break;        
      case OT_MSGID_FAULT_FLAGS: // ID = 5, R
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = 0;
        message.valueLB = 0;      
        break;
      /* case OT_MSGID_ROOM_SETPOINT: // ID = 16, W
        message.type = OT_MSGTYPE_WRITE_DATA;
        //message.f88(_regulator->getRoomSetpoint()); // TODO - in versions with multizone regulator this does not have sense
        //      because the room temperature setpoint can be  different for each room, it is reported by the room regulator
        //      and it is ambiguous, which temperature should be sent to boiler
        break;
      */
      case OT_MSGID_MODULATION_LEVEL: // ID = 17, R
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = 0;
        message.valueLB = 0;
        break;
      /* case OT_MSGID_ROOM_TEMP: // ID = 24, W
        message.type = OT_MSGTYPE_WRITE_DATA;
        //message.f88(_regulator->getRoomTemperature()); // TODO - in versions with multizone regulator this does not have sense
        //      because the room temperature is reported by the room regulator
        //      and it is ambiguous, which temperature should be sent to boiler
        break;
      */
      case OT_MSGID_FEED_TEMP: // ID = 25, R
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = 0;
        message.valueLB = 0;
        break;
      case OT_MSGID_DHW_TEMP: // ID = 26, R
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = 0;
        message.valueLB = 0;
        break;
      case OT_MSGID_DHW_SETPOINT: // ID = 56, R/W
        message.type = OT_MSGTYPE_WRITE_DATA;
        message.f88(getDHWsetpoint());
        break;
      case OT_MSGID_OUTSIDE_TEMP: // ID = 27, R
        message.type = OT_MSGTYPE_READ_DATA;
        message.valueHB = 0;
        message.valueLB = 0;      
        break;        
      default:
        message.type = OT_MSGTYPE_UNKNOWN_DATAID;
        break;
  }

  return message;
}

byte OTlooper::composeMasterStatusByte() {
  byte bitCH = isCHenabled();
  byte bitDHW = isDHWenabled();
  return bitCH | (bitDHW << 1);
}

boolean OTlooper::isCHenabled() {
  return _CHenabled;
}

boolean OTlooper::isDHWenabled () {
  return _DHWenabled;
}

/*
 * build message in reply to thermostat request
 */
OpenthermData OTlooper::buildAnswerForThermostat(OpenthermData thermostatRequestMessage) {
  OpenthermData message;
  message.id = thermostatRequestMessage.id;

        byte loc_DHWpresent = 1;
        byte loc_controlType = 1;
        byte loc_cooling = 1;
        byte loc_DHWconfig = 1;
        byte loc_pumpControl = 1;
        byte loc_CH2present = 0;
        
  switch (thermostatRequestMessage.id) {
      case OT_MSGID_STATUS:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_STATUS"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.valueHB = thermostatRequestMessage.valueHB;
        message.valueLB = _boilerStatus;
        break;
      case OT_MSGID_CH_SETPOINT:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_CH_SETPOINT"));
        message.type = OT_MSGTYPE_WRITE_ACK;
        message.f88(thermostatRequestMessage.f88());
        //message.f88(getCHsetpoint());
        break;
      case OT_MSGID_MASTER_CONFIG:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_MASTER_CONFIG"));
        memberID = thermostatRequestMessage.valueLB;
        message.type = OT_MSGTYPE_WRITE_ACK;
        message.u16(thermostatRequestMessage.u16());
        break;
      case OT_MSGID_SLAVE_CONFIG:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_SLAVE_CONFIG"));
        message.type = OT_MSGTYPE_READ_ACK;

        message.valueHB = loc_DHWpresent | (loc_controlType << 1) | (loc_cooling << 2) | (loc_DHWconfig << 3) | (loc_pumpControl << 4) | (loc_CH2present << 5);
        //message.valueLB = 56; // slave ID - nastavuji stejné, kterým se hlásí kotel
        message.valueLB = memberID;
        break;
      case OT_MSGID_FAULT_FLAGS:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_FAULT_FLAGS"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.u16(_boilerFaultFlags);
        break;        
      case OT_MSGID_MAX_MODULATION_LEVEL:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_MAX_MODULATION_LEVEL"));
        message.type = OT_MSGTYPE_WRITE_ACK;
        message.f88(thermostatRequestMessage.f88());
        break;
      case OT_MSGID_ROOM_SETPOINT: // ID = 16, W
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_ROOM_SETPOINT"));
        fireDataUpdate(DATA_ID_ROOM_TEMPERATURE_SETPOINT,thermostatRequestMessage.f88());        
        message.type = OT_MSGTYPE_WRITE_ACK;
        message.f88(thermostatRequestMessage.f88());        
        break;
      case OT_MSGID_MODULATION_LEVEL:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_MODULATION_LEVEL"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.valueHB = 0;
        message.valueLB = 0;
        break;
      case OT_MSGID_ROOM_TEMP:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_ROOM_TEMP"));
        DPRINTLN(thermostatRequestMessage.f88());
        fireDataUpdate(DATA_ID_ROOM_TEMPERATURE,thermostatRequestMessage.f88());
        message.type = OT_MSGTYPE_WRITE_ACK;
        message.f88(thermostatRequestMessage.f88());
        break;
      case OT_MSGID_FEED_TEMP:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_FEED_TEMP"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.f88(getBoilerWaterTemperature());    
        break;
      case OT_MSGID_DHW_SETPOINT:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_DHW_SETPOINT"));
        if (thermostatRequestMessage.type == OT_MSGTYPE_WRITE_DATA) {
          message.type = OT_MSGTYPE_WRITE_ACK;
          message.f88(thermostatRequestMessage.f88());
        } else {
          message.type = OT_MSGTYPE_READ_ACK;
          message.f88(thermostatRequestMessage.f88());
        }
        break; 
      case OT_MSGID_OUTSIDE_TEMP: // ID = 27, R
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_OUTSIDE_TEMP"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.f88(outsideTemperature);
        break;     
      case OT_MSGID_OEM_DIAGNOSTIC:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("OT_MSGID_OEM_DIAGNOSTIC"));
        message.type = OT_MSGTYPE_READ_ACK;
        message.u16(_boilerOEMDiagnosticCode);
        break;
      default:
        DPRINT(F(D_OTLOOPER_ANSWERING));DPRINTLN(F("UNKNOWN_MESSAGE"));
        message.type = OT_MSGTYPE_UNKNOWN_DATAID;
        message.valueHB = 0;
        message.valueLB = 0;        
        break;
  }

  return message;
}

float OTlooper::getBoilerWaterTemperature() {
  return boilerWaterTemperature;
}


void OTlooper::CHBoilerInterface::enableHeating() {
  _parent->_CHenabled = true;
  _parent->singleUseCommands.push(OT_MSGID_STATUS); 
}

void OTlooper::CHBoilerInterface::disableHeating() {
  _parent->_CHenabled = false;
  _parent->singleUseCommands.push(OT_MSGID_STATUS);
}

void OTlooper::CHBoilerInterface::setBoilerTemperature(float88 newBoilerTemperature) {
  if (_parent->isBoilerTemperatureValid(newBoilerTemperature)) {
    _parent->_targetBoilerTemperature = newBoilerTemperature;
    _parent->singleUseCommands.push(OT_MSGID_CH_SETPOINT); 
  } else {
    //throw some error "invalid boiler temperature setpoint"
  }
}



void OTlooper::DHWBoilerInterface::enableHeating()  {
  _parent->_DHWenabled = true;
  _parent->singleUseCommands.push(OT_MSGID_STATUS);
}

void OTlooper::DHWBoilerInterface::disableHeating() {
  _parent->_DHWenabled = false;
  _parent->singleUseCommands.push(OT_MSGID_STATUS);
}

void OTlooper::DHWBoilerInterface::setBoilerTemperature(float88 newDHWtemperature) {
  _parent->_targetDHWtemperature = newDHWtemperature;
  _parent->singleUseCommands.push(OT_MSGID_DHW_SETPOINT);   
}

float OTlooper::getDHWsetpoint() {
  return _targetDHWtemperature;
}

float OTlooper::getCHsetpoint() {
  return _targetBoilerTemperature;
}

boolean OTlooper::isBoilerTemperatureValid(float boilerTemperature) {
  return (boilerTemperature >= MIN_BOILER_TEMPERATURE) && (boilerTemperature <= MAX_BOILER_TEMPERATURE);
}

void OTlooper::oneMinuteCallback() {
  singleUseCommands.push(OT_MSGID_OUTSIDE_TEMP); 
}

void OTlooper::fiveMinutesCallback() {
  //singleUseCommands.push(OT_MSGID_ROOM_SETPOINT); // TODO
  //singleUseCommands.push(OT_MSGID_ROOM_TEMP); // TODO
  // in versions with multizone regulator this does not have sense
  //      because the room temperature and its setpoint can be  different for each room, they are reported by the room regulator
  //      and it is ambiguous, which temperature should be sent to boiler
}

OTlooper::CHBoilerInterface& OTlooper::getCHBoilerInterface() {
  return _CHBoilerInterface;
}

OTlooper::DHWBoilerInterface& OTlooper::getDHWBoilerInterface() {
  return _DHWBoilerInterface;
}
