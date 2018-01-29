/*
 * File:   Keypad.c
 * Author: Michael Ding
 *
 * Created on July 28, 2016, 11:10 AM
 * 
 * Edited by Tyler Gamvrelis on June 9, 2017
 */


#include <xc.h>
#include "Keypad.h"

unsigned char readKeyboard(void){
    /* Iterates through the keypad for possible presses. For each row that is
     * set high, each column is scanned for a connection (by default, these
     * columns being scanned are pulled to ground). The first key found is 
     * returned.
     * 
     * Arguments: none
     * 
     * Returns: byte corresponding to the key press, or 0xF0 if no key press
     */

    for(unsigned char i = 0; i < 4; i++){
        if(i == 0)
            LATB = 0x10; // 0b00010000
        else if(i == 1) 
            LATB = 0x20; // 0b00100000
        else if(i == 2)
            LATB = 0x40; // 0b01000000
        else if(i == 3)
            LATB = 0x80; // 0b10000000
         
        if(PORTBbits.RB0)
            return (i * 4U);
        if(PORTBbits.RB1)
            return (i * 4U) + 1U;
        if(PORTBbits.RB2)
            return (i * 4U) + 2U;
        if(PORTBbits.RB3)
            return (i * 4U) + 3U;
    }
    return 0xF0;
}

inline void keypadRoutine(void){
    /* Main keypad routine.
     *
     * Arguments: none
     *
     * Returns: none
     */
     
    /* Check if the keypad is supposed to be disabled. */
    if(KPD){
        __CLEAR_DATA_AVAILABLE();
        __DISABLE_BUS();
        while(KPD){ continue; }
        __CLEAR_BUS();
        __IDLE_BUS();
    }
    
    /* If the keypad is not supposed to be disabled, check for a key press. */
    else{
        __IDLE_BUS();
        
        /* Poll the keyboard for a key press. */
        dataOut = readKeyboard();

        /* Check if a key push is detected. */
        if (dataOut != 0xF0){

            /* Debounce button press. If the same key is read a second time,
             * after 20 milliseconds and it is the same as before, it is assumed
             * valid. */
            __delay_ms(20);
            if (dataOut == readKeyboard()){

                /* Write key press data to pins. Do not set the data available
                 * pin until the data has been latched onto the bus, or else
                 * the data may not be stable/valid when RB1 is read. */
                LATA = dataOut & 0b00001111U;
                __SEND_DATA();
                __SET_DATA_AVAILABLE();
                
                /* Hold data avaliable pin high while key is pressed. */
                while (dataOut == readKeyboard()){
                    
                    /* If the keypad is disabled during a key press, then
                     * disable the bus in this loop. */
                    if(KPD){
                        TRISA = 0xFF;
                        while(KPD){
                            continue;
                        }
                    }
                }
            }
            
           /* Clear the data latched on the bus, and set data pins to high 
            * impedance. */
            __CLEAR_BUS();
           __IDLE_BUS();
        }
    }
}

