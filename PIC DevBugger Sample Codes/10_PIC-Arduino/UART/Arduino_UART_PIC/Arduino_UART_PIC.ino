/* Arduino_UART_PIC
 *  
 * Author: Tyler Gamvrelis
 * Date: August 29, 2017 4:51 PM
 * Description: Sample program for arbitrary information exchange between
 *              the main PIC and the Arduino Nano
 * Preconditions:
 *   1.  Main PIC uses hardware UART
 *   2.  Arduino Nano uses software UART
 *   3.  The Co-processor is not driving lines on the UART bus (e.g. the UART jumpers are removed
 *   4.  The character LCD is in an Arduino Nano socket
 *   5.  PIC-Arduino link switches are enabled (ON) for D0 of the Arduino (the RX pin).
 *       However, make sure that neither of D0 and D1 are enabled (ON) while programming the Arduino Nano
 */

/* Includes. */
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

/* Read-only variables. */
static char arr[2]; // To hold received characters
const byte rxPin = 1;
const byte txPin = 0;
const int rs = 4, en = 5, d4 = 6, d5 = 7, d6 = 8, d7 = 9;

/* Set up objects. */
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  /* Set up the LCD's number of columns and rows. */
  lcd.begin(16, 2);
  
  /* Configure pin modes for tx and rx. */
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  /* Open software serial port with baud rate = 9600. */
  mySerial.begin(9600);

  arr[1] = '\0'; // Terminate string so that it will print properly on the LCD
}

void loop() {
  /* Wait to receive the message from the main PIC. */
  while(mySerial.available() < 1){ continue; }

  /* Write received byte to the LCD using ASCII encoding.
   * The received byte can also be seen on your PC under
   * Tools -> Serial Monitor. */
  arr[0] = mySerial.read();
  lcd.print(arr);
}
