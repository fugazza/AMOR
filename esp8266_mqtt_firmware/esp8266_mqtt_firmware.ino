/*  
 *  esp8266_mqtt_firmware
 *  version: v1.1
 *  Board: ESP8266
 *  
 *  Firmware responsible for linking Arduino with installed AMOR (Arduino Modular Opentherm Regulator) to the MQTT server via wifi.
 *  Does following tasks:
 *  - listens on serial port, waits for messages in format "#P:[topic]=[value]\r\n" which are then forwarded
 *    to the MQTT server -> [value] is published to the MQTT [topic]
 *  - if the message in format "#S:[topic]\r\n" is received, then it subscribes to the MQTT [topic] 
 *    when new [value] is received through this [topic], then following message is sent to the serial port: "V=[topic]:[value]"   
 *  - sends an actual date-time info to the serial port in regular intervals. Format is "T=[dd].[mm].[yyyy] [hh]:[ii]:[ss]"
 *  - after startup it announces the underlaying Arduino, that it has started by message "S=started"
 *  
 */

/* configuration section --------------------------------------------------------------------------------------------------
 * modify values to your needs
 */
/* SSID = your wifi name */
#define WIFI_SSID "your_wifi_name"

/* password to your wifi */
#define WIFI_PASS "your_wifi_password"

/* IP address of your MQTT server */
#define MQTT_IP "192.168.1.1"

/* port number on which your MQTT server listens
 * standard value 1883
 */
#define MQTT_PORT 1883

/* username used to authenticate to the MQTT server */
#define MQTT_USER ""

/* password for MQTT server */
#define MQTT_PASS ""

/* address of NTP server, which provides actual date-time
 * default value: "pool.ntp.org"
 */
#define NTP_SERVER "pool.ntp.org"

/* how often the time is is provided from ESP8266 to underlaying Arduino
 * default value: 1200 (= 1200 s = 20 min)
 */
#define NTP_UPDATE_INTERVAL 1200

/* timezone definition for NTP time update
 * choose your timezone from the list at https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
 * or modify according to this logic:
 *   daylight savings time starts at March = month (M3), fifth week (5), on Sunday (0) at two o'clock (02)
 *   winter time starts at October (M10), fifth week (5), on Sunday (0) at three o'clock (03)
 * default value: "CET-1CEST,M3.5.0/02,M10.5.0/03"
 */
#define NTP_TIMEZONE "CET-1CEST,M3.5.0/02,M10.5.0/03" 

/* debugging messages */
//#define DEBUG  // Comment this line to disable debug serial output.

/* END OF configuration section -------------------------------------------------------------------------------------------
 * usually no need to modify anything below this line 
 */


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TickTwo.h>
#include <time.h>
#include "config.h"

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif

// serial port status flags
#define SERIAL_NOTHING_RECEIVED     0
#define SERIAL_START_CHAR           1
#define SERIAL_COMMAND_CHAR         2
#define SERIAL_DELIMITER_CHAR       4
#define SERIAL_HAVE_TOPIC           8
#define SERIAL_CR                   16
#define SERIAL_MESSAGE_RECEIVED     32
#define SERIAL_MESSAGE_FORMAT_ERROR 128

byte serialState = SERIAL_NOTHING_RECEIVED;
char c;
char commandChar;

// wifi variables
const String chipId = String(ESP.getChipId());
const String hostName = "UNO_WIFI_" + chipId;
IPAddress ip;
boolean wifiConnected = false;

// MQTT variables
WiFiClient WiFiClient;
void messageReceivedCallback(const char topic[], byte* payload, unsigned int length); // forward declaration - the definition of the callback function is below
PubSubClient client(MQTT_IP, MQTT_PORT, messageReceivedCallback, WiFiClient);
#define MAX_TOPIC_LENGTH 64
#define SUBSCRIBED_TOPICS_LIST_LENGTH 20
char mqttTopic[MAX_TOPIC_LENGTH];
char mqttValue[64];
boolean mqttConnected = false;
char subscribedTopics[SUBSCRIBED_TOPICS_LIST_LENGTH][MAX_TOPIC_LENGTH];

// variables for time synchronization through NTP
boolean firstNTPsynchro = false;
void callbackNTPupdate(); // forward declaration - the definition of the callback function is below
TickTwo tickerNTPupdate(callbackNTPupdate, 10000); // first synchronizations occurs 10.000ms = 10s after startup of this ESP8266

