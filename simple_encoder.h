#ifndef SIMPLE_ENCODER
#define SIMPLE_ENCODER
//============  ENCODER ================

#include <TimerOne.h>
//Encoder pins
#define pinA A0
#define pinB A1
#define btn  A2
#define EncActive LOW //contacts and butto active state

volatile int16_t EncVal;
volatile int16_t EncMin;
volatile int16_t EncMax;
volatile int8_t  EncWrap;

//Set new current encoder value
inline void set_encoder(int16_t val)
{
  cli();
  EncVal = val;
  sei();
}

//returns currebt encoder value
inline int16_t get_encoder()
{ 
  
  return EncVal;
}
void set_encoder_limits(int16_t min, int16_t max, uint8_t wrap)
{
  EncMin = min;;
  EncMax = max;
  EncWrap =wrap;
}

//returns current button state
bool encoder_button()
{ 
  if (digitalRead(btn) == EncActive) return true; 
  return false;
}

//init emcoder pins and sets 
// start  - encoder current value
// min    - minimumm allowed value
// max    - maximum allowed value
void init_encoder(int16_t start, int16_t min, int16_t max, uint8_t wrap)
{
  byte mode = INPUT;
  if (EncActive == 0) mode =  INPUT_PULLUP;
  pinMode(pinA, mode);
  pinMode(pinB, mode);
  pinMode(btn, mode);
  EncVal = start;
  EncMin = min;
  EncMax = max;
  EncWrap = wrap;
}

//This function should be invoked once every 1-2 milliseconds to process encoder
void do_encoder()
{
  enum {
    WaitingForTransition,
    WaitingForBothActive,
    WaitingForBothInactive,
  };
  static uint8_t state = WaitingForTransition;

  switch (state)
  {
    case WaitingForTransition:
      if (digitalRead(pinA) == EncActive)
      {
        state = WaitingForBothActive;
        if (digitalRead(pinB) == EncActive)
          EncVal++;
        else
          EncVal--;
      }
      if (EncVal < EncMin)
      {
        EncVal = EncWrap ? EncMax : EncMin;
      }
      if (EncVal > EncMax)  
      {
        EncVal = EncWrap ? EncMin : EncMax;
      }
      break;

    case WaitingForBothActive:
      if ((digitalRead(pinA) == EncActive) && (digitalRead(pinB) == EncActive))
      {
        state = WaitingForBothInactive;
      }
      break;

    case  WaitingForBothInactive:
      if ((digitalRead(pinA) != EncActive) && (digitalRead(pinB) != EncActive))
      {
        state = WaitingForTransition;
      }
      break;
  }
}
//============  END ENCODER ================
#endif // SIMPLE_ENCODER
