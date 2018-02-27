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
        __delay_ms(250);
        angle += 15;
    }
    
    // Turn table counterclockwise
    
    while(angle > 0) {
        
    }
}

void DigitalIO_interrupt(void) {
    
}

