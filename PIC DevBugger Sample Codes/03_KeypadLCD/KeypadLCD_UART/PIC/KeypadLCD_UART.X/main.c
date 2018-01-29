/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on August 29, 2017, 4:29 PM
 * 
 * Description: Demonstration of PIC receiving key press data via UART bus.
 * 
 * Preconditions:
 *   1. Co-processor running UART keypad encoder (key 1)
 *   2. Character LCD is in a PIC socket
 *   3. JP_TX and JP_RX in USB module are shorted
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "UART_PIC.h"

/***** Constants *****/
const char keys[] = "123A456B789C*0#D"; 

void main(void){
    
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
    
    nRBPU = 0; /* Enable port B's pull-up resistors to show all port B pins can 
                * reach the I/O bus when the co-processor is in UART mode. */
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Initialize LCD. */
    initLCD();
    
    /* Initialize UART. */
    UART_Init(9600);

    /* Main loop. */
    while(1){
        /* Wait until a byte is received via the UART bus. */
        uartReceiveBlocking(1);
        
        /* Read key press data into a byte. */
        unsigned char keyData = UART -> _dataRX[0];
        
        /* Wait until the key has been released. */
        while(1){
            /* Wait to receive data from Co-processor. */
            uartReceiveBlocking(1);
            
            /* If data was received, compare it to the end
             * of data byte expected (0xF0). */
            if(UART -> _dataRX[0] == 0xF0){    break;   }
        }
        
        putch(keys[keyData]); // Push the character to be displayed on the LCD
    }
}