void keypadRoutine_StateBased(void){
    /* Keypad routine that runs with minimal blocking (i.e. allows the execution
     * of other tasks).
     *
     * Arguments: none
     *
     * Returns: none
     */
     
    /* Check current state */
    switch(KPDState){
        
        /* If the keypad is supposed to be disabled, check for first entry
         * and update the state. */
        case disabled:
            if(KPD){
                if(keypadFlags.f_enabled_to_disabled){
                    keypadFlags.f_enabled_to_disabled = 0; // Clear flag
                    
                    /* Set first entry into "keypad enabled" case, should the 
                     * condition arise.*/
                    keypadFlags.f_disabled_to_enabled = 1;
                    __CLEAR_DATA_AVAILABLE();
                    __DISABLE_BUS();
                    return;
                }
            }
            else{
                /* State transition. */
                KPDState = polling;
                return;
            }
            
        /* If the keypad is not supposed to be disabled, check for a key press. */
        case polling:
            if(KPD){
                KPDState = disabled;
                return;
            }
            /* If the keypad is not supposed to be disabled, check the first
             * entry flag to clear the bus, and then check for a keypress. */
            if(keypadFlags.f_disabled_to_enabled){
                keypadFlags.f_disabled_to_enabled = 0; // Clear flag
                
                /* Set first entry into "keypad disabled" case, should the 
                 * condition arise.*/
                keypadFlags.f_enabled_to_disabled = 1;
                __CLEAR_BUS();
            }
            __IDLE_BUS();

            /* Poll the keyboard for a key press. */
            dataOut = readKeyboard();
            
            /* Check if a key push is detected. */
            if (dataOut != 0xF0){
                
                /* Update state. */
                KPDState = debouncing;
            }
            return;
            
        /* If a key press was registered, then debounce it to see if it is
         * valid. */    
        case debouncing:
            if(KPD){
                KPDState = disabled;
                return;
            }
            /* Debounce button press. If the same key is read a second time,
             * after 20 milliseconds and it is the same as before, it is assumed
             * valid. In the future, it would be better to set a 20 ms timer and
             * have 1 state that starts the timer, one that returns while it's
             * not done, and one that updates the state once it is finished. */
            __delay_ms(20);
            if (dataOut == readKeyboard()){
                /* Update state. */
                KPDState = sending;
            }
            else{
                /* Update state. */
                KPDState = polling;
            }
            return;
           
        /* If a registered key press was determined to be valid, then send it
         * on the bus while it remained pressed. */    
        case sending:
            if(KPD){
                KPDState = disabled;
                keypadFlags.sending_f_first = 1; // Set flag upon exit
                return;
            }
            if(keypadFlags.sending_f_first){
                keypadFlags.sending_f_first = 0; // Clear flag
                
                /* Write key press data to pins. Do not set the data available
                * pin until the data has been latched onto the bus, or else
                * the data may not be stable/valid when RB1 is read. */
                LATA = dataOut & 0b00001111U;
                __SEND_DATA();
                __SET_DATA_AVAILABLE();
            }
            else{
                if(dataOut == readKeyboard()){
                    return;
                }
                else{
                    /* Clear the data latched on the bus, and set data pins to 
                     * high impedance. */
                     __CLEAR_BUS();
                    __IDLE_BUS();
                    
                    /* Update state. */
                    KPDState = polling;
                    keypadFlags.sending_f_first = 1; // Set flag upon exit
                }
            }
            return;
    }
}

inline void keypadRoutineUART(void){
    /* Main keypad routine, with key press data transmitted via the UART.
     *
     * Arguments: none
     *
     * Returns: none
     */
     
    /* Check if the keypad is supposed to be disabled. */
    if(KPD_UART){
        /* Transmitter will be held idle during this loop. */
        while(KPD_UART){ continue; }
    }
    
    /* If the keypad is not supposed to be disabled, check for a key press. */
    else{       
        /* Poll the keyboard for a key press. */
        dataOut = readKeyboard();

        /* Check if a key press is detected. */
        if (dataOut != 0xF0){
            /* Debounce switch press. If the same key is read a second time,
             * after 20 milliseconds and it is the same as before, it is assumed
             * valid. */
            __delay_ms(20);
            
            if (dataOut == readKeyboard()){
                /* Transmit the key press data via the UART. */
                uartTransmitBlocking(&dataOut, 1);

                /* Transmit key press data while key is still pressed. */
                while (dataOut == readKeyboard()){
                    uartTransmitBlocking(&dataOut, 1);
                    
                    /* If the keypad is disabled during a key press, then
                     * no UART transmissions while in this loop. */
                    if(KPD_UART){
                        /* Transmit end of data byte. */
                        uartTransmitBlocking(&dataEnd, 1);

                        while(KPD_UART){
                            continue;
                        }
                        return;
                    }
                }

                /* After key is released, transmit end of data byte. */
                uartTransmitBlocking(&dataEnd, 1);
            }
        }
    }
}
