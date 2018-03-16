/* 
 * File:   DigitalIO_PIC.h
 * Author: morris
 *
 * Created on February 27, 2018, 8:30 AM
 */

#ifndef DIGITALIO_PIC_H
#define	DIGITALIO_PIC_H

#include "config_bits.h"

#define TURNTABLE_CW() LATAbits.LATA3 = 1; LATAbits.LATA4 = 0; 
#define TURNTABLE_CCW() LATAbits.LATA3 = 0; LATAbits.LATA4 = 1;
#define TURNTABLE_STOP() LATAbits.LATA3 = 0; LATAbits.LATA4 = 0;
#define LEADSCREW_CW LATCbits.LATC0
#define LEADSCREW_CCW LATCbits.LATC1

#define ROTARY_ENCODER PORTAbits.RA2
#define OPTICAL_SENSOR_1 PORTAbits.RA0
//#define OPTICAL_SENSOR_2 PORTAbits.PORTA1

double cmpt1 = 0;
double rotary_angle = 0;
unsigned char rotary_prev = 0;

void orient_container(void);
void clamp_up(void);
void clamp_down(void);
void rotate_to_compartment(unsigned char cmpt);


#endif	/* DIGITALIO_PIC_H */

