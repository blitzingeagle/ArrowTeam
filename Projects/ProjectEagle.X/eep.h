/* 
 * File:   eep.h
 * Author: morris
 *
 * Created on March 6, 2018, 4:17 PM
 */

#ifndef EEP_H
#define	EEP_H

#include "config_bits.h"

void eep_write_octet(unsigned char adr, unsigned char data);
unsigned char eep_read_octet(unsigned char adr);

void eep_write(unsigned char adr, const unsigned char data[4]);
void eep_read(unsigned char adr, unsigned char *data);

#endif	/* EEP_H */

