/* 
 * File:   Keypad.h
 * Author: Tyler Gamvrelis
 *
 * Created on July 28, 2017, 12:30 PM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

/********************************** Includes **********************************/
#include <xc.h>
#include "configBits.h"
#include "UART_PIC.h"

/*********************************** Macros ***********************************/
/* Disables the data bus and data available pin by setting them to high
 * impedance (input). */
#define __DISABLE_BUS() TRISA = 0xFF

/* Clears the data available pin. */
#define __CLEAR_DATA_AVAILABLE()    LATAbits.LATA5 = 0

/* Sets the data available pin. */
#define __SET_DATA_AVAILABLE()  LATAbits.LATA5 = 1

/* Bus idle mode; data available asserted as 0 while data available are high
 * impedance (input). */
#define __IDLE_BUS()  TRISA = 0b11011111

/* Write data to bus. */
#define __SEND_DATA()   TRISA = 0b11010000

/* Clear bus. */
#define __CLEAR_BUS()   LATA = 0x00

/*********************************** Defines **********************************/
/* Definition of pin used to disable keypad (1 = disabled, 0 = enabled). */
#define KPD PORTAbits.RA4
#define KPD_UART    0   /* UNIMPLEMENTED. */

typedef struct{
    unsigned char f_disabled_to_enabled;
    unsigned char f_enabled_to_disabled;
    unsigned char sending_f_first;
}keypadFlags_t;

enum keypadStates{
    disabled,
    polling,
    debouncing,
    sending
};

enum keypadStates KPDState = polling;
keypadFlags_t keypadFlags;

/****************************** Private Variables *****************************/
static unsigned char dataOut;
static unsigned char dataEnd = 0xF0; // For UART

/************************ Private Function prototypes *************************/
unsigned char readKeyboard(void);

/****************************** Public Interfaces *****************************/
void keypadRoutine_StateBased(void);
inline void keypadRoutine(void);
inline void keypadRoutineUART(void);

#endif	/* UART_PIC_H */
