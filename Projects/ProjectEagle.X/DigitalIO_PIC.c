#include "DigitalIO_PIC.h"

void orient_container(void) {
    rotary_angle = 0;
    
    TURNTABLE_CW();
    int cnt = 0;
    while(rotary_angle < 405) {
        if(OPTICAL_SENSOR_1) {
            cmpt1 = rotary_angle;
        }
    }
    
    TURNTABLE_STOP();
    __delay_ms(1000);
    
    TURNTABLE_CCW();
    __delay_ms(1000);
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
//    char angle = CMPT1;
//    
//    TURNTABLE_CW = 1;
//    TURNTABLE_CCW = 0;
//    
//    // TODO WITH ROTARY ENCODER POLLING
//    
//    TURNTABLE_CW = 0;
//    TURNTABLE_CCW = 0;
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

