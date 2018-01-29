/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on August 12, 2017, 5:40 PM
 * 
 * Description: Demonstration of some of the character LCD capabilities. Note
 *              that the more you use the standard print formatting library, the
 *              more memory your program will take up!
 * 
 * Precondition:
 *   1. Character LCD is in a PIC socket
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "lcd.h"

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
    TRISA = 0xFF; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    /* Initialize character LCD. */
    initLCD();

    /* Demonstrate printing a string literal. */
    printf("Hello world!");
    __delay_ms(2000);
    
    /* Main loop. */
    while(1){
        /* Demonstrate newline. */
        __lcd_clear();
        __lcd_home();
        printf("Text can be on");
        __lcd_newline();
        printf("multiple lines");
        __delay_ms(2000);
        
        /* Demonstrate place holders. */
        __lcd_clear();
        __lcd_home();
        printf("Integer: %d", 10);
        __lcd_newline();
        printf("Float: %f", 3.14159);
        __delay_ms(2000);
        
        __lcd_clear();
        __lcd_home();
        printf("Long: %lu", 4291234567); // "%lu" is for unsigned long
        __lcd_newline();
        char arr[7] = "AER201\0"; // Strings must be null-terminated
        printf("Char[]: %s", arr);
        __delay_ms(2000);
        
        /* Demonstrate cursor blink on/off. */
        __lcd_clear();
        __lcd_home();
        printf("Cursor blink can");
        __lcd_newline();
        printf("be toggled");
        __delay_ms(2000);
        __lcd_display_control(1, 1, 0); // Turn off blink
        __delay_ms(2000);
        
        /* Demonstrate cursor on/off. */
        __lcd_clear();
        __lcd_home();
        printf("As can the");
        __lcd_newline();
        printf("cursor!");
        __delay_ms(2000);
        __lcd_display_control(1, 0, 0); // Turn off cursor
        __delay_ms(2000);
        
        /* Demonstrate display on/off. */
        __lcd_clear();
        __lcd_home();
        printf("As can the");
        __lcd_newline();
        printf("display");
        __delay_ms(2000);
        __lcd_display_control(0, 0, 0); // Turn off display
        __delay_ms(2000);
        
        __lcd_clear();
        __lcd_home();
        printf("But that will");
        __lcd_newline();
        printf("likely stay on!");
        __lcd_display_control(1, 0, 0); // Turn on display
        __delay_ms(2000);

        /* Demonstrate set cursor position function. */
        __lcd_clear();
        __lcd_home();
        printf("Cursor position");
        __lcd_newline();
        printf("can be specified");
        __delay_ms(2000);
        __lcd_clear();
        __lcd_home();

        /* Demo of bouncing the cursor position around the screen. */
        for(unsigned char x = 0; x < LCD_SIZE_HORZ; x++){
            if(x % LCD_SIZE_VERT){
                lcd_set_cursor((unsigned char)(LCD_SIZE_HORZ - x), \
                        (unsigned char)(x % LCD_SIZE_VERT));
                printf("%d", x % 10);
            }
            else{
                lcd_set_cursor(x, (unsigned char)(x % LCD_SIZE_VERT));
                printf("%d", x % 10);
            }
            __delay_ms(150);
        }
        
        /* Demonstrate display shifting. */
        unsigned char numShifts = 0; /* Store this for origin reference. When
                                      * the display is shifted, coordinate
                                      * (0, 0) will no longer be visible on a
                                      * 16x2 panel, so we want to make sure we
                                      * track where we're moving it so we don't
                                      * lose it. */
        
        lcd_set_cursor(LCD_SIZE_HORZ + 1, 0);
        printf("Display can be");
        lcd_set_cursor(LCD_SIZE_HORZ + 4, 1);
        printf("shifted");
        __delay_ms(2000);
        
        numShifts = LCD_SIZE_HORZ;
        for(unsigned char i = 0; i < numShifts; i++){
            lcd_shift_display(1, LCD_LEFT);
            __delay_ms(150);
        }
        __delay_ms(2000);
        
        /* Clear bottom row of display characters. */
        lcd_set_cursor(numShifts - 16, 1);
        for(unsigned char i = numShifts; i > 0; i--){
            putch(' ');
        }
        
        /* Write text offscreen (area we shifted away from) and shift back from
         * it. */
        lcd_set_cursor(numShifts - 16, 0);
        printf("Hello world!    ");
        
        for(unsigned char i = 0; i < LCD_SIZE_HORZ; i++){
            lcd_shift_display(1, LCD_RIGHT);
            __delay_ms(150);
        }
        __lcd_display_control(1, 1, 1); // Turn on cursor, blink, and display
        __delay_ms(2000);
    }
}
