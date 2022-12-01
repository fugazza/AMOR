#ifndef Programmer_h
#define Programmer_h

#include "Config.h"
#include "Float88DataType.h"
#include "DataListener.h"

#define PROGRAM_TYPE_OFF 0
#define PROGRAM_TYPE_FIXED_SETPOINT 1
#define PROGRAM_TYPE_PID 2
#define PROGRAM_TYPE_PID_THERMOSTAT_TEMPERATURE 3
#define PROGRAM_TYPE_MASK             0b00111111
#define PROGRAM_TEMPORARY_BIT         0b10000000
#define PROGRAM_TEMPORARY_RUNNING_BIT 0b01000000

#define PROGRAM_DAY_MONDAY    0b00000010
#define PROGRAM_DAY_TUESDAY   0b00000100
#define PROGRAM_DAY_WEDNESDAY 0b00001000
#define PROGRAM_DAY_THURSDAY  0b00010000
#define PROGRAM_DAY_FRIDAY    0b00100000
#define PROGRAM_DAY_SATURDAY  0b01000000
#define PROGRAM_DAY_SUNDAY    0b00000001
#define PROGRAM_DAY_ALL       0b01111111
#define PROGRAM_DAY_WORKDAY   0b00111110
#define PROGRAM_DAY_WEEKEND   0b01000001

#ifndef PROGRAM_DEFAULT_TEMPERATURE
  #define PROGRAM_DEFAULT_TEMPERATURE 10.0
#endif

#ifndef PROGRAMS_COUNT
  #define PROGRAMS_COUNT   5
#endif

struct ProgramData {
  // memory size 3 bytes = 1 (byte) + 2 (float88)
  byte type;
  float88 temperature;

  bool isTemporary();
  bool isTemporaryRunning();
  void setTemporaryRunning();  
} ;

struct Program {
  // memory size 8 bytes = 3 (ProgramData) + 1 (byte) + 2 * 2 (short)
  ProgramData data;
  byte day;
  unsigned short start;
  unsigned short stop;

  Program():data({.type = PROGRAM_TYPE_OFF, .temperature = PROGRAM_DEFAULT_TEMPERATURE}) {};
  
  Program(ProgramData _data, int _day, int startHour, int startMinute, int stopHour, int stopMinute) {
    data = _data;
    day = _day;
    start = 60 * startHour + startMinute;
    stop = 60 * stopHour + stopMinute;
    };

  void deactivate();
  void setDay(byte _day);
};

class Programmer: public DataListener
{
  public:
    ProgramData getActiveProgramData();
    void setProgram(int i, Program newProgram);
    void dataUpdated(byte dataID, const char *value);
    void dataUpdated(byte dataID, float88 value) {};

  private:
    ProgramData defaultProgramData = {.type = PROGRAM_TYPE_OFF, .temperature = PROGRAM_DEFAULT_TEMPERATURE};
    Program programs[PROGRAMS_COUNT];
   
};
#endif
