#!/usr/bin/python3

import serial
import time

print("Control Arduino LED over serial communication")
print("Press Ctrl-C to abort")
print()

# adjust the port
portName = '/dev/ttyACM0'
portName = 'COM6'

ser = serial.Serial(portName, 115200, timeout=0)

outgoing = '0'
while True:
    try:
        incoming = ser.read(80)
        if incoming:
            print("Incoming:", incoming.decode())

        ser.write(outgoing.encode())
        if outgoing == '0':
            outgoing = '1'
        else:
            outgoing = '0'
            
        time.sleep(0.5)

    except KeyboardInterrupt:
        break
        
