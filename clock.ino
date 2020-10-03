#include <Arduino.h>
#include <DS3231.h>
#include <Wire.h>

#include "simple_encoder.h"
#include "moving_average.h"
#include "display_tm1637.h"



#define LOW_BR   2
#define LOW_T   50   //предел освешенности для минимальной яркости
#define MED_BR   4
#define MED_T  200   //предел освешенности для средней яркости 
#define HIGH_BR  7
#define BR_DELAY 5000 //Меняем яркость не чаще чем раз в  5 sec

#define DEBOUNCE 200
#define TIMEOUT 4000  //Таймаут при настройке часов. Если энкодер перестали крутить, то возращаемся в показ времени через 4 сек
DS3231 Clock;


#define SZ 32  //Усредняем по SZ значениям яркости
avrgData_t avdata;
byte buf[SZ];

//Change 2 digit value using encoder and shows it on display
//return 0 on timeout
//return 1 on button press
byte SetVal(byte &val, byte max, byte pos)
{
  uint32_t t_start = millis();
  uint32_t last_change = t_start;
  byte timeout = 0;
  set_encoder_limits(0, max, true);
  set_encoder(val); 
  int16_t oldEnc = val;
  DisplayShow2Digits(val,pos);

  while (1)
  {
    int16_t  newEnc = get_encoder();
    if (oldEnc != newEnc)
    {
      oldEnc = newEnc;
      last_change = millis();
      DisplayShow2Digits(newEnc,pos);
    }
    if ((millis() - t_start) > DEBOUNCE)
    {
      val = byte(newEnc);
      if (encoder_button())  return 1;
      if (((millis() - last_change)) > TIMEOUT) return 0;
    }
  }
}

int8_t GetBrightness()
{
  byte b = analogRead(A7)>>2; //
  b = GetNextAvrg(&avdata,b);
  if (b<LOW_T) return LOW_BR;
  if (b<MED_T) return MED_BR;
  return HIGH_BR;
}
void SetBrightness(byte br)
{
  static byte current = HIGH_BR;
  static uint32_t lastUpdate = 0;
  if (((millis() - lastUpdate) < BR_DELAY) || br == current) return;
  if (current < br) ++current;
  if (current > br) --current;
  DisplaySetBrightness(current);
  lastUpdate = millis();
  Serial.println((String) "Set brightness to " + current);
}

void setup()
{
  Serial.begin(115200);
  Serial.print("Setup");
  Wire.begin();
  init_encoder(0, 0, 60, true);
  Timer1.initialize(1000);            //Настраиваем таймер на прерывания раз в миллисекунду
  Timer1.attachInterrupt(do_encoder); //Для обслуживания энкодера
  DisplaySetBrightness(HIGH_BR);
  avdata.buf = buf;
  InitAvrg(&avdata,SZ);
  Serial.println(" done");
}

void loop()
{
  static bool dots = false;
  static byte prev_s = 0;
  bool h12, PM;
  //Получаем время из RTC
  byte s = Clock.getSecond();
  byte m = Clock.getMinute();
  byte h = Clock.getHour(h12, PM);
  if (h12 && PM) h += 12;  //если 12 часовой формат, то переводи в 24 часовой  
   
  //раз в секунду выводим время на дисплей
  if (prev_s != s)
  {
    prev_s = s;   
    dots = ! dots; //зажигаем двоеточие каждую вторую секунду
    DisplayShowTime(h,m,dots);
  }
  
  //проверяем нажата ли кнопка на энкодере
  if (encoder_button())
  {
    Serial.print((String)"Btn! ");
    byte ret = SetVal(m, 60, 2); //Получаем новое значение для минут
    Clock.setMinute(m);          //Сохраняем его в RTC
    Serial.println((String)"M-" + m);
    if (ret)                     //Eсли кнопка была нажата сразу после установки минут, то настраиваем часы, 
    {                            //а если не была, то после установки минут по таймауту продолжаем показывать время 
      SetVal(h, 24, 0);          //Получаем новое значение часв
      Serial.println((String)"H-" + h);
      Clock.setHour(h);          //Сохраняем его в RTC
      delay(DEBOUNCE);  //для устранения дребезга
    }                    
  }
  SetBrightness(GetBrightness());
  delay(50);
}
