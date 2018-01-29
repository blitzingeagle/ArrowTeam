/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on August 13, 2017, 6:00 PM
 * 
 * Description: Demonstration of GLCD initialization, and the fundamental
 *              graphics "function", drawing rectangles. Drawing pixels are also
 *              demonstrated as a special case of drawing rectangles.
 * 
 * Preconditions:
 *   1. GLCD is in a PIC socket
 */

/***** Includes *****/
#include <xc.h>
#include "config_bits.h"
#include "GLCD_PIC.h"

void init(void) {
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
    TRISD = 0b00000001; /* RD0 is the GLCD chip select (tri-stated so that it's
                         * pulled up by default),
                         * RD1 is the GLCD register select,
                         * RD2 is the character LCD RS,
                         * RD3 is the character LCD enable (E),
                         * RD4-RD7 are character LCD data lines. */
    TRISE = 0b00000100; /* RE2 is the SD card chip select (tri-stated so that
                         * it's pulled up by default).
                         * Note that the upper 4 bits of TRISE are special 
                         * control registers. Do not write to them unless you 
                         * read §9.6 in the datasheet */
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Initialize GLCD. */
    initGLCD();
}

typedef unsigned long px_char;

#define PX_32 0b000000000000000000000000000000000000000000000000000000000000
#define PX_33 0b000000000000100001000010000100001000010000100000000010000000
#define PX_34 0b000000101001010010100000000000000000000000000000000000000000

void main(void) {
    init();
    
    /* Main loop. */
    while(1) {
        /* Fill screen with rainbow colors. */
        glcdDrawRectangle(0, 18, 0, GLCD_SIZE_VERT, RED);
        glcdDrawRectangle(18, 36, 0, GLCD_SIZE_VERT, ORANGE);
        glcdDrawRectangle(36, 54, 0, GLCD_SIZE_VERT, YELLOW);
        glcdDrawRectangle(54, 72, 0, GLCD_SIZE_VERT, GREEN);
        glcdDrawRectangle(72, 90, 0, GLCD_SIZE_VERT, BLUE);
        glcdDrawRectangle(90, 108, 0, GLCD_SIZE_VERT, INDIGO);
        glcdDrawRectangle(108, 128, 0, GLCD_SIZE_VERT, VIOLET);
    }
}
