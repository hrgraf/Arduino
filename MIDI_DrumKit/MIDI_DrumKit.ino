/**
 * Arduino MIDI DrumKit by H.R.Graf
 *
 * Piezo to MIDI
 *
 * MIDI out either direct at 31250 baud over the serial interface, 
 * or over USB as virtual COM port at 115200 baud.
 * 
 * On the host (PC), the MIDI communication over USB is accessible 
 * as virtual COM port. There, some additional SW is required to translate 
 * between UART and MIDI. The baud rate of 115200 is compatible to both
 * - Hairless MIDI to Serial Bridge
 * - my pizmidi/midiUartBridge (recommended!)
 *
 * For debug purposes, define NO_MIDI, to have debug messages instead of MIDI output.
 */

#include <Arduino.h> // for Intellisense

//#define NO_MIDI // comment out for MIDI Output
//#define DEBUG_PIEZO // comment out for piezo data logger

#define MIDI_CHANNEL  0x09 // GM Percussion on channel 10
#define MIDI_NOTEON   0x90

// MT Power Drum Kit (VST DrumKit)
#define MIDI_KICK       36
#define MIDI_SNARE      38
#define MIDI_TOM_LOW    41
#define MIDI_TOM_MID    45
#define MIDI_TOM_HI     48
#define MIDI_HI_HAT     46

#ifdef NO_MIDI // Debug
  #define BAUD_RATE 115200
  #define DEBUG(x) Serial.print(x)
  #define SEND_MIDI(x)
  #define BUF_LEN  500
#else 
//#define BAUD_RATE  31250 // MIDI HW Output
  #define BAUD_RATE 115200 // USB <-> UART <-> MIDI
  #define DEBUG(x)
  #define SEND_MIDI(x) Serial.write(x)
#endif

#define THRESHOLD 16

typedef struct
{
    int8_t   pin; // Arduino pin
    uint8_t  key; // MIDI key
    int16_t  max; // 10-bit ADC value
    uint32_t t_on, t_off; // in us
} piezo_t;

// piezo configuration
static piezo_t piezo[] =
{
    { A0, MIDI_HI_HAT,  0, 0, 0 },
    { A1, MIDI_SNARE,   0, 0, 0 },
    { A2, MIDI_TOM_MID, 0, 0, 0 },
    { A3, MIDI_TOM_HI,  0, 0, 0 },
    { A4, MIDI_KICK,    0, 0, 0 },
    { -1, 0,            0, 0, 0 } // terminator
};

// -----------------------------------------------------------------------------

void sendNoteOn(byte key, byte velocity)
{
    digitalWrite(LED_BUILTIN, velocity ? HIGH : LOW);

    SEND_MIDI(MIDI_NOTEON | MIDI_CHANNEL);
    SEND_MIDI(key      & 0x7F);
    SEND_MIDI(velocity & 0x7F);

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

// -----------------------------------------------------------------------------
// Debug loop

#ifdef DEBUG_PIEZO

void loop()
{
    static int buf[BUF_LEN];

    int val = analogRead(A0);
    unsigned long now = micros();
    //DEBUG(val);
    //DEBUG("\n");

    if (val >= THRESHOLD)
    {
        unsigned long start = now;
        buf[0] = val;
        for (int i=1; i<BUF_LEN; i++)
        {
            now += 100; // step size in us
            while (micros() < now)
                ;

            buf[i] = analogRead(A0);

        }

        DEBUG("Triggered:\n");
        int pos = 0, max = 0, sum = 0, len = 0;
        for (int i=0; i<BUF_LEN; i++)
        {
            DEBUG(i+1);
            DEBUG("\t");
            DEBUG(buf[i]);
            DEBUG("\n");

            if (buf[i] >= THRESHOLD)
                len = i;
            sum += buf[i];
            if (max < buf[i])
            {
                max = buf[i];
                pos = i;
            }
        }
        unsigned long step_us = (now-start)/(BUF_LEN-1);

        DEBUG("Recorded ");
        DEBUG((500+now-start)/1000);
        DEBUG("ms with steps of ");
        DEBUG(step_us);
        DEBUG("us. ");

        DEBUG("Energy of ");
        DEBUG(sum);
        DEBUG(" with length ");
        DEBUG(len);
        DEBUG(" (");
        DEBUG((len*step_us+500)/1000);
        DEBUG("ms) and peak of ");
        DEBUG(max);
        DEBUG(" (vs. ");
        DEBUG(buf[0]);
        DEBUG(") at ");
        DEBUG(pos);
        DEBUG(" (");
        DEBUG(pos*step_us);
        DEBUG("us)\n");
    }
}

#else

// -----------------------------------------------------------------------------
// Main loop

void loop()
{
    piezo_t *p = piezo;

    while (p->pin >= 0)
    {
        int16_t  val = analogRead(p->pin);
        uint32_t now = micros();

        if (p->t_on) // started
        {
            if (p->max < val)
                p->max = val;

            if (now >= p->t_on)
            {
//              DEBUG("Hit: ");
//              DEBUG(p->max);
//              DEBUG("\n");

                // map to 1..127 with optional compression/offset
                //val = p->max / 8;
                //val = (p->max - THRESHOLD) / 4 + 32;
                val = p->max / 2;
                if (val > 127)
                    val = 127;
                if (val < 1)
                    val = 1;
                sendNoteOn(p->key, val);
                p->t_on = 0;
            }
        }
        else if (p->t_off) // not stopped yet
        {
            if (now >= p->t_off)
            {
                sendNoteOn(p->key, 0); // off
                p->t_off = 0;
            }
        }
        else // no activity
        {
            if (val >= THRESHOLD)
            {
                p->max   = val;
                p->t_on  = now +  1000;
                p->t_off = now + 50000;
            }
        }

        p++; // next piezo
    }
}

#endif

// -----------------------------------------------------------------------------
