/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on October 1, 2017, 2:28 PM
 * 
 * Description: Test of basic capabilities of the development board.
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "GLCD_PIC.h"
#include "I2C.h"
#include "lcd.h"
#include "SD_PIC.h"
#include "SPI_PIC.h"
#include "UART_PIC.h"

/***** Constants *****/
const char happynewyear[7] = {  0x45, // 45 Seconds 
                                0x59, // 59 Minutes
                                0x23, // 24 hour mode, set to 23:00
                                0x00, // Sunday
                                0x31, // 31st
                                0x12, // December
                                0x17  // 2017
};

/* Enumerate program states. */
enum programStates{
    SETRTC,         //  0   (key 1)
    GETRTC,         //  1   (key 2)
    GLCD,           //  2   (key 3)
    SDCARD = 4,     //  4   (key 4)
    IO = 5,         //  5   (key 5)
    UARTSEND = 6,   //  6   (key 6)
    A2D = 7         //  8   (key 7)
};

/* Global array to hold RTC time. */
unsigned char time[7]; // Create a byte array to hold time read from RTC

/* Function prototypes. */
void testRTCSet(void);
void testRTCGet(void);
void testGLCD(void);
void testSDCard(void);
void testIO(void);
void testUART(void);
void testA2D(void);
unsigned short readADC(char channel);

enum programStates getKey(void){
    /* Blocks program execution until a key is pressed. When a key is pressed, 
     * this function blocks program execution until the key is released.
     *
     * Inputs: none
     *
     * Outputs: byte (which is an enum member) corresponding to the key
     */
    
    /* Declare local variables. */
    enum programStates key;
    
    /* While no key is pressed, don't do anything. */
    while(PORTBbits.RB1 == 0){  continue;   }
    
    key = (PORTB & 0xF0) >> 4; // Use the keypad to select the state
    
    /* While key is being pressed, don't do anything. */
    while(PORTBbits.RB1 == 1){  continue;   }
    
    return key;
}

void main(void){
    /* Configure pins, SFRs, and on-board modules. */
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
    TRISA = 0b11111111; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0b11110010; // RB1, RB4, RB5, RB6, RB7 as inputs (for keypad)
    TRISC = 0b10000000; /* RC3 is SCK/SCL (SPI/I2C),
                         * RC4 is SDI/SDA (SPI/I2C),
                         * RC5 is SDO (SPI),
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
    ADCON1 = 0x0F; // Set all A/D ports to digital (pg. 222)
    ADCON2bits.ADFM = 1; // Right justify A/D result
    
    CVRCON = 0x00; // Disable comparator voltage reference (pg. 239)
    CMCONbits.CIS = 0; // Connect C1 Vin and C2 Vin to RA0 and RA1 (pg. 233)
    ADCON2 = 0b10110001; // Right justify A/D result, 16TAD, FOSC/8 clock
    //</editor-fold>
    
    /* Initialize some of the peripherals */
    initLCD();
    UART_Init(9600);
    
    /* Declare/initialize variables. */
    enum programStates state; // Global state variable
    
    /* Main loop. */
    while(1){
        /* Test character LCD hardware by basic print function. */
        __lcd_clear();
        __lcd_home();
        printf("BOARD TEST");
        __lcd_newline();
        printf("Press a key");
        __delay_ms(250);
        
        /* Get user input. */
        state = getKey(); // Blocks program execution until a key is pressed
        
        /* Change program state based on user input. */
        switch(state){
            case SETRTC:    //  key 1
                testRTCSet();
                continue;
            case GETRTC:    //  key 2
                testRTCGet();
                continue;
            case GLCD:      //  key 3
                testGLCD();
                continue;
            case SDCARD:    //  key 4
                testSDCard();
                continue;
            case IO:        //  key 5
                testIO();
                continue;
            case UARTSEND:  //  key 6
                testUART();
                continue;
            case A2D:       //  key 7
                testA2D();
                continue;
            default:
                break; // Try again!
        }
    }
}

void testRTCSet(void){
    /* Sets the time on the RTC based on the values stored in the happynewyear
     * array.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds

    /* Write array. */
    for(unsigned char i = 0; i < 7; i++){
        I2C_Master_Write(happynewyear[i]);
    }

    I2C_Master_Stop(); //Stop condition
}

void testRTCGet(void){
    /* Gets the current time from the RTC and displays the results on the
     * character LCD. This function incorporates an infinite loop that updates
     * the time once per second. The loop can be exited by pressing any key on
     * the keypad.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    while(1){
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        for(unsigned char i = 0; i < 6; i++){
            time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition

        /* Print received data to LCD. */
        __lcd_clear();
        __lcd_home();
        printf("%02x/%02x/%02x", time[6],time[5],time[4]); // Print date in YY/MM/DD
        __lcd_newline();
        printf("%02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
        
        /* Check for key press while delaying for about 1 second. */
        for(int i = 0; i < 100; i++){
            __delay_ms(10);
            if(PORTBbits.RB1 == 1){
                while(PORTBbits.RB1 == 1){  continue;   }
                return;
            }
        }
    }
}

