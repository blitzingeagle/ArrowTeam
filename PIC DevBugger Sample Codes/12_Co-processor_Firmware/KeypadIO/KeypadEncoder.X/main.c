/*
 * File:   main.c
 * Author: Tyler Gamvrelis
 *
 * Created on July 28, 2017, 6:37 PM
 */


#include <xc.h>
#include "machineConfig.h"
#include "Keypad.h"

void main(void) {
    machineConfig();
    
    while(1){
        keypadRoutine();
    }
}
