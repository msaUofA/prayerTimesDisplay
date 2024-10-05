#include "TM1637.h"

#define CLK1 27 
#define DIO1 14 

#define CLK2 25 
#define DIO2 33 

#define CLK3 22 
#define DIO3 23 

#define CLK4 18 
#define DIO4 19 

#define CLK5 2
#define DIO5 0

TM1637 display1;
TM1637 display2;
TM1637 display3;
TM1637 display4;
TM1637 display5;


const uint8_t SEGMENTS[] = {0b01111111, 0b01111111, 0b01111111, 0b01111111}; 

void setup() {

  display1.begin(CLK1, DIO1, 4);
  display2.begin(CLK2, DIO2, 4);
  display3.begin(CLK3, DIO3, 4);
  display4.begin(CLK4, DIO4, 4);
  display5.begin(CLK5, DIO5, 4);

  display1.setBrightness(7);
  display2.setBrightness(7);
  display3.setBrightness(7);
  display4.setBrightness(7);
  display5.setBrightness(7);

  display1.displayTime(5, 42, 1);
  display2.displayTime(1, 23, 1);
  display3.displayTime(4, 22, 1);
  display4.displayTime(7, 05, 1);
  display5.displayTime(8, 55, 1);
}

void loop() {}
