/**
 * Processing example controlling Arduino by serial communication.
 * 
 * Check if the mouse is over a rectangle and writes the status to the serial port. 
 */

import processing.serial.*;

Serial myPort;  // Create object from Serial class

void setup() 
{
  size(200, 200);
  // Open whatever port is the one you're using.
  String portName = "/dev/ttyACM0"; //Serial.list()[0];
  myPort = new Serial(this, portName, 115200);
}

void draw() 
{
  background(255);
  if (mouseOverRect() == true) 
  {
    fill(204);                      // change color and
    myPort.write('1');              // send an H to indicate mouse is over square
  } 
  else 
  {
    fill(0);                      // change color and
    myPort.write('0');              // send an L otherwise
  }
  
  rect(50, 50, 100, 100);         // Draw a square
}

boolean mouseOverRect() 
{ // Test if mouse is over square
  return ((mouseX >= 50) && (mouseX <= 150) && (mouseY >= 50) && (mouseY <= 150));
}
