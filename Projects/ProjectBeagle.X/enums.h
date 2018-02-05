/* 
 * File:   enums.h
 * Author: Daniil Orekhov
 *
 * Created on January 17, 2018, 5:35 PM
 */

#ifndef ENUMS_H
#define	ENUMS_H

enum machineMode {
    MODE_STANDBY,
    MODE_INPUT,
    MODE_RUNNING
};

enum inputMode {
    MODE_NO_INPUT,
    MODE_EMPTY_INPUT,
    MODE_INPUT_DESTINATION,
    MODE_INPUT_DIET,
    MODE_INPUT_DIET_NUM,
    MODE_INPUT_PROMPT,
    MODE_SHOW_INPUT
};

enum diet {
            // P/R
    R,      // 0/1
    F,      // 1/2
    L,      // 2/3
    RF,     // 3/4
    RL,     // 4/5
    FL,     // 5/6
    RRF,    // 6/7
    RRL,    // 7/8
    RFF,    // 8/9
    RLL,    // 9/10
    RFL,    // 10/11
    FFL,    // 11/12
    FLL,    // 12/13
    RRFL,   // 13/14
    RFFL,   // 14/15
    RFLL,   // 15/16
    RLLL,   // 16/17
    FLLL    // 17/18
};

static const char *DIETS[] = {
    "R", "F", "L", "RF", "RL", "FL", "RRF", "RRL", "RFF", "RLL", "RFL", "FFL",
    "FLL", "RRFL", "RFFL", "RFLL", "RLLL", "FLLL"
};

#endif	/* ENUMS_H */