void setup() {
  Serial.begin(115200); 
  delay(50);
  DPRINTLN(F("esp8266_mqtt_firmware - version 1.1"));
  DPRINTLN(F("-----------------------------------"));

  DPRINT(F("MAC: "));      
  DPRINTLN(WiFi.macAddress());
  
  // connect to the wifi
  connectWifi();

  // connect to the MQTT server
  connectMQTT();

  // start the NTP time synchronization
  configTime(NTP_TIMEZONE, NTP_SERVER);

  // start the ticker (timer, scheduler) for NTP synchronizations
  tickerNTPupdate.start();

  // announce Arduino, that the ESP8266 wifi module has started and is ready to listen
  Serial.println("S=started");  
}

 
void loop() {

  // check the serial port
  if (Serial.available()) {
    while(Serial.available()) {
      c = Serial.read();

      if (c == '\r' && serialState != SERIAL_MESSAGE_FORMAT_ERROR) {
        serialState = SERIAL_CR;
      } else if (c == '\n') {
        if (serialState == SERIAL_CR) {
          serialState = SERIAL_MESSAGE_RECEIVED;
          break;
        } else if (serialState == SERIAL_MESSAGE_FORMAT_ERROR) {
          clearSerialLineForNextMessage();
          DPRINTLN(F("ERROR: wrong message format - discarding message"));
        } else {
          clearSerialLineForNextMessage();
          DPRINTLN(F("ERROR: \\n received without \\r - discarding message"));
        }
      } else {
        switch (serialState) {
          case SERIAL_NOTHING_RECEIVED:
            if (c=='#') {
              serialState = SERIAL_START_CHAR;
            } else {
              DPRINT(F("ERROR: char # not found at the beginning of the message - '"));
              DPRINT(c);
              DPRINTLN(F("' found instead"));
              serialState = SERIAL_MESSAGE_FORMAT_ERROR;
            }
            break;
          case SERIAL_START_CHAR:
            commandChar = c;
            serialState = SERIAL_COMMAND_CHAR;
            break;
          case SERIAL_COMMAND_CHAR:
            if (c==':') {
              serialState = SERIAL_DELIMITER_CHAR;
            } else {
              DPRINT(F("ERROR: char ':' (semicolon) not found after command char - '"));
              DPRINT(c);
              DPRINTLN(F("' found instead"));
              serialState = SERIAL_MESSAGE_FORMAT_ERROR;
            }
            break;
          case SERIAL_DELIMITER_CHAR:
            if (c=='=') {
              serialState = SERIAL_HAVE_TOPIC;
            } else {
              strncat(mqttTopic,&c, 1);
            }
            break;
          case SERIAL_HAVE_TOPIC:
            strncat(mqttValue,&c, 1);
            break;         
        }   
      }     
    }
  }
  

  // if message received then we will forward it to the MQTT server
  if (serialState == SERIAL_MESSAGE_RECEIVED) {

    // first check the wifi connection and reconnect if disconnected
    wifiConnected = WiFi.isConnected();
    if (!wifiConnected) {
      connectWifi();
      wifiConnected = WiFi.isConnected();
    }

    if (wifiConnected) {
      // after wifi succesfully connected
      // check that the connection to the MQTT server exists and eventually reconnect    
      mqttConnected = client.connected();
      if (!mqttConnected) {
        mqttConnected = connectMQTT();
      }

      if (mqttConnected) {
        // after we are connected to the MQTT server, we can process the commands

        switch (commandChar) {
          case 'P': 
            // publish = send the MQTT message
            publishMQTTMessage(mqttTopic, mqttValue); 
            break;
          case 'S':
            // subscribe
            if (!isTopicSubscribed(mqttTopic)) {
              DPRINT(F("DEBUG: Subscribing to topic '")); 
              DPRINT(mqttTopic);
              DPRINTLN(F("'"));
              client.subscribe(mqttTopic);
              registerSubscribedTopic(mqttTopic);
            } else {
              DPRINT(F("DEBUG: Topic '")); 
              DPRINT(mqttTopic);
              DPRINTLN(F("' is already subscribed."));              
            }
            break;
          default:
            DPRINT(F("ERROR: Unknown command '"));      
            DPRINT(commandChar);
            DPRINTLN(F("'"));
            break;
        }
      } else {
        DPRINTLN(F("ERROR: Not connected to the MQTT server."));
      }
       
    } else {
      // we are not connected to the wifi
      // would be nice to throw some meaningfull error to know, that there are problems with wifi
      DPRINTLN(F("ERROR: WiFi not connected, cannot send mqtt messages."));
    }

    // when the message is processed then it is time to prepare for listening to next message 
    clearSerialLineForNextMessage();
  }

  // check the incoming MQTT messages in subscribed topics
  client.loop();

  // update the NTP synchronization scheduler
  tickerNTPupdate.update();

} // end loop();

