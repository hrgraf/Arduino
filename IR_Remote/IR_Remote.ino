/**
 * Decode a NEC InfraRed Remote Control with an Arduino, 
 * by wiring an TSOP38238 to Pin 2.
 */

#include <Arduino.h> // for Intellisense
#include <IRremote.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  irrecv.enableIRIn(); // Start the receiver
  
  Serial.begin(115200);
  Serial.println("IR remote monitor for NEC");  
}

void loop() 
{
  if (irrecv.decode(&results)) 
  {
    if (! results.bits)
    {
      Serial.println("???"); // decoded nothing
    }
    else if (results.decode_type == NEC)
    {
      char *str="unknown";
      Serial.print("NEC: ");
      Serial.print(results.value, HEX);
#if 1
      Serial.print(" : ");
#else     
      Serial.print(" (");
      Serial.print(results.bits, DEC);
      Serial.print("bits) : ");
#endif
      switch(results.value) 
      {
        case 0xFFA25D: str="1"; digitalWrite(LED_BUILTIN, HIGH); break;
        case 0xFF629D: str="2"; break;
        case 0xFFE21D: str="3"; break;
        case 0xFF22DD: str="4"; break;
        case 0xFF02FD: str="5"; break;
        case 0xFFC23D: str="6"; break;
        case 0xFFE01F: str="7"; break;
        case 0xFFA857: str="8"; break;
        case 0xFF906F: str="9"; break;
        case 0xFF9867: str="0"; digitalWrite(LED_BUILTIN, LOW); break;

        case 0xFF18E7: str="up";    break;
        case 0xFF10EF: str="left";  break;
        case 0xFF5AA5: str="right"; break;
        case 0xFF4AB5: str="down";  break;
        case 0xFF38C7: str="OK";    break;
        
        case 0xFF6897: str="*"; break;
        case 0xFFB04F: str="#"; break;
      }
      Serial.println(str);
    }
    else
    {
        Serial.print("Unknown: ");
        Serial.print(results.value, HEX);
        Serial.print(" (");
        Serial.print(results.bits, DEC);
        Serial.println("bits)");
    }
    irrecv.resume(); // Receive the next value
  }
}
