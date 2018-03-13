/* 
 * File:   arduino_cmd.h
 * Author: morris
 *
 * Created on February 26, 2018, 11:16 AM
 */

#ifndef ARDUINO_CMD_H
#define	ARDUINO_CMD_H

const int word_len = 4;
const char* cmd_begin = "CMDB";
const char* cmd_end = "CMDE";
const char* template_fastener_data = "CMDB{\"cmd\":\"f_data\",\"data\":[%d,%d,%d,%d,%d,%d,%d,%d]}CMDE";

const char* template_gate_drop = "CMDB{\"cmd\":\"gate_drop\"}CMDE";
const char* template_gate_return = "CMDB{\"cmd\":\"gate_return\"}CMDE";
const char* template_gate_overflow = "CMDB{\"cmd\":\"gate_overflow\"}CMDE";

const char* template_load_compartment = "CMDB{\"cmd\":\"load_cmpt\",\"data\":%d}CMDE";

void arduino_ping(void);

void arduino_begin_cmd(void);
void arduino_end_cmd(void);

void arduino_send_gate_drop(void);
void arduino_send_gate_return(void);
void arduino_send_gate_overflow(void);
void arduino_send_fastener_data(char set_count[][4]);
void arduino_send_load_compartment(char x);

void TX_interface(void);
void RX_interface(void);

#endif	/* ARDUINO_CMD_H */

