#include "eep.h"

void eep_write_octet(unsigned char adr, unsigned char data) {
    while(EECON1bits.WR);   // Block until previous write is completed
    
    EEADR = adr;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.WREN  = 1;
    INTCONbits.GIE   = 0;
    // required sequence start
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    // required sequence end
    INTCONbits.GIE = 1;
    __delay_ms(100);
}

unsigned char eep_read_octet(unsigned char adr) {
    while(EECON1bits.WR);   // Block until previous write is completed
    
    EEADR = adr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.RD    = 1;
    __delay_ms(100);
    
    return EEDATA;
}

void eep_write(unsigned char adr, const unsigned char data[4]) {
    for(char i = 0; i < 4; i++) eep_write_octet(adr+i, data[i]);
}

void eep_read(unsigned char adr, unsigned char *data) {
    for(char i = 0; i < 4; i++) data[i] = eep_read_octet(adr+i);
}

void eep_write_long(unsigned char adr, unsigned long l) {
    unsigned char data[4];
    data[0] = l & 0xFF;
    data[1] = (l >> 8) & 0xFF;
    data[2] = (l >> 16) & 0xFF;
    data[3] = (l >> 24) & 0xFF;
    
    eep_write(adr, data);
}

unsigned long eep_read_long(unsigned char adr) {
    unsigned char data[4];
    eep_read(adr, data);
    
    return (unsigned long) data[0] | ((unsigned long) data[1] << 8) | ((unsigned long) data[2] << 16) | ((unsigned long) data[3] << 24);
}