/* Arduino_I2C_PIC
 *  
 * Author: Michael Ding
 * Date: Summer 2016
 * Edited by Tyler Gamvrelis, summer 2017
 * Description: Demonstration of Arduino-PIC interfacing via I2C. In this program, the PIC outputs
 *              keypad data to the Arduino, which forwards it to a PC. If a triple-A sequence is
 *              given on the keypad, then the PIC will display data entered into the serial monitor
 *              on the character LCD. To open the serial monitor, go to Tools -> Serial Monitor.
 * Preconditions:
 *   1. PIC-Arduino link switches are enabled (ON) for A4 and A5 of the Arduino
 *      Nano (RC4 and RC3 for the primary PIC, respectively)
 *   2. PIC and Arduino Nano agree on the Arduino Nano's slave address (i.e. the
 *      same slave address is used in software)
 *   3. A PC is connected if the Arduino Nano serial monitor is to be used
 */

#include <Wire.h>

void setup(){
  Wire.begin(8); // Join I2C bus with address 8
  Wire.onReceive(receiveEvent); // Register a function to be called when this slave device receives a transmission from a master
  Wire.onRequest(requestEvent); // Register a function to be called when a master requests data from this slave device

  Serial.begin(9600); // Open serial port to PC (hardware UART)         
}

uint8_t state = 0;
uint8_t incomingByte;
void loop(){
  if (state && Serial.available() > 0 && !incomingByte) {
    incomingByte = Serial.read();
  }
}

uint8_t buf[3];
uint8_t counter = 0;

void receiveEvent(void){
  /* This function is called whenver a transmission is received from a master.
   * 
   * Arguments: none
   * 
   * Returns: none
   */
   
  uint8_t x = Wire.read(); // Receive byte
  Serial.println((char)x); // Print to serial output as char (ASCII representation)

  buf[counter++] = x;
  counter = (counter == 3) ? 0 : counter;

  if(buf[0]=='A' && buf[1]=='A' && buf[2]=='A'){
    state = 1;
  }
}

void requestEvent(void){
   /* This function is called whenver a master requests data.
   * 
   * Arguments: none
   * 
   * Returns: none
   */
   
  Wire.write(incomingByte); // Respond with message of 1 byte
  incomingByte = 0; // Clear output buffer
}
