# Arduino MIDI Test

Works out-of-the-box on Arduino Uno / Leonardo compatible boards.
No MIDI DIN circuit needed, as MIDI events are sent/received over USB.

Received Note On/Off control built-in LED and are sent back on channel 2
with slight change in pitch (to demonstrate active functionality).

On the Arduino Uno, the serial interface is used to send MIDI over USB
at 115200 baud. On the host (PC), the USB communication is accessible 
as virtual COM port. There, some additional SW is required to translate 
between UART and MIDI. The baud rate of 115200 is compatible to both
  - Hairless MIDI to Serial Bridge
  - [my pizmidi/midiUartBridge (recommended!)](https://github.com/hrgraf/pizmidi)

On the Arduino Leonardo, the USB MIDI functionality is built-in and the 
serial interface is available for serial monitor / debug messages.
No additional drivers needed on the host (PC) side.
