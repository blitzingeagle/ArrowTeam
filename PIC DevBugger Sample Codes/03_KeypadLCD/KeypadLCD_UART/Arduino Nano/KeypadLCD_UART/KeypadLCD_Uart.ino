/* KeypadLCD_UART
 *  
 * Author: Tyler Gamvrelis
 * Date: August 1, 2017 12:02 PM
 * Description: Sample program that receives key press data
 *              via the hardware UART and displays it on
 *              the character LCD
 * Preconditions:
 *  1.  Keypad encoder is in UART mode
 *  2.  PIC-Arduino link switches are enabled (ON) for D0 of the Arduino (the RX pin).
 *      However, make sure that neither of D0 and D1 are enabled (ON) while programming the Arduino Nano
 *  3.  The main PIC is not driving its RC6 and RC7 lines (i.e. they are in the tristate/input mode 
 *      or the main PIC is removed altogether)
 *  4.  Character LCD is in an Arduino Nano socket
 */

/* Includes. */
#include <LiquidCrystal.h>

/* Initialize global variables. */
static uint8_t keyData = 0;
const char keys[] = "123A456B789C*0#D";
const int rs = 4, en = 5, d4 = 6, d5 = 7, d6 = 8, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  /* Initialize LCD. */
  lcd.begin(16, 2);
  
  /* Open serial port with baud rate = 9600. */
  Serial.begin(9600, SERIAL_8N1);
}

void loop() {
  /* Wait to receive the message from the Co-Processor. */
  while(Serial.available() < 1){ continue; }

  /* Read key press data into a byte. */
  keyData = Serial.read();

  /* Wait for key to be released. */
  while(1){
    /* Wait to receive data from Co-processor. */
    if(Serial.available() < 0){ continue; }

    /* If data was received, compare it to the end
     * of data byte expected (0xF0). */
    else if(Serial.read() == 0xF0){ break;  }
  }

  /* Write received byte to the LCD using ASCII encoding. */
  lcd.print(keys[keyData]);
}
