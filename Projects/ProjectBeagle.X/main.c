#include <xc.h>

#include <stdio.h>
#include "configBits.h"
#include "lcd.h"
 
int tick_count=0x0;
 
void main(void) {
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
    
    /* Initialize LCD. */
    initLCD();
    
    __lcd_clear();
    printf("START");
    
    /*
    T1CON = 0x01;               //Configure Timer1 interrupt
    PIE1bits.TMR1IE = 1;           
    INTCONbits.PEIE = 1;
    RCONbits.IPEN=0x01;
    IPR1bits.TMR1IP=0x01;            // TMR1 high priority ,TMR1 Overflow Interrupt Priority bit
    INTCONbits.GIE = 1;
    PIR1bits.TMR1IF = 0;
    T0CON=0X00;
    INTCONbits.T0IE = 1;               // Enable interrupt on TMR0 overflow
    INTCON2bits.TMR0IP=0x00;        
    T0CONbits.TMR0ON = 1;
    
    while(1) {
        if(tick_count % 153 == 0) {
            __lcd_clear();
            printf("%d", tick_count/153);
        }
    }
    */
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 0;
    TMR0H = 0x67;
    TMR0L = 0x69;
    T0CONbits.TMR0ON = 1;
    TMR0IE = 1;
    
    while(1);
}
 
void interrupt interruptHandler(void) {
    
    if(TMR0IF) {
        TMR0IF = 0;
        
        ++tick_count;
        
        __lcd_clear();
        printf("%d", tick_count);
    }
}

//void interrupt tc_int(void) {            // High priority interrupt
//    if(TMR1IE && TMR1IF) {
//        TMR1IF=0;
//        ++tick_count;
//        
//        TRISC=1;
//        LATCbits.LATC4 = 0x01;
//    }
//}
// 
//void interrupt low_priority LowIsr(void) {    // Low priority interrupt
//    if(INTCONbits.T0IF && INTCONbits.T0IE) {  // If Timer flag is set & Interrupt is enabled
//        INTCONbits.T0IF = 0;            // Clear the interrupt flag 
//        ADCON1=0x0F;
//        TRISB=0x0CF;
//        LATBbits.LATB5 = 0x01;          // Toggle a bit 
//    }
//}