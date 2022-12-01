#include <Arduino.h>
#include <TimeLib.h>
#include "Config.h"
#include "Programmer.h"

ProgramData Programmer::getActiveProgramData() {
  ProgramData* activeData = &defaultProgramData;
  boolean defaultProgrammUsed = true;
  unsigned long timeNow = hour() * 3600l + minute() * 60 + second();
  int dayNow = weekday() - 1;
  #ifdef DEBUG
    char s[128];
    snprintf_P(s, sizeof(s), PSTR(D_PROG_PROGRAMMER), timeNow);
    DPRINTLN(s);
  #endif
  for (int i = 0; i < PROGRAMS_COUNT; i++) {
    #ifdef DEBUG
      snprintf_P(s, sizeof(s), PSTR(D_PROG_PROGRAM_DESCRIPTION), i, programs[i].day, (programs[i].start*60l), (programs[i].stop*60l), programs[i].data.type);
      DPRINTLN(s);      
    #endif
    if (((programs[i].day >> dayNow) & 1) && (programs[i].start * 60l < timeNow) && (programs[i].stop * 60l > timeNow) ) {
      activeData = &programs[i].data;
      if (activeData->isTemporary()) {
        activeData->setTemporaryRunning();
      }
      defaultProgrammUsed = false;
      DPRINT(F(D_PROG_SELECTED_PROGRAM));
      DPRINTLN(i);
      break;
    } else if (programs[i].data.isTemporaryRunning()) {
      DPRINT(F(D_PROG_PROGRAM_DEACTIVATION));
      DPRINTLN(i);
      programs[i].deactivate();
    }
  }

  if (defaultProgrammUsed) {
    DPRINTLN(F(D_PROG_DEFAULT_PROGRAM));
  }
  return *activeData;
}

  
void Programmer::setProgram(int i, Program newProgram){
  //programs[i] = newProgram;
  programs[i].data.type = newProgram.data.type;
  programs[i].data.temperature = newProgram.data.temperature;
  programs[i].day = newProgram.day;
  programs[i].start = newProgram.start;
  programs[i].stop = newProgram.stop;
}

void Programmer::dataUpdated(byte dataID, const char *value) {
  byte programID = 255;
  
  if (dataID == DATA_ID_PROGRAM) {
      int i=0;
      char * pch;
      pch = strtok (value,",");
      while (pch != NULL && i<8)
      {
        switch(i++) {
          case 0:
            // program number
            programID = atoi(pch);
            break;
          case 1:
            // type
            programs[programID].data.type = atoi(pch);
            break;
          case 2:
            // temperature
            programs[programID].data.temperature = atof(pch);
            break;
          case 3:
            // day(s)
            //programs[programID].day = atoi(pch);
            programs[programID].setDay(atoi(pch));           
            break;
          case 4:
            // start hour
            programs[programID].start = 60 * atoi(pch);            
            break;
          case 5:
            // start minute
            programs[programID].start += atoi(pch);            
            break;
          case 6:
            // stop time
            programs[programID].stop = 60 * atoi(pch);            
            break;
          case 7:
            // stop minute
            programs[programID].stop += atoi(pch);  
            break;
        }

        if (programID <0 || programID >= PROGRAMS_COUNT) {
          break;
        }

        pch = strtok (NULL, ",");
      }    

      #ifdef DEBUG
        char s[128];
        if (programID <0 || programID >= PROGRAMS_COUNT) {
          snprintf_P(s, sizeof(s), PSTR(D_PROG_NEW_PROGRAM_WRONG_ID), programID, (PROGRAMS_COUNT-1));
          DPRINTLN(s);
        } else {     
          snprintf_P(s, sizeof(s), PSTR(D_PROG_NEW_PROGRAM_UPDATING), programID, (PROGRAMS_COUNT-1));
          DPRINTLN(s);

          snprintf_P(s, sizeof(s), PSTR(D_PROG_NEW_PROGRAM_DESCRIPTION), programs[programID].data.type, programs[programID].data.temperature, programs[programID].day, programs[programID].start / 60, programs[programID].start % 60, programs[programID].stop / 60, programs[programID].stop % 60);          
          DPRINTLN(s);

          getActiveProgramData();          
        }        
      #endif
  } else {
    DPRINTLN(F(D_PROG_DATA_UPDATED_BUT_NOT_PROGRAM));
  }
}

bool ProgramData::isTemporary() {
  return (type & PROGRAM_TEMPORARY_BIT) > 0;
}

bool ProgramData::isTemporaryRunning() {
  return (type & PROGRAM_TEMPORARY_RUNNING_BIT) > 0;
}

void ProgramData::setTemporaryRunning() {
  type = (type & PROGRAM_TYPE_MASK) | PROGRAM_TEMPORARY_RUNNING_BIT;
}

void Program::deactivate() {
  data.type = PROGRAM_TYPE_OFF;
  data.temperature = PROGRAM_DEFAULT_TEMPERATURE;
  day = 0;
  start = 0;
  stop = 0;
}

void Program::setDay(byte _day) {
  day = _day;
}
