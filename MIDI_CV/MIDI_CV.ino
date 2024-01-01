/**
 * Arduino MIDI CV by H.R.Graf
 *
 * Works out-of-the-box on Arduino Uno / Leonardo compatible boards.
 * No MIDI DIN circuit needed, as MIDI events are sent/received over USB.
 *
 * Received Note On/Off control built-in LED and GATE_OUT, and creates
 * pitch control voltage (0..5V) using DAC MCP4725 over I2C.
 *
 * On the Arduino Uno, the serial interface is used to send MIDI over USB
 * at 115200 baud. On the host (PC), the USB communication is accessible 
 * as virtual COM port. There, some additional SW is required to translate 
 * between UART and MIDI. The baud rate of 115200 is compatible to both
 * - Hairless MIDI to Serial Bridge
 * - my pizmidi/midiUartBridge (recommended!)
 *
 * On the Arduino Leonardo, the USB MIDI functionality is built-in and the 
 * serial interface is available for serial monitor / debug messages.
 * No additional drivers needed on the host (PC) side.
 */

#include <Arduino.h> // for Intellisense

#include "Wire.h"
#include "MCP4725.h"

#define BAUD_RATE 115200 // UART
#define GATE_OUT      12 // pin
#define PITCH_ADDR  0x60 // MCP4725 base addr
#define MOD_ADDR    0x61 // MCP4725 alternative addr
#define BASE_KEY      36 // C2=C @ 65.41Hz
 
#if defined(ARDUINO_AVR_LEONARDO)
  #define LED_BLINK LED_BUILTIN // 13
  //#define LED_BLINK 17 // RX LED on Pro Micro
  
  #define DEBUG_INIT(x) { Serial.begin(x); while (!Serial) ; }
  #define DEBUG(x) Serial.print(x)
  
  #include <USB-MIDI.h>
  USBMIDI_CREATE_DEFAULT_INSTANCE();  
  //MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDICoreSerial);  
  
#else // USB <-> UART <-> MIDI
  #define LED_BLINK LED_BUILTIN
  
  #define DEBUG_INIT(x) // not available
  #define DEBUG(x)
  
  #include <MIDI.h>

  struct CustomBaud : public midi::DefaultSettings{
      static const long BaudRate = BAUD_RATE; // e.g. Hairless MIDI to Serial Bridge
  };
  MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, CustomBaud);
#endif

MCP4725 PITCH_OUT(PITCH_ADDR), MOD_OUT(MOD_ADDR);
static bool has_pitch = false, has_mod = false;
static float max_voltage = 5.13; // manually calibrated
static float pitch_voltage = 0.0;

// -----------------------------------------------------------------------------

void handleNoteOn(byte channel, byte key, byte velocity)
{
    // Do whatever you want when a note is pressed.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.

    if (velocity)
    {
      float voltage = 0.0;
      
      if (has_pitch)
      {
        voltage = (float)(key - BASE_KEY) * (1.0/12.0); // 1V per octave
        if ((voltage < 0.0) || (voltage > max_voltage))
          return; // ignore key outside range
        pitch_voltage = voltage;
        PITCH_OUT.setVoltage(voltage);
      }
/*
      DEBUG("NoteOn: ");
      DEBUG(key);
      DEBUG(" @ ");
      DEBUG(velocity);
      if (has_pitch)
      {
        DEBUG(" : ");
        DEBUG(voltage);
        DEBUG(" V");
      }
      DEBUG("\n"); 
*/
      digitalWrite(LED_BLINK, HIGH);
      digitalWrite(GATE_OUT, HIGH);
    }
    else
    {
      digitalWrite(LED_BLINK, LOW);
      digitalWrite(GATE_OUT, LOW);
    }
}

void handleNoteOff(byte channel, byte key, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
    
    digitalWrite(LED_BLINK, LOW);
    digitalWrite(GATE_OUT, LOW);
/*  
    DEBUG("NoteOff: ");
    DEBUG(key);
    DEBUG(" @ ");
    DEBUG(velocity);
    DEBUG("\n");
*/
}

void handlePitchBend(byte channel, int bend)
{
  float voltage = pitch_voltage;
  voltage += (1.0*bend) / MIDI_PITCHBEND_MAX; // max. +-1V
  PITCH_OUT.setVoltage(voltage);
/*
    DEBUG("PitchBend: ");
    DEBUG(bend);
    DEBUG(" : ");
    DEBUG(voltage);
    DEBUG(" V");
    DEBUG("\n");
*/
}

void handleControlChange(byte channel, byte number, byte value)
{
  if ((number == 1) && (has_mod))
  {
    float voltage =  (max_voltage*value) / 127;
    MOD_OUT.setVoltage(voltage);
  }
/*
    DEBUG("ControlChange: ");
    DEBUG(number);
    DEBUG(" : ");
    DEBUG(value);
    DEBUG("\n");
*/
}

// -----------------------------------------------------------------------------

void setup()
{
    DEBUG_INIT(BAUD_RATE);
    DEBUG("Midi_CV\n");
    
    pinMode(LED_BLINK, OUTPUT);
    digitalWrite(LED_BLINK, LOW);

    pinMode(GATE_OUT, OUTPUT);
    digitalWrite(GATE_OUT, LOW);

    if (PITCH_OUT.begin())
    {
      DEBUG("Connected to PITCH_OUT DAC\n");
      has_pitch = true;
      PITCH_OUT.setMaxVoltage(max_voltage);
      PITCH_OUT.setVoltage(0.0);
    }
    else
    {
      DEBUG("Could not find PITCH_OUT DAC\n");
    }
    
    if (MOD_OUT.begin())
    {
      DEBUG("Connected to MOD_OUT DAC\n");
      has_mod = true;
      MOD_OUT.setMaxVoltage(max_voltage);
      MOD_OUT.setVoltage(0.0);
    }
    else
    {
      DEBUG("Could not find MOD_OUT DAC\n");
    }
    
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandlePitchBend(handlePitchBend);
    MIDI.setHandleControlChange(handleControlChange);
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.turnThruOn();
    
    DEBUG("Ready for MIDI notes\n");
}

void loop()
{
    MIDI.read();
}

// -----------------------------------------------------------------------------
