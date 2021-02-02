/**
 * Arduino Synthesizer by H.R.Graf
 */

#include "Arduino.h" // for Intellisense

// Need a PWM capable pin
#define PIN_OUT 9

// frequency in Hz
#define C4  262
#define D4  294
#define E4  330
#define F4  349
#define G4  392
#define A4  440
#define H4  494
#define C5  523

#define MELODY_LEN 8
const int melody[MELODY_LEN] = { C4, D4, E4, F4, G4, A4, H4, C5 };

void setup()
{
  // initialize the serial communication:
  Serial.begin(115200);
  Serial.print("Arduino Synthesizer on pin ");
  Serial.println(PIN_OUT);
}

void loop()
{
  Serial.println("Simple scale");
  pinMode(PIN_OUT, OUTPUT);
  for (int i=0; i<MELODY_LEN; i++)
  {
      tone(PIN_OUT, melody[i]);
      delay(200);
  }
  noTone(PIN_OUT);

  Serial.println("Change timbre by PWM duty cycle");
  for (int i=0; i<255; i++) 
  {
    analogWrite(PIN_OUT, i);
    delay(10);
  }
  noTone(PIN_OUT);

  //delay(1000);
}
