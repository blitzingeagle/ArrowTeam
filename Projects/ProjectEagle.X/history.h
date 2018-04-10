/* 
 * File:   history.h
 * Author: morris
 *
 * Created on February 27, 2018, 12:24 AM
 */

#ifndef HISTORY_H
#define	HISTORY_H

//struct History {
//    unsigned char time[7];
//    unsigned char sets[8];
//    unsigned char steps;
//};
//
//struct History make_history(char set_count[][4], char steps);

#include <time.h>

#define MAX_LOGS 4

#define SETS_BEGIN_ADR 4
#define LOG_BEGIN_ADR 12
#define LOG_SIZE 20

struct Log {
    time_t time_start, time_end;
    unsigned char qty[8];
    unsigned char set_enum[8];
    unsigned char overflow[4];
    unsigned char num_sets;
};

struct History {
    unsigned char count;
    unsigned char write_index;
    unsigned char first_log;
    struct Log logs[MAX_LOGS];
};

void read_history(struct History *history);
void write_history(struct History history);

void add_log(struct Log log, struct History *history);

#endif	/* HISTORY_H */