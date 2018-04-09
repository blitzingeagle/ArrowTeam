#include "protocol_manager.h"

void use_protocol(enum PROTOCOL p) {
    switch(p) {
        case SPI:
            spiInit(4);
            break;
        case I2C:
            TRISCbits.TRISC3 = 1; // SCL (clock) pin
            TRISCbits.TRISC4 = 1; // SDA (data) pin
            SSPCON1 = 0b00101000; // Clear errors & enable the serial port in master mode
            SSPCON2 = 0b00000000; // Set entire I2C operation to idle
            break;
    }
    curr_protocol = p;
}
