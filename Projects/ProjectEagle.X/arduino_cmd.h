/* 
 * File:   arduino_cmd.h
 * Author: morris
 *
 * Created on February 26, 2018, 11:16 AM
 */

#ifndef ARDUINO_CMD_H
#define	ARDUINO_CMD_H

const int word_len = 4;
//const char* cmd_begin = "CMDB";
//const char* cmd_end = "CMDE";
//const char* template_orient = "CMDB{\"cmd\":\"orient_tt\"}CMDE";
const char* template_fastener_data = "CMDB{\"cmd\":\"f_data\",\"data\":[%d,%d,%d,%d,%d,%d,%d,%d,%d]}CMDE";

//const char* template_gate_drop = "CMDB{\"cmd\":\"gate_drop\"}CMDE";
//const char* template_gate_return = "CMDB{\"cmd\":\"gate_return\"}CMDE";
//const char* template_gate_overflow = "CMDB{\"cmd\":\"gate_overflow\"}CMDE";

const char* template_load_compartment = "CMDB{\"cmd\":\"load_cmpt\",\"data\":%d}CMDE";

void arduino_ping(void);

void arduino_send_fastener_data(char set_count[][4], unsigned char steps);

#endif	/* ARDUINO_CMD_H */

