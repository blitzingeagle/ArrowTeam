/* 
 * File:   history.h
 * Author: morris
 *
 * Created on February 27, 2018, 12:24 AM
 */

#ifndef HISTORY_H
#define	HISTORY_H

struct History {
    unsigned char time[7];
    unsigned char sets[8];
    unsigned char steps;
};

struct History make_history(char set_count[][4], char steps);

#endif	/* HISTORY_H */

