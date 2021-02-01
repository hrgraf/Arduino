/*
  LED control over serial communication.
  
  From Raspian terminal, type
    echo "1" > /dev/ttyACM0
  or
    echo "0" > /dev/ttyACM0
 */

#include "Arduino.h" // for Intellisense

const int ledPin = LED_BUILTIN;

void setup()
{
  // initialize the ledPin as an output:
  pinMode(ledPin, OUTPUT);
  
  // initialize the serial communication:
  Serial.begin(115200);
  Serial.println("Send 0 or 1 to control LED");
}

void loop() 
{
  // check if data has been sent from the computer:
  if (Serial.available()) 
  {
    // read the most recent byte
    byte val = Serial.read();
    
    // set the brightness of the LED:
    if (val == '0')
      digitalWrite(ledPin, LOW);
    else if (val == '1')
      digitalWrite(ledPin, HIGH);
  }
}
