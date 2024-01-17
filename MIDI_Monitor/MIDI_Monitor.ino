/**
 * Arduino MIDI Monitor by H.R.Graf
 *
 * Works out-of-the-box on Arduino Leonardo compatible boards.
 * No MIDI DIN circuit needed, as MIDI events are sent/received over USB.
 * Received Note On/Off control built-in LED.
 * The serial interface is used for monitor / debug messages.
 * No additional drivers needed on the host (PC) side.
 */

#include <Arduino.h> // for Intellisense

#define BAUD_RATE 115200

#define LED_BLINK LED_BUILTIN // 13
//#define LED_BLINK 17 // RX LED on Pro Micro
  
#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();  

using namespace MIDI_NAMESPACE;

// -----------------------------------------------------------------------------

static void OnNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BLINK, velocity ? HIGH : LOW);
  
  Serial.print(F("NoteOn  from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.println(velocity);
}

static void OnNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(LED_BLINK, LOW);
  
  Serial.print(F("NoteOff from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.println(velocity);
}

static void OnAfterTouchPoly(byte channel, byte note, byte pressure) {
  Serial.print(F("AfterTouchPoly from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", pressure: "));
  Serial.println(pressure);
}

static void OnControlChange(byte channel, byte number, byte value) {
  Serial.print(F("ControlChange from channel: "));
  Serial.print(channel);
  Serial.print(F(", number: "));
  Serial.print(number);
  Serial.print(F(", value: "));
  Serial.println(value);
}

static void OnProgramChange(byte channel, byte number) {
  Serial.print(F("ProgramChange from channel: "));
  Serial.print(channel);
  Serial.print(F(", number: "));
  Serial.println(number);
}

static void OnAfterTouchChannel(byte channel, byte pressure) {
  Serial.print(F("AfterTouchChannel from channel: "));
  Serial.print(channel);
  Serial.print(F(", pressure: "));
  Serial.println(pressure);
}

static void OnPitchBend(byte channel, int bend) {
  Serial.print(F("PitchBend from channel: "));
  Serial.print(channel);
  Serial.print(F(", bend: "));
  Serial.println(bend);
}

static void OnSystemExclusive(byte *data, unsigned length) {
  Serial.print(F("SYSEX: ("));
  Serial.print(length);
  Serial.print(F(" bytes) "));
  for (uint16_t i = 0; i < length; i++)
  {
    Serial.print(data[i]>>4, HEX);
    Serial.print(data[i]&0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

static void OnTimeCodeQuarterFrame(byte data) {
  Serial.print(F("TimeCodeQuarterFrame: "));
  Serial.println(data, HEX);
}

static void OnSongPosition(unsigned beats) {
  Serial.print(F("SongPosition: "));
  Serial.println(beats);
}

static void OnSongSelect(byte songnumber) {
  Serial.print(F("SongSelect: "));
  Serial.println(songnumber);
}

static void OnTuneRequest() {
  Serial.println(F("TuneRequest"));
}

static void OnClock() {
  Serial.println(F("Clock"));
}

static void OnStart() {
  Serial.println(F("Start"));
}

static void OnContinue() {
  Serial.println(F("Continue"));
}

static void OnStop() {
  Serial.println(F("Stop"));
}

static void OnActiveSensing() {
  Serial.println(F("ActiveSensing"));
}

static void OnSystemReset() {
  digitalWrite(LED_BLINK, LOW);
  Serial.println(F("SystemReset"));
}

// -----------------------------------------------------------------------------

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial) ;
  Serial.println("MIDI Monitor");
    
  pinMode(LED_BLINK, OUTPUT);
  digitalWrite(LED_BLINK, LOW);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleAfterTouchPoly(OnAfterTouchPoly);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandleProgramChange(OnProgramChange);
  MIDI.setHandleAfterTouchChannel(OnAfterTouchChannel);
  MIDI.setHandlePitchBend(OnPitchBend);
  MIDI.setHandleSystemExclusive(OnSystemExclusive);
  MIDI.setHandleTimeCodeQuarterFrame(OnTimeCodeQuarterFrame);
  MIDI.setHandleSongPosition(OnSongPosition);
  MIDI.setHandleSongSelect(OnSongSelect);
  MIDI.setHandleTuneRequest(OnTuneRequest);
  MIDI.setHandleClock(OnClock);
  MIDI.setHandleStart(OnStart);
  MIDI.setHandleContinue(OnContinue);
  MIDI.setHandleStop(OnStop);
//MIDI.setHandleActiveSensing(OnActiveSensing);
  MIDI.setHandleSystemReset(OnSystemReset);

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}

// -----------------------------------------------------------------------------

void loop()
{
  MIDI.read();
}

// -----------------------------------------------------------------------------
