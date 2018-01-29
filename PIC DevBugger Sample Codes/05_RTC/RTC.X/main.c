/*
 * File:   main.c
 * Author: Michael Ding
 *
 * Created on July 18, 2016, 12:11 PM
 * 
 * Edited by Tyler Gamvrelis, summer 2017
 * 
 * Description: Demonstration of setting the real-time clock (RTC) and reading
 *              values back from it.
 * 
 * Preconditions:
 *   1. Jumpers JP6, and JP7 in the RTC module are shorted
 *   2. RTC chip is properly situated in its socket
 *   3. CR2032 20 mm coin battery is properly situated in its socket and is not
 *      low on charge
 */

/***** Includes *****/
#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "lcd.h"
#include "I2C.h"

/***** Macros *****/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

/***** Function Prototypes *****/
void RTC_setTime(void);

/***** Constants *****/
const char keys[] = "123A456B789C*0#D";
const char happynewyear[7] = {  0x45, // 45 Seconds 
                                0x59, // 59 Minutes
                                0x23, // 24 hour mode, set to 23:00
                                0x00, // Sunday
                                0x31, // 31st
                                0x12, // December
                                0x17  // 2017
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

    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    di(); // Disable all interrupts
    
    /* Set the time in the RTC.
     * 
     * To see the RTC keep time, comment this line out after programming the PIC
     * directly before with this line included. */
    //RTC_setTime();
    
    /* Declare local variables. */
    unsigned char time[7]; // Create a byte array to hold time read from RTC
    unsigned char i; // Loop counter
    
    /* Main loop. */
    while(1){
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
