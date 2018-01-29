/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 *
 * Created on July 31, 2017, 12:56 PM
 * 
 * Description: Iterates through the pins on the PIC, making the debug LEDs
 *              toggle sequentially.
 * 
 * Preconditions:
 *   1. No external circuitry driving the data lines
 *   2. LED debug switches are all enabled
 *   3. Debug DIP switches are all in the middle position
 *   4. Arduino-PIC link switches are all OFF
 *   5. JP_TX and JP_RX in the USB module are open (i.e. not shorted)
 *   6. The A2D jumpers are open (i.e. not connected to the PIC)
 *   7. The PRG/RUN switch must be in the PRG position (this is assumed for ALL
 *      sample code)
 * 
 * Note: RB1 is held low by the co-processor when it runs the default keypad
 *       encoder program. To see it toggle, simply boot the co-processor into
 *       the UART keypad encoder by holding key 1 on the keypad while turning
 *       on power, or use a jumper to short KPD on the I/O bus to VCC.
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h" // For _XTAL_FREQ, required in delay macros

/***** Defines *****/
#define DELAY_TIME  250

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
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Main loop. This demonstrates writing to the latches for each I/O pin on 
     * the IC18F4620. Note that RE2, RA6, and RA7 do not have LEDs in the debug
     * module. To view the latches for these pins being set and cleared, the
     * I/O bus can be used to connect external LEDs with series resistors to
     * ground. */
    while(1){
        /* Initialize local variables. */
        unsigned char i = 0;
        unsigned char temp = 0x00;

        temp = 1 << 7; // Set temporary byte to 0b10000000.
        /* Write temp to the latch, then bit shift to right. Iterate over the port
         * size. */
        for (i = 0; i < 8; i++){
            LATA = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATA = 0x00; // Clear latch outputs after demonstrating the port.

        /* Same process for all other ports. */
        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATB = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATB = 0x00;

        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATC = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATC = 0x00;

        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATD = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATD = 0x00;

        /* Note: Look in the SD card module to see the LED for RE2. */
        temp = 1 << 2;
        for (i = 0; i < 3; i++){
            LATE = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATE = 0x00;
    }
}
