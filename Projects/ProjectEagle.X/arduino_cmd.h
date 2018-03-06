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
const char* template_fastener_data = "{\"cmd\":\"f_data\",\"data\":[%d,%d,%d,%d,%d,%d,%d,%d]}";

const char* template_gate_drop = "{\"cmd\":\"gate_drop\"}";
const char* template_gate_return = "{\"cmd\":\"gate_return\"}";
const char* template_gate_overflow = "{\"cmd\":\"gate_overflow\"}";

const char* template_load_compartment = "{\"cmd\":\"load_cmpt\",\"data\":%d}";

void arduino_ping(void);
void arduino_send_gate_drop(void);
void arduino_send_gate_return(void);
void arduino_send_gate_overflow(void);
void arduino_send_fastener_data(char set_count[][4]);
void arduino_send_load_compartment(char x);

#endif	/* ARDUINO_CMD_H */

