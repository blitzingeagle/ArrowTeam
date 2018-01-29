/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 *
 * Created on July 28, 2017, 6:37 PM
 */


#include <xc.h>
#include "machineConfig.h"
#include "Keypad.h"
#include "UART_PIC.h"

void main(void) {
    /* Configure pins, SFRs, and on-board modules. */
    machineConfig();
    
    /* Initialize EUSART module for asynchronous 9600/8N1. */
    UART_Init(9600);
    __UART_ENABLE_TX(); // Enable transmitter
    
    /* Disable the data bus and data available pin by setting them to high
     * impedance (input). This way, RB1, RB7:4 on the primary PIC can be used
     * as GPIO. */
    TRISA = 0b11101111;
    
    LATAbits.LATA4 = 1; // Set RA4 output high (i.e. set KPD)
    
    while(1){
        keypadRoutineUART();
    }
}
