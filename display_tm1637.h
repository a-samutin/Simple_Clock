#include <TM1637Display.h>

// Display Module connection pins (Digital Pins)
#define CLK 5
#define DIO 4

TM1637Display display(CLK, DIO);

void DisplaySetBrightness(byte br)
{
  display.setBrightness(br);
}
void DisplayShowTime(byte h, byte m, bool dots)
{
  uint16_t dtime = uint16_t (h) * 100 + m; //сдвигаем часы на 2 десятичные позиции влево, а в 2 правые позиции добавляем минуты
  byte dts = 0;
  if (dots) dts = 0b01000000;
  byte leading_zero = 0;
  if (!h) leading_zero = 1; //делаем так, что бы 0 часов отображалось как 00:
  display.showNumberDecEx(dtime, dts, leading_zero, 4, 0);
}

//Показываем 2 цифры начиная с позиции pos. 0 - самая левая
void DisplayShow2Digits(byte val, byte pos)
{
  display.clear();
  display.showNumberDec(val, false, 2, pos);
}
