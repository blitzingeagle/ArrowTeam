/*
 * File:   main.c
 * Author: Morris Chen
 * 
 * Created on February 22, 2018, 6:00 PM
 * 
 * Preconditions:
 *   1. GLCD is in a PIC socket
 *   2. LCD is in a PIC socket
 *   3. Keypad is in a PIC socket
 */

/***** Includes *****/
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "config_bits.h"
#include "lcd.h"
#include "I2C.h"
#include "GLCD_PIC.h"
#include "SPI_PIC.h"
#include "px_ascii.h"
#include "protocol_manager.h"
#include "program_states.h"
#include "RTC.h"
#include "UART_PIC.h"
#include "arduino_cmd.h"

/***** Macros *****/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

/***** Function Prototypes *****/
void RTC_setTime(void);

/***** Constants *****/
const char keys[] = "123B456N789S*0#W";

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
    
    /* Initialize LCD. */
    initLCD();
    /* Initialize GLCD. */
    initGLCD();
    
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, RED);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 14, 110, BLACK);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
    
    /* Initialize I2C */
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    
    /* Initialize UART */
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    UART_Init(9600); // Baud rate 9600
    
    /* Initialize Program States */
    init_program_states();
    
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    ei(); // Enable all interrupts
}

void main(void) {
    init();
    
    FUNC_STATE_STANDBY(); // Start program in standby mode
    
    /* Declare local variables. */
    unsigned char time[7];
    
    /* Main loop. */
    while(1) {
        RTC_read_time(time);
        
        use_protocol(SPI);
        glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, RED);
        
        char buffer[22];
        sprintf(buffer, "%02x/%02x/%02x     %02x:%02x:%02x", time[5], time[4], time[6], time[2], time[1], time[0]);
        print_px_string(1, 1, buffer);
        
        use_protocol(I2C);
        
        arduino_send_fastener_data();
        
        __delay_ms(1000);
    }
}

void interrupt interruptHandler(void) {
    if(INT1IF) { /* Interrupt on change handler for RB1. */
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        unsigned char key = keys[keypress];
        
        program_states_interrupt(key);
        
        INT1IF = 0;  // Clear interrupt flag bit to signify it's been handled
        return;
    }
    
    /* Handle UART interrupt */
    UART_interrupt();
}

// TODO: Interface functions for UART_interrupt
void tmp(void (*TX_interface)(void), void (*RX_interface)(void)) {
    if(TX_interface) TX_interface();
}

