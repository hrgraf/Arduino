# Arduino MIDI Drum Kit

Piezo to MIDI

MIDI out either direct at 31250 baud over the serial interface, 
or over USB as virtual COM port at 115200 baud.

On the host (PC), the MIDI communication over USB is accessible 
as virtual COM port. There, some additional SW is required to translate 
between UART and MIDI. The baud rate of 115200 is compatible to both
  - Hairless MIDI to Serial Bridge
  - [my pizmidi/midiUartBridge (recommended!)](https://github.com/hrgraf/pizmidi)

