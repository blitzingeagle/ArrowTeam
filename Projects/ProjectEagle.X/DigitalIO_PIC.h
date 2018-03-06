/* 
 * File:   DigitalIO_PIC.h
 * Author: morris
 *
 * Created on February 27, 2018, 8:30 AM
 */

#ifndef DIGITALIO_PIC_H
#define	DIGITALIO_PIC_H

#include "config_bits.h"

#define TURNTABLE_CW LATAbits.LATA0
#define TURNTABLE_CCW LATAbits.LATA1
#define LEADSCREW_CW LATCbits.LATC0
#define LEADSCREW_CCW LATCbits.LATC1

#define ROTARY_ENCODER PORTAbits.PORTA2
#define OPTICAL_SENSOR_1 PORTAbits.PORTA3
//#define OPTICAL_SENSOR_2 PORTAbits.PORTA4

unsigned char CMPT1 = 0;

void orient_container(void);
void clamp_up(void);
void clamp_down(void);
void rotate_to_compartment(unsigned char cmpt);

void DigitalIO_interrupt(void);

#endif	/* DIGITALIO_PIC_H */

