/* 
 * File:   RTC.h
 * Author: morris
 *
 * Created on February 24, 2018, 9:53 AM
 */

#ifndef RTC_H
#define	RTC_H

#include <time.h>

#define __bcd_to_num(bcd) (bcd & 0x0F) + ((bcd & 0xF0)>>4)*10

void RTC_set_time(char *setTime);
void RTC_read_time(char *time);

time_t get_epoch_time(char *time);

#endif	/* RTC_H */

