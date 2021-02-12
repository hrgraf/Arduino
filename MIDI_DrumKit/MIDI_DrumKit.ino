/**
 * Arduino MIDI DrumKit by H.R.Graf
 *
 * Works out-of-the-box on Arduino Uno / Leonardo compatible boards.
 * No MIDI DIN circuit needed, as MIDI events are sent/received over USB.
 *
 * Piezo -> MIDI
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

#define NO_MIDI // comment out for MIDI Output

#define MIDI_CHANNEL  0x0A
#define MIDI_NOTEON   0x90

#ifdef NO_MIDI // Debug
  #define BAUD_RATE 115200
  #define DEBUG(x) Serial.print(x)
  #define SEND_MIDI(x)
#else 
//#define BAUD_RATE  31250 // MIDI DIN-5 Output
  #define BAUD_RATE 115200 // USB <-> UART <-> MIDI
  #define DEBUG(x)
  #define SEND_MIDI(x) Serial.write(x)
#endif

#define THRESHOLD 10
#define BUF_LEN  500

// -----------------------------------------------------------------------------

static void sendNoteOn(byte channel, byte key, byte velocity)
{
    digitalWrite(LED_BUILTIN, velocity ? HIGH : LOW);

    SEND_MIDI(MIDI_NOTEON | MIDI_CHANNEL);
    SEND_MIDI(key);
    SEND_MIDI(velocity);

    DEBUG("NoteOn: ");
    DEBUG(key);
    DEBUG(" @ ");
    DEBUG(velocity);
    DEBUG("\n");
}

// -----------------------------------------------------------------------------

void setup()
{
    Serial.begin(BAUD_RATE);
    DEBUG("MIDI Drum Kit\n");
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    DEBUG("Ready\n");
}

void loop()
{
    static int buf[BUF_LEN];

    int val = analogRead(A0);
    long now = micros();
    //DEBUG(val);
    //DEBUG("\n");

    if (val >= THRESHOLD)
    {
        long start = now;
        buf[0] = val;
        for (int i=1; i<BUF_LEN; i++)
        {
            now += 100; // step size in us
            while (micros() < now)
                ;

            buf[i] = analogRead(A0);

        }

        DEBUG("Triggered:\n");
        for (int i=0; i<BUF_LEN; i++)
        {
            DEBUG(i+1);
            DEBUG("\t");
            DEBUG(buf[i]);
            DEBUG("\n");
        }
        DEBUG("Recorded ");
        DEBUG((500+now-start)/1000);
        DEBUG("ms with steps of ");
        DEBUG((now-start)/(BUF_LEN-1));
        DEBUG("us\n");
    }
}

// -----------------------------------------------------------------------------
