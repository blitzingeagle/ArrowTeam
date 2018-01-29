/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on August 29, 2017, 4:51 PM
 * 
 * Description: Sample program for arbitrary information exchange between
 *              the main PIC and the Arduino Nano. The character 'H' is sent to
 *              the Arduino Nano via UART. To see the results on the Arduino
 *              Nano side, use the Arduino sample program Arduino_UART_PIC.
 * 
 * Preconditions:
 *   1.  Main PIC uses hardware UART
 *   2.  Arduino Nano uses software UART
 *   3.  The Co-processor is not driving lines on the UART bus (e.g. the JP_TX and JP_RX jumpers are removed)
 *   4.  The character LCD is in an Arduino Nano socket
 *   5.  PIC-Arduino link switches are enabled (ON) for D0 of the Arduino (the RX pin).
 *       However, make sure that neither of D0 and D1 are enabled (ON) while programming the Arduino Nano
 */


#include <xc.h>
#include "machineConfig.h"
#include "configBits.h"
#include "lcd.h"
#include "UART_PIC.h"

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
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Initialize UART. */
    UART_Init(9600);

    /* Main loop. */
    unsigned char arr[1] = {'H'};
    while(1){
        /* Send 'H'. */
        uartTransmitBlocking(arr, 1);
        __delay_ms(1000); // Wait 1 second
    }
}
