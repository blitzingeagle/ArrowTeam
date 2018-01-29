/* PortTest
 *  
 * Author: Tyler Gamvrelis
 * Date: August 1, 2017 9:51 AM
 * Description: Sample program that iterates through the I/O on the Arduino Nano.
 */

/* Declare global variables. */
static uint8_t i;

void setup() {
  /* Configure each pin iteratively. */
  for(i = 0; i < 20; i++){
    pinMode(i, OUTPUT);   // Configure the data direction for the I/O pins to output
    digitalWrite(i, LOW); // Clear all outputs (i.e. assert low logic level)
  }
}

void loop() {
  /* Toggle each pin state for 1 second, one pin at a time, forever. */
  for(i = 0; i < 20; i++){
    digitalWrite(i, HIGH);
    delay(1000); // Argument in milliseconds
    digitalWrite(i, LOW);
  }
}

