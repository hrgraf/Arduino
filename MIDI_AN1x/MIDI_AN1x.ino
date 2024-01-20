/**
 * Arduino MIDI AN1x CC and SysEx Monitor by H.R.Graf
 *
 * Works out-of-the-box on Arduino Leonardo compatible boards.
 * No MIDI DIN circuit needed, as MIDI events are sent/received over USB.
 * Parses AN1x SysEx messages (e.g. real-time parameter changes)
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

  static byte x=0, z=0;

  if (number == 12)
    z = value;
  else if (number == 13)
    x = value;
    
  if ((number == 12) || (number == 13))
  {
    DEBUG("Ribbon XZ: ");
    DEBUG(x);
    DEBUG(" - ");
    DEBUG(z);
    DEBUG("\n");
  }
  else
  {
    DEBUG("ControlChange ");
    DEBUG(number);
    DEBUG(": ");
    DEBUG(value);
    DEBUG("\n");
  }
}

static void OnSystemExclusive(byte *data, unsigned length) 
{
  if ((length  >=    9         ) && 
     ( data[0] == 0xF0         ) &&  // SysEx
     ( data[1] == 0x43         ) &&  // Yamaha
     ((data[2] & 0xF0) == 0x10 ) &&  // device number in lower nibble
     ( data[3] == 0x5C         ) &&  // model number
     ( data[length-1] == 0xF7  ) )   // End
  {
    int dev = data[2] & 0x0F;
    int val = 0;
    if (length == 9)
      val = data[7];
    else if (length == 10)
      val = (data[7]<<7) | data[8];

    if ((data[4] == 0x10) && (data[5] == 0x10)) // addr high & mid
    {
      int num = data[6] & 0x7F; // addr low

      // won't implement SysEx-To-CC translation
      // -> just select AN1x Control Change Mode 2 instead
    }
    
    DEBUG("AN1x ");
    DEBUG(dev);
    DEBUG(" SysEx ");
    DEBUG(data[4]); // addr high
    DEBUG(" ");
    DEBUG(data[5]); // addr mid
    DEBUG(" ");
    DEBUG(data[6]); // addr low
    DEBUG(": ");
    DEBUG(val);
    DEBUG("\n");
  }
  else
  {
    DEBUG("SysEx: (");
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
