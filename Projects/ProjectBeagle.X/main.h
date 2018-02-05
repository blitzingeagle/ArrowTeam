/* 
 * File:   main.h
 * Author: Daniil Orekhov
 *
 * Created on January 16, 2018, 3:37 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include "configBits.h"
#include "enums.h"
#include "inputHandler.h"
#include "I2C.h"

typedef struct {
    
    int destination;
    enum diet dietType;
    int dietNum; //number of types the diet is repeated
    
} Operation;

void enterStandby(void);

#endif	/* MAIN_H */

