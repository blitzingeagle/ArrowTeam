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
#include <string.h>
#include <math.h>

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
#include "eep.h"
#include "canvas.h"
#include "history.h"

/***** Macros *****/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

/***** Function Prototypes *****/
//void RTC_setTime(void);

/***** Constants *****/
//const char keys[] =  "123B456N789S*0#W";
//const char keys[] = "WSNB#9630852*741";
const char keys[] = "147*2580369#BNSW";

char RX_packet[5];

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
    
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    
    // </editor-fold>
    
    /* Initialize LCD. */
    initLCD();
    
    /* Initialize GLCD. */
    initGLCD();
    
    draw_frame();
    draw_container();
    draw_table();
    
    /* Initialize I2C */
    I2C_Master_Init(9600); // Initialize I2C Master with 100 kHz clock
    RTC_read_time(program_status.time); // Read RTC time
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    /* Initialize UART */
    UART_Init(9600); // Baud rate 9600
    memset(RX_packet, ' ', 4);
    RX_packet[4] = '\0';
    uartReceiveIT(1);
    
    /* Initialize Program States */
    init_program_states();
    
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0; 
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 1;
    TMR0H = 0x67;
    TMR0L = 0x69;
    T0CONbits.TMR0ON = 1;
    TMR0IE = 1;
    
    ei(); // Enable all interrupts
}

void main(void) {
    init();
    
    printf("Initializing...");
    read_history(&program_status.history);
    
    __lcd_clear();
    FUNC_STATE_STANDBY(); // Start program in standby mode
    
    /* Main loop. */
    while(1) {
        if(program_status.operating) {
            struct Log *log = &program_status.history.logs[program_status.history.write_index];
            log->time_start = get_epoch_time(program_status.time);
            memset(log->set_enum, 0, 8);
            memset(log->qty, 0, 8);
            __lcd_clear();
            for(unsigned char set = 0; set < 8; set++) {
                unsigned char num_sets = program_status.compartment_count;
                unsigned char sector = (num_sets <= 6) ? set + (set / (num_sets - 3)) : set;
                log->set_enum[sector] = program_status.set_enum[set];
                printf("%d %d ", program_status.set_enum[set], log->set_enum[sector]);
                
                log->qty[sector] = program_status.set_qty[set];
            }
            
            log->num_sets = program_status.compartment_count;
            
//            use_protocol(SPI);
//            glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
//            char buffer[22];
//            sprintf(buffer, "%d step assembly", program_status.compartment_count);
            
            arduino_send_fastener_data(program_status.set_count, program_status.compartment_count);
            
            while(program_status.operating) {
                di();
                use_protocol(SPI);
                update_header();
                ei();
                __delay_ms(500);
            }
            printf("Done!");
            
            log->time_end = get_epoch_time(program_status.time);
            log->overflow[0] = program_status.overflow.B;
            log->overflow[1] = program_status.overflow.N;
            log->overflow[2] = program_status.overflow.S;
            log->overflow[3] = program_status.overflow.W;
            program_status.time_elapsed = log->time_end - log->time_start;
            
            for(unsigned char i = 0; i < 8; i++) {
                printf("%d ", program_status.history.logs[program_status.history.write_index].set_enum[i]);
            }
            program_status.history.write_index = (program_status.history.write_index + 1) % MAX_LOGS;
            
            __delay_ms(5000);
            
//            add_log(log, &program_status.history);
            
            program_state = STATE_COMPLETION;
            __lcd_clear();
            __lcd_home();
            PROG_FUNC[program_state]();
            
            di();
            write_history(program_status.history);
            ei();
        }
        di();
        use_protocol(SPI);
        update_header();
        ei();
        __delay_ms(500);
    }
}



void TX_interface(void) {    
}

void RX_interface(void) {
    for(unsigned char c = 0; c < 3; c++) RX_packet[c] = RX_packet[c+1];
    RX_packet[3] = UART->_dataRX[0];
    UART->_numReceives = 0;
    
    if(strcmp(RX_packet, "DONE") == 0) {
        program_status.operating = false;
    } else if(RX_packet[0] == 'E' && RX_packet[1] == 'X') {
        unsigned char count = RX_packet[3] - 'A';
        switch(RX_packet[2]) {
            case 'B': program_status.overflow.B = count; break;
            case 'N': program_status.overflow.N = count; break;
            case 'S': program_status.overflow.S = count; break;
            case 'W': program_status.overflow.W = count; break;
        }
    } else {
        uartReceiveIT(1);
        return;
    }
    
    uartTransmitIT(" ACK", 4);
    uartReceiveIT(1);
}

void interrupt interruptHandler(void) {
    enum PROTOCOL p = curr_protocol;
    
    if(INT1IE && INT1IF) { /* Interrupt on change handler for RB1. */
        di();
        INT1IF = 0;  // Clear interrupt flag bit to signify it's been handled
        
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        unsigned char key = keys[keypress];
        
//        use_protocol(SPI);
//        sprintf(Canvas.footer_text, "Pressed: %c", key);
//        update_footer();
        
        program_states_interrupt(key);
        
        TMR0IF = 0;
        
        ei();
    }
    
    if(TMR0IE && TMR0IF) {
        di();
        TMR0IF = 0;
        
        unsigned char *time = program_status.time;
        
        use_protocol(I2C);
        RTC_read_time(time);

        sprintf(Canvas.header_text, "%02x/%02x/%02x     %02x:%02x:%02x", time[5], time[4], time[6], time[2], time[1], time[0]);
        
        if(program_state == STATE_SET_TIME && program_status.edit_time_idx == 0) {
            FUNC_STATE_SET_TIME();
        }
        
        ei();
    }
    
    /* Handle UART interrupt */
    UART_interrupt(TX_interface, RX_interface);
    
    use_protocol(p);
}

