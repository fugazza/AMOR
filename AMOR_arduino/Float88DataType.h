#ifndef Float88DataType_h
#define Float88DataType_h

#include <Arduino.h>

class float88 {
  private:
    int16_t f88data;

    void setVal(float newVal) {
      f88data = (newVal * 256);   
    }

  public:
    float88() {
      f88data = 0;
    }
  
    float88(float newVal) {
      setVal(newVal);
    }

    float88& operator= (float newVal) {
      setVal(newVal);
      return *this;      
    }
    
    operator float() const {      
      return f88data / 256.0;
    }   
    
};

#endif
