#include "DigitalIO_PIC.h"

void orient_container(void) {
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    
    unsigned char angle = 0;
    
    // Turn table clockwise
    TURNTABLE_CW = 1;
    TURNTABLE_CCW = 0;
    
    while(angle < 405) {
        // replace with rotary encoder polling
//        char x = ROTARY_ENCODER;
        
        __delay_ms(250);
        angle += 15;
        
        // Read for white tape
//        char opt = OPTICAL_SENSOR_1;
//        if(opt) {
//            CMPT1 = 0;
//        }
//        CMPT1 += 15;
    }
    
    // Turn table counterclockwise
    TURNTABLE_CW = 0;
    TURNTABLE_CCW = 1;
    
    while(angle > 0) {
        // replace with rotary encoder polling
        __delay_ms(250);
        angle -= 15;
        
        // Read for white tape
//        char opt = OPTICAL_SENSOR_1;
//        if(opt) {
//            CMPT1 = 0;
//        }
//        CMPT1 += 15;
    }
    
    // Stop turn table
    TURNTABLE_CW = 0;
    TURNTABLE_CCW = 0;
}

void clamp_up(void) {
    INT2IE = 1; // Enable RB2 (microswitch) interrupt
    
    LEADSCREW_CW = 0;
    LEADSCREW_CCW = 1;
    
    while(INT2IE);
}

void clamp_down(void) {
    INT2IE = 1; // Enable RB2 (microswitch) interrupt
    
    LEADSCREW_CW = 1;
    LEADSCREW_CCW = 0;
    
    while(INT2IE);
}

void rotate_to_compartment(unsigned char cmpt) {
    char angle = CMPT1;
    
    TURNTABLE_CW = 1;
    TURNTABLE_CCW = 0;
    
    // TODO WITH ROTARY ENCODER POLLING
    
    TURNTABLE_CW = 0;
    TURNTABLE_CCW = 0;
}

void DigitalIO_interrupt(void) {
    if(INT2IE && INT2IF) {
        INT2IF = 0; // Clear interrupt flag
        
        // Stop leadscrew turning
        LEADSCREW_CW = 0;
        LEADSCREW_CCW = 0;
        
        INT2IE = 0; // Disable microswitch interrupt
    }
}

