#include "arduino_cmd.h"

#include <stdio.h>
#include <string.h>

#include "UART_PIC.h"

void arduino_ping(void) {
    unsigned char arr[1] = {'H'};
    uartTransmitIT(arr, 1);
    uartReceiveIT(1);
}

void arduino_send_gate_drop(void) {
    char buffer[256];
    sprintf(buffer, template_gate_drop);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}

void arduino_send_gate_return(void) {
    char buffer[256];
    sprintf(buffer, template_gate_return);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}

void arduino_send_gate_overflow(void) {
    char buffer[256];
    sprintf(buffer, template_gate_overflow);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}

void arduino_send_fastener_data(char set_count[][4]) {
    char buffer[256];
    unsigned char sets[8];
    for(int i = 0; i < 8; i++) {
        sets[i] = set_count[i][0];
        for(int j = 0; j < 4; j++) {
            sets[i] = sets[i] * 5 + set_count[i][j]; 
        }
    }
    sprintf(buffer, template_fastener_data, sets[0], sets[1], sets[2], sets[3], sets[4], sets[5], sets[6], sets[7]);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}

void arduino_send_load_compartment(char x) {
    char buffer[256];
    sprintf(buffer, template_load_compartment, x);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}
