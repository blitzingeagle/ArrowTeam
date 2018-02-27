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

void arduino_ping(void);
void arduino_send_fastener_data(void);

#endif	/* ARDUINO_CMD_H */

