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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_bits.h"
#include "lcd.h"
#include "I2C.h"
#include "GLCD_PIC.h"
#include "SPI_PIC.h"
#include "px_ascii.h"
#include "protocol_manager.h"

/***** Macros *****/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

/***** Function Prototypes *****/
void RTC_setTime(void);

/***** Constants *****/
const char keys[] = "123A456B789C*0#D";
const char happynewyear[7] = {  0x00, // 45 Seconds 
                                0x43, // 59 Minutes
                                0x16, // 24 hour mode, set to 23:00
                                0x02, // Tues
                                0x20, // 20th
                                0x02, // Feb
                                0x18  // 2018
};

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
    initGLCD();
    
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, RED);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 14, 110, BLACK);
    glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
    
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    
    /* Set the time in the RTC.
     * 
     * To see the RTC keep time, comment this line out after programming the PIC
     * directly before with this line included. */
    //RTC_setTime();
    
    /* Declare local variables. */
    unsigned char time[7]; // Create a byte array to hold time read from RTC
    unsigned char i; // Loop counter
    
    /* Main loop. */
    while(1) {
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        for(i = 0; i < 6; i++){
            time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        
        /* Print received data to LCD. */
        __lcd_home();
        printf("%02x/%02x/%02x", time[6],time[5],time[4]); // Print date in YY/MM/DD
        __lcd_newline();
        printf("%02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
        
        use_protocol(SPI);
        glcdDrawRectangle(0, GLCD_SIZE_VERT, 0, 14, GREEN);
        char buffer[22];
        int a = 5, b = 3;
        sprintf(buffer, "%02x:%02x:%02x", time[2], time[1], time[0]);
        print_px_string(0, 0, buffer);
        use_protocol(I2C);
        
        __delay_ms(1000);
    }
}

void RTC_setTime(void){
    /* Writes the happynewyear array to the RTC memory.
     *
     * Arguments: none
     *
     * Returns: none
     */
    
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    
    /* Write array. */
    for(char i=0; i<7; i++){
        I2C_Master_Write(happynewyear[i]);
    }
    
    I2C_Master_Stop(); //Stop condition
}
