/*
 * File:   main.c
 * Author: Morris Chen
 * 
 * Created on February 4, 2018, 6:44 PM
 * 
 * Preconditions:
 *   1. GLCD is in a PIC socket
 *   2. LCD is in a PIC socket
 *   3. Keypad is in a PIC socket
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
#include "program_states.h"

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
    
    print_px_string(0, 0, "Feb 06, 2018 1:26 PM");
    
    char eta[] = "ETA:     - min -- sec\0";
    unsigned int sec = 180;
    
    while(1) {
        eta[9] = '0' + (sec/60);
        eta[15] = '0' + ((sec%60)/10);
        eta[16] = '0' + ((sec%60)%10);
        glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
        print_px_string(0, 114, eta);
        __delay_ms(1000);
        
        if(program_state == STATE_EXECUTE) {
            sec--;
        }
    }
    
    return;
}

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

void interrupt interruptHandler(void) {
    if(INT1IF) {
        /* Interrupt on change handler for RB1. */
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        unsigned char key = keys[keypress];
        bool state_changed = true;
        
        switch(program_state) {
            case STATE_STANDBY: // Key pressed on standby mode
                program_state = STATE_PROMPT_COMPARTMENT_COUNT; // Proceed to prompt for number of compartments
                break;
            case STATE_PROMPT_COMPARTMENT_COUNT: // Key pressed while prompting for compartment count
                if('4' <= key && key <= '8') { // Check if input admissible
                    program_status.compartment_count = key - '0'; // Store compartment count
                    program_status.compartment_count_index = 0; // Start compartment index at 0
                    // Reset the B, N, S, and W for each compartment to 0
                    memset(program_status.set_count, 0, sizeof(program_status.set_count[0][0]) * 8 * 4);
                    // Fill buffer with null terminators
                    for(char i = 0 ; i < 8; i++) program_status.buffer[i] = '\0';
                    program_state = STATE_PROMPT_FASTENER_SET; // Proceed to prompt for fastener set contents
                }
                break;
            case STATE_PROMPT_FASTENER_SET: // Key pressed while prompting for fastener set contents
                if('A' <= key && key <= 'Z') { // Check if input is uppercase alphabet
                    char leaf_index = trie_node_leaf_index(key);
                    if(leaf_index < 4) {
                        program_status.buffer[program_status.buffer_index++] = key;
                        putch(key);
                        switch(key) {
                            case 'B': program_status.B++; break;
                            case 'N': program_status.N++; break;
                            case 'S': program_status.S++; break;
                            case 'W': program_status.W++; break;
                        }
                        program_status.trie_ptr = program_status.trie_ptr->children[leaf_index];
                    }
                    state_changed = false;
                } else if(key == '*' && program_status.buffer_index > 0) {
                    lcd_shift_cursor(1, 0);
                    putch(' ');
                    lcd_shift_cursor(1, 0);
                    char x = program_status.buffer[--program_status.buffer_index];
                    switch(x) {
                        case 'B': program_status.B--; break;
                        case 'N': program_status.N--; break;
                        case 'S': program_status.S--; break;
                        case 'W': program_status.W--; break;
                    }
                    program_status.buffer[program_status.buffer_index] = '\0';
                    program_status.trie_ptr = program_status.trie_ptr->parent;
                    state_changed = false;
                } else if(key == '#' && program_status.buffer_index > 0) {
                    if(strcmp(program_status.buffer,"BB") != 0 && strcmp(program_status.buffer,"BNN") != 0) {
                        program_status.max_quantity = 4;
                        if(program_status.B > 0) program_status.max_quantity = min(4/(program_status.B + program_status.N + program_status.S + program_status.W), 2/program_status.B);
                        if(program_status.N > 0) program_status.max_quantity = min(program_status.max_quantity, 3/program_status.N);
                        if(program_status.S > 0) program_status.max_quantity = min(program_status.max_quantity, 2/program_status.S);
                        if(program_status.W > 0) program_status.max_quantity = min(program_status.max_quantity, 4/program_status.W);
                        
                        program_state = STATE_PROMPT_FASTENER_SET_QUANTITY;
                    } else {
                        state_changed = false;
                    }
                }
                break;
            case STATE_PROMPT_FASTENER_SET_QUANTITY:
                if('1' <= key && key <= ('0' + program_status.max_quantity)) {
                    program_status.set_count_tmp = key - '0';
                    program_state = STATE_PREVIEW_FASTENER_SET;
                } else {
                    state_changed = false;
                }
                break;
            case STATE_PREVIEW_FASTENER_SET:
                if(key == '#') {
                    while(program_status.trie_ptr != program_status.trie_ptr->parent) {
                        char c = program_status.trie_ptr->letter;
                        char p = (c <= 'N') ? ((c == 'B') ? 0 : 1) : ((c == 'S') ? 2 : 3);
                        program_status.set_count[program_status.compartment_count_index][p] += program_status.set_count_tmp;
                        program_status.trie_ptr = program_status.trie_ptr->parent;
                    }
                    ;
                    if(++program_status.compartment_count_index < program_status.compartment_count) {
                        program_state = STATE_PROMPT_FASTENER_SET;
                    } else {
                        program_state = STATE_CONFIRM_SETS;
                    }
                } else if(key == '*') {
                    program_state = STATE_PROMPT_FASTENER_SET;
                } else {
                    state_changed = false;
                }
                break;
            case STATE_CONFIRM_SETS:
                if(key == '*') {
                    program_state = STATE_PROMPT_COMPARTMENT_COUNT;
                } else if(key == '#') {
                    program_state = STATE_EXECUTE;
                } else if('1' <= key && key <= '0'+program_status.compartment_count) {
                    program_status.compartment_count_index = key - '1';
                    program_state = STATE_REVIEW_SET;
                } else {
                    state_changed = false;
                }
                break;
            case STATE_REVIEW_SET:
                program_state = STATE_CONFIRM_SETS;
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
