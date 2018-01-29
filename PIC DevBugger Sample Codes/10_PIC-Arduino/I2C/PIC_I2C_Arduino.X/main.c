/*
 * File:   main.c
 * Author: Michael Ding
 *
 * Created on July 18, 2016, 12:11 PM
 * 
 * Edited by Tyler Gamvrelis, summer 2017
 * 
 * Description: Demonstration of communication between the primary PIC and the
 *              Arduino Nano, via I2C. To see the results on the Arduino Nano
 *              side, open the Arduino sample program Arduino_I2C_PIC. If the
 *              PIC receives a triple-A sequence from the keypad, it changes
 *              from being a transmitter to being a receiver that displays data
 *              on the character LCD. To change it back, reset the PIC.
 * 
 * Preconditions:
 *   1. PIC-Arduino link switches are enabled (ON) for A4 and A5 of the Arduino
 *      Nano (RC4 and RC3 for the primary PIC, respectively)
 *   2. PIC and Arduino Nano agree on the Arduino Nano's slave address (i.e. the
 *      same slave address is used in software)
 *   3. A PC is connected if the Arduino Nano serial monitor is to be used
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "I2C.h"

/***** Constants *****/
const char keys[] = "123A456B789C*0#D";

void main(void) {

    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0xFF; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0xFF;
    TRISC = 0b10000000; /* RC3 is SCK/SCL (SPI/I2C),
                         * RC4 is SDA (I2C),
                         * RC5 is SDA (SPI),
                         * RC6 and RC7 are UART TX and RX, respectively. */
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Initialize LCD. */
    initLCD();  

    /* Initialize I2C Master with 100 kHz clock. */
    I2C_Master_Init(100000); 
    I2C_Master_Start(); // Start condition
    
    /* Write the address of the slave device, that is, the Arduino Nano. Note
     * that the Arduino Nano must be configured to be running as a slave with
     * the same address given here. Note that other addresses can be used if
     * desired, as long as the change is reflected on both the PIC and Arduino
     * ends. */
    I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
    I2C_Master_Stop();
    
    /* Initialize local variables. */
    unsigned char mem[3]; // Initialize array to check for triple-A sequence
    unsigned char counter = 0; // Increments each time a byte is sent
    unsigned char keypress; // Stores the data corresponding to the last key press
    unsigned char data; // Holds the data to be sent/received
    static bit SEND;
    SEND = 1;
    
    /* Main loop. */
    while(1) {
        if(SEND){
            while (PORTBbits.RB1 == 0) {    continue;   }
            keypress = (PORTB & 0xF0) >> 4;
            while (PORTBbits.RB1 == 1) {    continue;   }
            data = keys[keypress];
            
            I2C_Master_Start(); // Start condition
            I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
            I2C_Master_Write(data); // Write key press data
            I2C_Master_Stop();

            /* Check for a triple-A sequence. If this sequence occurs, switch 
             * the PIC to receiver mode. To switch back to transmitter mode,
             * reset the PIC. */
            mem[counter] = data;
            counter++;
            counter = (counter == 3) ? 0 : counter;

            if(mem[0]=='A' && mem[1]=='A' && mem[2]=='A'){
                SEND = 0;
            }
        }
        else{
            /* Receive data from Arduino and display it on the LCD. */
            I2C_Master_Start();
            I2C_Master_Write(0b00010001); // 7-bit Arduino slave address + Read
            data = I2C_Master_Read(NACK); // Read one char only
            I2C_Master_Stop();
            if(data){
                putch(data);
            }
        }
    }
}
