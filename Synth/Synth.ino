/**
 * Arduino Synthesizer by H.R.Graf
 */

#include "Arduino.h" // for Intellisense
#include <avr/interrupt.h> // Use timer interrupt library
#include <avr/pgmspace.h>

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

#define WAVE_LEN 256 // do not changle
static byte wave[WAVE_LEN];
static byte sine[WAVE_LEN];

static const byte rom[WAVE_LEN] __ATTR_PROGMEM__ = 
{     
  128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,173, 
  176,179,182,185,187,190,193,195,198,201,203,206,208,210,213,215,
  217,219,222,224,226,228,230,231,233,235,236,238,240,241,242,244,
  245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,
  255,254,254,254,254,254,253,253,252,251,251,250,249,248,247,246,
  245,244,242,241,240,238,236,235,233,231,230,228,226,224,222,219,
  217,215,213,210,208,206,203,201,198,195,193,190,187,185,182,179,
  176,173,170,167,164,161,158,155,152,149,146,143,140,137,134,131,
  128,124,121,118,115,112,109,106,103,100, 97, 94, 91, 88, 85, 82,
  79,  76, 73, 70, 68, 65, 62, 60, 57, 54, 52, 49, 47, 45, 42, 40,
  38,  36, 33, 31, 29, 27, 25, 24, 22, 20, 19, 17, 15, 14, 13, 11,
  10,   9,  8,  7,  6,  5,  4,  4,  3,  2,  2,  1,  1,  1,  1,  1,
  1,    1,  1,  1,  1,  1,  2,  2,  3,  4,  4,  5,  6,  7,  8,  9,
  10,  11, 13, 14, 15, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 36,
  38,  40, 42, 45, 47, 49, 52, 54, 57, 60, 62, 65, 68, 70, 73, 76,
  79,  82, 85, 88, 91, 94, 97,100,103,106,109,112,115,118,121,124,
};

ISR(TIMER2_COMPA_vect) // Called when TCNT2 == OCR2A
{
  static byte index=0; // Points to each table entry
  OCR1AL = wave[index++]; // Update the PWM output
  asm("NOP;NOP;"); // Fine tuning
  TCNT2 = 6; // Timing to compensate for ISR run time
}

static void dumpWave()
{
  for (int i=0; i<WAVE_LEN; i++)
  {
    Serial.print(" ");
    Serial.print(wave[i]);
  }
  Serial.println();
  Serial.println();
}

void setup()
{
  // initialize the serial communication:
  Serial.begin(115200);
  Serial.print("Arduino Synthesizer on pin ");
  Serial.println(PIN_OUT);

  // create sine wave table
  for (int i=0; i<WAVE_LEN; i++)
    sine[i] = int(127 - 127*cos(TWO_PI/WAVE_LEN*i));

  // Set timer1 for 8-bit fast PWM output
  pinMode(PIN_OUT, OUTPUT);
  TCCR1B  = (1 << CS10);   // Set prescaler to full 16MHz
  TCCR1A |= (1 << COM1A1); // Pin low when TCNT1=OCR1A
  TCCR1A |= (1 << WGM10);  // Use 8-bit fast PWM mode
  TCCR1B |= (1 << WGM12);
 
  // Set timer2 to call ISR
  TCCR2A = 0; // No options in control register A
  TCCR2B = (1 << CS21); // Set prescaler to divide by 8
  TIMSK2 = (1 << OCIE2A); // Call ISR when TCNT2 = OCRA2
  OCR2A = 18; // Set frequency of generated wave (2MHz/256/18=434Hz)
 
  // Enable interrupts to generate waveform!
  sei(); 
}

void loop() 
{
  Serial.println("Sine");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = sine[i];
  dumpWave();
  delay(2000);

  Serial.println("Ramp");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = i;
  dumpWave();
  delay(2000);

  Serial.println("Square");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = (i < WAVE_LEN/2) ? 0 : 255;
  dumpWave();
  delay(2000);

  Serial.println("Pulse");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = (i < WAVE_LEN*3/4) ? 0 : 255;
  dumpWave();
  delay(2000);

  Serial.println("Triangle");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = (i < WAVE_LEN/2) ? (2*i) : (511-2*i);
  dumpWave();
  delay(2000);

  Serial.println("ROM");
  for (int i=0; i<WAVE_LEN; i++)
    wave[i] = rom[i];
  dumpWave();
  delay(2000);
}

#if 0
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
#endif