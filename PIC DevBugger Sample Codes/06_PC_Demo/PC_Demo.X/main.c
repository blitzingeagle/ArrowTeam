/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 *
 * Created on July 10, 2017, 10:27 AM
 * 
 * Description: Demonstration of PIC-PC communication. This program sends key
 *              press data to a PC connected to the co-processor, while also
 *              displaying data received from the PC to the character LCD.
 * 
 * Preconditions:
 *   1. Co-processor running the virtual COM port program
 *   2. The character LCD is in a PIC socket
 *   3. JP_TX and JP_RX in the USB module are shorted
 *   4. USB port in co-processor module is plugged into a PC
 *   5. Co-processor has enumerated correctly to host PC (i.e. is recognized
        as a virtual COM port by the host PC)
 */

/***** Constants *****/
#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "UART_PIC.h"

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
    
    // <editor-fold defaultstate="collapsed" desc="Initialize EUSART module for asynchronous 9600/8N1">
    /* Configure the baud rate generator for 9600 bits per second. */
    long baudRate = 9600;
    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baudRate)) - 1);
    
    /* Configure transmit control register */
    TXSTAbits.TX9 = 0; // Use 8-bit transmission (8 data bits, no parity bit)
    TXSTAbits.SYNC = 0; // Asynchronous communication
    TXSTAbits.TXEN = 1; // Enable transmitter
    __delay_ms(5); // Enabling the transmitter requires a few CPU cycles for stability
    
    /* Configure receive control register */
    RCSTAbits.RX9 = 0; // Use 8-bit reception (8 data bits, no parity bit)
    RCSTAbits.CREN = 1; // Enable receiver
    
    /* Enforce correct pin configuration for relevant TRISCx */
    TRISC6 = 0; // TX = output
    TRISC7 = 1; // RX = input
    
    /* Enable serial peripheral */
    RCSTAbits.SPEN = 1;
    // </editor-fold>

    /* Declare local variables. */
    unsigned char receivedByte; // For receiving characters from the PC
    
    while(1){
        /* RB1 is the interrupt pin, so if there is no key pressed, RB1 will be
         * 0. RCIF is the interrupt flag for UART reception. Even without UART
         * receive interrupts enabled, this flag will still be set when a byte
         * is received and the UART module is running. The line below forces the
         * PIC to do nothing until either a key has been pressed, or information
         * has been received from the PC (i.e. there is a UART reception). */
        while(!(PORTBbits.RB1 || RCIF)){   continue;   }

        /*************** CASE 1: CHARACTER RECEIVED FROM PC *******************/
        /* Check we exited the while loop because of a UART reception. */
        if(RCIF){
            /* Byte received via UART. First, read the receive register, RCREG, 
             * into memory. */
            receivedByte = RCREG;

            /* Check/clear overrun error (next byte arrives before previous
             * was read). This error sometimes arises for the first reception,
             * but usually doesn't happen otherwise. */
            if(RCSTAbits.OERR){
                /* Receive buffer flushed in hardware when receiver is reset */
                RCSTAbits.CREN = 0;
                RCSTAbits.CREN = 1;
            }
            putch(receivedByte); // Print the character received to the character LCD
            continue; // Go back to the main loop so that we don't poll the keypad
        }
        
        /**************** CASE 2: KEY PRESS DETECTED ON KEYPAD ****************/
        /* If we did not receive a byte via the UART, then we exited the while
         * loop due to a key press. In this case, read the 4-bit character 
         * code. */
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        
        /* Wait until the key has been released. */
        while(PORTBbits.RB1 == 1){  continue;   }
        
        /* Wait until the previous TXREG data has completed its transfer 
         * into the TSR and the TSR has finished transmitting all bits 
         * before loading the TXREG register. */
        while(!TXIF | !TRMT){   continue;   }
        
        /* Load data into the transmit register, TXREG. */
        TXREG =  keys[keypress];
    }
}