void testGLCD(void){
    /* Initializes the GLCD and draws two rectangles on it.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    initGLCD(); // Initialize the GLCD
    glcdDrawRectangle(0, 64, 0, 128, RED); // Left half of screen is red
    glcdDrawRectangle(64, 128, 0, 128, VIOLET); // Right half of screen is violet
}

void testSDCard(void){
    /* Initializes the SD card and displays the storage capacity on the
     * character LCD as a double. To exit this function, press any key on the
     * keypad.
     * 
     * NOTE: If there is no SD card inserted, this function will never finish.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    initSD(); // Initialize SD card
    __lcd_clear();
    __lcd_home();
    printf("SD Card success!");
    __lcd_newline();
    printf("Size: %f", SDCard.size); // Display card capacity as a double
    while(PORTBbits.RB1 == 0){  continue;   } // Exit on key press
}

#define DELAY_TIME  500
void testIO(void){
    /* This function is identical in functionality to the PortTest sample code.
     * The pin iteration runs in an infinite loop that can only be exited by
     * resetting the PIC.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    /* First, we need to make sure the PIC is configured correctly. Usage of the
     * on-board modules will sometimes force pins to be in a certain state by
     * default, which is undesirable for this test.
     * 
     * First, we disable the UART peripheral. Take note of the steps that are
     * required.
     */
    __UART_DISABLE_TX(); // Disable the transmitter
    __UART_DISABLE_RX(); // Disable the receiver
    RCSTAbits.SPEN = 0; // Disable the serial port (if enabled, RC6 and RC7 will
                        // behave as serial port pins, which means we cannot
                        // control them using the TRISx & LATx registers)
    
    /* Next, we disable the MSSP module, which incorporates the SPI interface
     * used by the SD card and GLCD, as well as the I2C interface used by the
     * RTC. Note that the MSSP module only operates one of these interfaces
     * (SPI, I2C) at any given time. */
    __MSSP_DISABLE();
    
    /* Finally, we change our I/O settings. */
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    while(1){
        /* Initialize local variables. */
        unsigned char i = 0;
        unsigned char temp = 0x00;

        temp = 1 << 7; // Set temporary byte to 0b10000000.
        /* Write temp to the latch, then bit shift to right. Iterate over the port
         * size. */
        for (i = 0; i < 8; i++){
            LATA = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATA = 0x00; // Clear latch outputs after demonstrating the port.

        /* Same process for all other ports. */
        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATB = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATB = 0x00;

        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATC = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATC = 0x00;

        temp = 1 << 7;
        for (i = 0; i < 8; i++){
            LATD = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATD = 0x00;

        /* Note: Look in the SD card module to see the LED for RE2. */
        temp = 1 << 2;
        for (i = 0; i < 3; i++){
            LATE = temp;
            temp >>= 1;
            __delay_ms(DELAY_TIME);
        }
        LATE = 0x00;
    }
}

void testUART(void){
    /* Sends "Hello world!" via the UART bus, which can be received by a PC
     * connected to the USB port in the USB module.
     *
     * Inputs: none
     * 
     * Outputs: none
     */
    
    char message[] = "Hello world!"; // Create a new array of characters
    uartTransmitBlocking(message, sizeof(message)); // Send the message
}

void testA2D(void){
    /* Acquires analog data from RA2 and RA3 and displays the results on the 
     * character LCD in hexadecimal format. This function incorporates an 
     * infinite loop that updates the readings 10 times per second. The loop can
     * be exited by pressing any key on the keypad.
     * 
     * Inputs: none
     * 
     * Outputs: none
     */
    
    ADCON1 = 0x0B;  // RA0-3 are analog (pg. 222)
    while(1){
        __lcd_clear();
        __lcd_home();
        printf("RA2: %.3x", readADC(2));
        __lcd_newline();
        printf("RA3: %.3x", readADC(3));
        __delay_ms(100);
        if(PORTBbits.RB1 == 1){  break;  } // Exit on key press
    }
    ADCON1 = 0x0F; // All inputs are digital
}

/* Helper function for the A2D test. */
unsigned short readADC(char channel){
    /* Reads the analog input from the specified analog channel.
     *
     * Arguments: channel, the byte corresponding to the channel to read
     *
     * Returns: the 10-bit value corresponding to the voltage read from
     *          the specified channel
     */
    
    ADCON0 = (channel & 0x0F) << 2; // Select ADC channel (i.e. pin)
    ADON = 1; // Enable module
    ADCON0bits.GO = 1; // Initiate sampling
    while(ADCON0bits.GO_NOT_DONE){  continue;   } // Poll for acquisition completion
    return (ADRESH << 8) | ADRESL; // Return result as a 16-bit value
}