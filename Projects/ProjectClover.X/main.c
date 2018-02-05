/*
 * File:   main.c
 * Author: Morris Chen
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
#include <stdbool.h>
#include <string.h>
#include "config_bits.h"
#include "GLCD_PIC.h"
#include "px_ascii.h"
#include "lcd.h"
#include "I2C.h"

/***** Constants *****/
const char keys[] = "123A456B789C*0#D"; 

enum PROG_STATES {
    STATE_STANDBY,
    STATE_PROMPT_COMPARTMENT_COUNT,
    STATE_PROMPT_FASTENER_SET,
    NUM_STATES
} program_state;

void FUNC_STATE_STANDBY(void) {
    printf("Load fasteners &");
    __lcd_newline();
    printf("Press to start.");
    __lcd_display_control(1, 1, 1);
}

void FUNC_STATE_PROMPT_COMPARTMENT_COUNT(void) {
    printf("# assembly steps");
    __lcd_newline();
    printf("(4-8) >");
}

void FUNC_STATE_PROMPT_FASTENER_SET(void) {
    printf("Fastener set");
    __lcd_newline();
    printf("Fasteners >");
}

void (*PROG_FUNC[NUM_STATES])(void);

void init_program_states() {
    program_state = STATE_STANDBY;
    PROG_FUNC[STATE_STANDBY] = FUNC_STATE_STANDBY;
    PROG_FUNC[STATE_PROMPT_COMPARTMENT_COUNT] = FUNC_STATE_PROMPT_COMPARTMENT_COUNT;
    PROG_FUNC[STATE_PROMPT_FASTENER_SET] = FUNC_STATE_PROMPT_FASTENER_SET;
}

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
    
    init_program_states();
    
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    ei(); // Enable all interrupts
}

void draw_px_char(int x, int y, px_char p) {
    uint32_t m = 0b1;
    for(int i = 0; i < 32; i++) {
        if(m & p.part[0]) glcdDrawPixel(x+i%PX_CHAR_WIDTH, y+i/PX_CHAR_WIDTH, BLACK);
        if(m & p.part[1]) glcdDrawPixel(x+(i+32)%PX_CHAR_WIDTH, y+(i+32)/PX_CHAR_WIDTH, BLACK);

        m <<= 1;
    }
}

void main(void) {
    init();
    
    FUNC_STATE_STANDBY();
    
    /* Fill screen with rainbow colors. */
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, RED);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 14, 110, BLACK);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
    
    print_px_string(0, 0, "Jan 28, 2018  11:50 PM");
    
    char eta[] = "ETA:     - min -- sec\0";
    
    while(1) {
        
    }
    
    return;
}

void interrupt interruptHandler(void) {
    if(INT1IF) {
        /* Interrupt on change handler for RB1. */
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        bool state_changed = true;
        
        switch(program_state) {
            case STATE_STANDBY:
                program_state = STATE_PROMPT_COMPARTMENT_COUNT;
                break;
            case STATE_PROMPT_COMPARTMENT_COUNT:
                if('4' <= keys[keypress] && keys[keypress] <= '8') {
                    program_state = STATE_PROMPT_FASTENER_SET;
                }
                
                break;
            case STATE_PROMPT_FASTENER_SET:
                putch(keys[keypress]);
                
                state_changed = false;
                
                break;
            default:
                program_state = STATE_STANDBY;
        }
        
        if(state_changed) {
            __lcd_clear();
            __lcd_home();
            PROG_FUNC[program_state]();
        }
        
        INT1IF = 0;  // Clear interrupt flag bit to signify it's been handled
    }
}