void clearSerialLineForNextMessage() {
  serialState = SERIAL_NOTHING_RECEIVED;
  mqttTopic[0] = '\0';    
  mqttValue[0] = '\0';  
}

void connectWifi() {
  DPRINTLN();
  DPRINT(F("Connecting to "));
  DPRINTLN(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostName);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DPRINT(".");
  }
  DPRINTLN();
  DPRINTLN(F("WiFi connected"));
  DPRINT(F("IP address: "));
  ip = WiFi.localIP();
  DPRINTLN(ip);  
}

boolean connectMQTT() {
  if (!client.connected()) {
    DPRINT(F("DEBUG: MQTT server not connected (client state = "));
    DPRINT(client.state());
    DPRINT(F(") - attempting connection ..."));
    if (client.connect(chipId.c_str(), MQTT_USER, MQTT_PASS)) {
      DPRINTLN(F(" success"));

      /* after the connection to the MQTT server is established
       * subscribe to all the topics, where underlaying Arduino expects the data from
       * because if the connecion to the MQTT server was lost, then it is probable
       * that also all the subscriptions were forgotten
      */
      DPRINTLN(F("DEBUG: re-subscribing following topics:"));
      for(int i=0; i<SUBSCRIBED_TOPICS_LIST_LENGTH; i++) {
        if (strlen(subscribedTopics[i]) > 0) {
          DPRINT(F("   "));
          DPRINTLN(subscribedTopics[i]);
          client.subscribe(subscribedTopics[i]);
        }    
      }      
      return true;
    } else {
      DPRINT(F(" failed, rc="));
      DPRINTLN(client.state());
      return false;
    }
  } else {
    DPRINTLN(F("DEBUG: MQTT server already connected."));
    return true;  // already connected 
  }
}

void publishMQTTMessage(const char topic[], const char value[]) {
  DPRINTLN(F("publishing values"));
  DPRINT(F("topic: "));
  DPRINT(topic);
  DPRINT(F("; value: "));
  DPRINTLN(value);      
  client.publish(topic, value); 
}

void callbackNTPupdate() {
  // check the wifi connection first and eventually reconnect
  wifiConnected = WiFi.isConnected();
  if (!wifiConnected) {
    connectWifi();
    wifiConnected = WiFi.isConnected();
  }

  if (wifiConnected) {
    // we can synchrnize the time through NTP once we are successfully connected to the wifi
    time_t now;
    tm tm; 

    time(&now);
    localtime_r(&now, &tm);


    // send the updated date time to the serial port
    // in format "T=[dd].[mm].[yyyy] [hh]:[ii]:[ss]"
    Serial.print("T=");
    Serial.print(tm.tm_mday);
    Serial.print(".");
    Serial.print((tm.tm_mon+1));
    Serial.print(".");
    Serial.print((tm.tm_year+1900));
    Serial.print(" ");
    Serial.print(tm.tm_hour);
    Serial.print(":");
    Serial.print(tm.tm_min);
    Serial.print(":");
    Serial.println(tm.tm_sec); 
    
    // first NTP synchronization occurs soon (10 seconds) after startup of the ESP8266
    // following synchronizations occur repeatedly in NTP_UPDATE_INTERVAL
    if (!firstNTPsynchro) {
      firstNTPsynchro = true;
      tickerNTPupdate.interval(1000l * NTP_UPDATE_INTERVAL);
    }     
  } else {
    // we are not connected to the wifi
    // would be nice to throw some meaningfull error to know, that there are problems with wifi
    DPRINTLN(F("ERROR: WiFi not connected, cannot synchronize time over NTP protocol."));
  }

}

void messageReceivedCallback(const char topic[], byte* payload, unsigned int length) {
  Serial.print(F("V="));
  Serial.print(topic);
  Serial.print(F(":"));
  for (int i=0; i<length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

bool isTopicSubscribed(const char *topic) {
  for(int i=0; i<SUBSCRIBED_TOPICS_LIST_LENGTH; i++) {
    if (strcmp(topic,subscribedTopics[i]) == 0) {
      return true;
    }    
  }

  return false;
}

void registerSubscribedTopic(const char *topic) {
  for(int i=0; i<SUBSCRIBED_TOPICS_LIST_LENGTH; i++) {
    if (strlen(subscribedTopics[i]) == 0) {
      strcpy(subscribedTopics[i], topic);
      break;
    }    
  }
}

/*
 * sets up that the NTP time synchronization starts earlier than default 60s
 */
uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 ()
{
  return 8000UL; // 8s
}
