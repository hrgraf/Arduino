/**
 * Arduino MIDI AN1x SysEx-To-CC Translator by H.R.Graf
 *
 * Works out-of-the-box on Arduino Leonardo compatible boards.
 * No MIDI DIN circuit needed, as MIDI events are sent/received over USB.
 * Translates AN1x specific SysEx to Midi Control change.
 * Passing thru standard MIDI messages.
 * Received Note On/Off control built-in LED.
 * The serial interface is used for monitor / debug messages.
 * No additional drivers needed on the host (PC) side.
 */

#include <Arduino.h> // for Intellisense

#define LED_BLINK LED_BUILTIN // 13
//#define LED_BLINK 17 // RX LED on Pro Micro

#define CH_IN  MIDI_CHANNEL_OMNI
#define CH_OUT 2

#define BAUD_RATE 115200

#if 1
  #define DEBUG(x) Serial.print(x)
  #define DEBUG2(x,y) Serial.print(x,y)
#else
  #define DEBUG(x)
  #define DEBUG2(x,y)
#endif

#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();  

using namespace MIDI_NAMESPACE;

// -----------------------------------------------------------------------------

static void OnNoteOn(byte channel, byte key, byte velocity) 
{
  digitalWrite(LED_BLINK, velocity ? HIGH : LOW);
  MIDI.sendNoteOn(key, velocity, CH_OUT); 
  
  DEBUG("NoteOn ");
  DEBUG(key);
  DEBUG(": ");
  DEBUG(velocity);
  DEBUG("\n");
}

static void OnNoteOff(byte channel, byte key, byte velocity) 
{
  digitalWrite(LED_BLINK, LOW);
  MIDI.sendNoteOff(key, velocity, CH_OUT);
  
  DEBUG("NoteOff ");
  DEBUG(key);
  DEBUG(": ");
  DEBUG(velocity);
  DEBUG("\n");
}

static void OnAfterTouchChannel(byte channel, byte pressure) 
{
  MIDI.sendAfterTouch(pressure, CH_OUT);
  
  DEBUG("AfterTouch: ");
  DEBUG(pressure);
  DEBUG("\n");
}

static void OnPitchBend(byte channel, int bend) 
{
  MIDI.sendPitchBend(bend, CH_OUT);

  DEBUG("PitchBend: ");
  DEBUG(bend);
  DEBUG("\n");
}

static void OnControlChange(byte channel, byte number, byte value) 
{
  MIDI.sendControlChange(number, value, CH_OUT);
  
  DEBUG("ControlChange ");
  DEBUG(number);
  DEBUG(": ");
  DEBUG(value);
  DEBUG("\n");
}

static void OnSystemExclusive(byte *data, unsigned length) {
  DEBUG("SYSEX: (");
  DEBUG(length);
  DEBUG(" bytes) ");
  for (uint16_t i = 0; i < length; i++)
  {
    DEBUG2(data[i]>>4, HEX);
    DEBUG2(data[i]&0xF, HEX);
    DEBUG(" ");
  }
  DEBUG("\n");
}

static void OnProgramChange(byte channel, byte number) 
{
  MIDI.sendProgramChange(number, CH_OUT);

  DEBUG("ProgramChange: ");
  DEBUG(number);
  DEBUG("\n");
}

static void OnSystemReset() 
{
  digitalWrite(LED_BLINK, LOW);
  MIDI.sendSystemReset();
    
  DEBUG("SystemReset");
}


// -----------------------------------------------------------------------------

void setup()
{
  Serial.begin(BAUD_RATE); 
  while (!Serial) 
    ;
  Serial.print("MIDI AN1x SysEx-To-CC Translator\n");

  pinMode(LED_BLINK, OUTPUT);
  digitalWrite(LED_BLINK, LOW);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleAfterTouchChannel(OnAfterTouchChannel);
  MIDI.setHandlePitchBend(OnPitchBend);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandleSystemExclusive(OnSystemExclusive);
  MIDI.setHandleProgramChange(OnProgramChange);
  MIDI.setHandleSystemReset(OnSystemReset);

  MIDI.begin(CH_IN);

  if (CH_IN == MIDI_CHANNEL_OMNI)
  {
    Serial.println("Listening on all channels");
  }
  else
  {
    Serial.print("Listening on channel ");
    Serial.println(CH_IN);
  }

  Serial.print("Output on channel ");
  Serial.println(CH_OUT);
}

// -----------------------------------------------------------------------------

void loop()
{
  MIDI.read();
}

// -----------------------------------------------------------------------------
