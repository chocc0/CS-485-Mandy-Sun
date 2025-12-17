#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 4
#define DIO 5

const int indexB = A4;
const int palmB = A3;

const int a = 0b00000001;
const int b = 0b00000010;
const int c = 0b00000100;
const int d = 0b00001000;
const int e = 0b00010000;
const int f = 0b00100000;
const int g = 0b01000000;

const uint8_t SEG_DONE[] = {
	b | c | d | e | g,           // d
	a | b | c | d | e | f,   // O
	e | f | b | c | a,                           // n
	a | d | e | f | g            // E
	};


const uint8_t digits[] = {
  0b00111111, // 0  
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111 // 9
};

TM1637Display display(CLK, DIO);

void setup()
{
  // put your setup code here, to run once:
  pinMode(indexB, INPUT_PULLUP);
  pinMode(palmB, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop()
{
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0b00111111, 0b00111111, 0b00111111, 0b00111111 };
  display.setBrightness(0x0f);

  int num = 0;
  int pnum = num;
  bool updated = false;
  while(true){
    if(analogRead(indexB) < 100 && analogRead(palmB) < 100) { // reset if pressure is high on both sensors
      num = 0;
      for (int x = 0; x < 4; x++){
        blank[x] = digits[0];
      }
      display.setSegments(blank);

      delay(250);
      updated = true;
      continue;
    }

    float avgIndex = 0;
    float avgPalm = 0;

    for(int x = 0; x < 15; x++){
      avgIndex += analogRead(indexB);
      avgPalm += analogRead(palmB);
      delay(1);
    }

    avgIndex /= 15;
    avgPalm /= 15;

    Serial.print((avgIndex));
    Serial.print(" ");
    Serial.println((avgPalm));
    
     // hand must be bent and finger must be bending to increment count
    if ((avgPalm <= 350 && avgIndex <= 235) && !updated){
      num++;
      Serial.println("^^^VALID^^^");

      for (int x = 0; x < 4; x++){
        blank[x] = 0;
      }

      int ones = num % 10;
      int tens = (num % 100) / 10;
      int hund = (num % 1000) / 100;
      int thous = num / 1000;
      blank[0] = blank[0] | digits[thous];
      blank[1] = blank[1] | digits[hund];
      blank[2] = blank[2] | digits[tens];
      blank[3] = blank[3] | digits[ones]; 

      display.setSegments(blank);
      updated = true;
    } else if ((avgPalm <= 350 && avgIndex <= 235) && updated) {
      display.setSegments(blank);
      updated = true;
    } else {
      updated = false;
    }
    
    pnum = num;
    delay(200);
  }

  display.setSegments(SEG_DONE);
  delay(1000);
}
