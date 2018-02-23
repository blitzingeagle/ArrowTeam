/* 
 * File:   protocol_manager.h
 * Author: morris
 *
 * Created on February 23, 2018, 4:59 PM
 */

#ifndef PROTOCOL_MANAGER_H
#define	PROTOCOL_MANAGER_H

#include "SPI_PIC.h"
#include "I2C.h"

enum PROTOCOL {
    SPI, I2C
};

void use_protocol(enum PROTOCOL p);

#endif	/* PROTOCOL_MANAGER_H */

