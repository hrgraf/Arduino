/**
 * Processing example controlling Arduino by serial communication.
 * 
 * Check if the mouse is over a rectangle and writes the status to the serial port. 
 */

import processing.serial.*;

// Open whatever port is the one you're using.
String portName = 
  //Serial.list()[0];
  //"/dev/ttyACM0"; 
  "COM6";

Serial port;  // Create object from Serial class

void setup() 
{
  size(200, 200);
  port = new Serial(this, portName, 115200);
}

void draw() 
{
  background(255);
  if (mouseOverRect() == true) 
  {
    fill(204);                      // change color and
    port.write('1');              // send an H to indicate mouse is over square
  } 
  else 
  {
    fill(0);                      // change color and
    port.write('0');              // send an L otherwise
  }
  
  rect(50, 50, 100, 100);         // Draw a square
}

// Test if mouse is over square
boolean mouseOverRect() 
{ 
  return ((mouseX >= 50) && (mouseX <= 150) && (mouseY >= 50) && (mouseY <= 150));
}
