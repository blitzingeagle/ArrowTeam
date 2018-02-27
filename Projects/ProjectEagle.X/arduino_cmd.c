#include "arduino_cmd.h"

#include <stdio.h>
#include <string.h>

#include "UART_PIC.h"

void arduino_ping(void) {
    unsigned char arr[1] = {'H'};
    uartTransmitIT(arr, 1);
    uartReceiveIT(1);
}

void arduino_send_fastener_data(void) {
    char buffer[256];
    sprintf(buffer, template_fastener_data, 1, 2, 3, 4, 5, 6, 7, 8);
    
    uartTransmitIT(cmd_begin, word_len);
    uartTransmitIT(buffer, strlen(buffer));
    uartTransmitIT(cmd_end, word_len);
}
