/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 * 
 * Created on August 29, 2017, 4:51 PM
 * 
 * Description: Sample program for arbitrary information exchange between
 *              the main PIC and the Arduino Nano. The character 'H' is sent to
 *              the Arduino Nano via UART. To see the results on the Arduino
 *              Nano side, use the Arduino sample program Arduino_UART_PIC.
 * 
 * Preconditions:
 *   1.  Main PIC uses hardware UART
 *   2.  Arduino Nano uses software UART
 *   3.  The Co-processor is not driving lines on the UART bus (e.g. the JP_TX and JP_RX jumpers are removed)
 *   4.  The character LCD is in an Arduino Nano socket
 *   5.  PIC-Arduino link switches are enabled (ON) for D0 of the Arduino (the RX pin).
 *       However, make sure that neither of D0 and D1 are enabled (ON) while programming the Arduino Nano
 */


#include <xc.h>
#include "machineConfig.h"
#include "configBits.h"
#include "lcd.h"
#include "UART_PIC.h"

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
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    /* Initialize UART. */
    UART_Init(9600);

    /* Main loop. */
    unsigned char arr[1] = {'H'};
    while(1){
        /* Send 'H'. */
        uartTransmitIT(arr, 1);
        uartReceiveIT(1);
        __delay_ms(1000); // Wait 1 second
    }
}

void interrupt interruptHandlerUART(void){
    /* Any time that an interrupt is generated, the microcontroller will branch
     * here. As such, any interrupts that are enabled need to be handled in this
     * function. Otherwise, unexpected behavior can arise.
     */
    
    /* UART transmit: TXREG empty */
    if(IF_TX){
        /* Load data into transmit register TXREG */
        UARTinterruptState = INTCONbits.GIE; // Enter critical section
        di();
        TXREG = UART -> _dataTX[UART -> _numTransmits];
        INTCONbits.GIE = UARTinterruptState; // Exit critical section
        
        UART -> _numTransmits++; // Increment transmit count

        /* Disable transmit interrupts if last byte transmitted */
        if((UART -> _numTransmits) == (UART -> _numBytesTX)){
            IE_TX = 0;
            
            /* Set TX state to ready */
            UART -> _stateTX = UART_STATE_READY;
        }
        
        TRISCbits.TRISC0 = 0;
        LATCbits.LATC0 = 1;
    }
    
    /* UART receive: RCREG full */
    if(IF_RX){
        /* Check/clear framing error (too many or too few bytes between start
         * and stop bit.*/
        if(FRAMING_ERROR){
            /* Store information for now. If there is a framing error, it will 
             * be cleared when the receive register RCREG is read. But, in this
             * case, that entry in _dataRX will be garbage */
            UART -> _lastReceiveFERR = 1;
        }
        else{
            UART -> _lastReceiveFERR = 0;
        }

        /* Read RCREG into memory */
        UARTinterruptState = INTCONbits.GIE; // Enter critical section
        di();
        UART -> _dataRX[UART -> _numReceives] = RCREG;
        INTCONbits.GIE = UARTinterruptState; // Exit critical section

        /* Check/clear overrun error (next byte arrives before previous
         * was read). */
        if(OVERRUN_ERROR){
            /* Receive buffer flushed in hardware when receiver is reset */
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        else{
            UART -> _lastReceiveOERR = 0;
        }
        
        UART -> _numReceives++; // Increment receive count
        
        /* Disable RX interrupts if last byte received */
        if(UART -> _numReceives >= UART -> _numBytesRX){
            IE_RX = 0;
            
            /* Set RX state to ready */
            UART -> _stateRX = UART_STATE_READY; 
        }
    }
}

