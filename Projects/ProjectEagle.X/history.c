#include "history.h"

#include "eep.h"

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

void read_history(struct History *history) {
    unsigned char mem[4];
    if((mem[0] = eep_read_octet(0)) == LOG_BEGIN_ADR && \
            ((mem[1] = eep_read_octet(1)) - mem[0]) % LOG_SIZE == 0) {
        history->count = eep_read_octet(3);
        for(unsigned char i = 0; i < MAX_LOGS; i++) {
            unsigned char adr = LOG_BEGIN_ADR + i * LOG_SIZE;
            history->logs[i].time_start = eep_read_long(adr);
            history->logs[i].time_end = eep_read_long(adr+4);
        }
    }
}

void write_history(struct History history) {
    eep_write_octet(0, LOG_BEGIN_ADR);
    eep_write_octet(1, LOG_BEGIN_ADR + history.count * LOG_SIZE);
    eep_write_octet(2, LOG_BEGIN_ADR + history.write_index * LOG_SIZE);
    eep_write_octet(3, history.count);
    for(unsigned char i = 0; i < MAX_LOGS; i++) {
        unsigned char adr = LOG_BEGIN_ADR + i * LOG_SIZE;
        eep_write_long(adr, history.logs[i].time_start);
        eep_write_long(adr+4, history.logs[i].time_end);
    }
}

void add_log(struct Log log, struct History *history) {
    history->logs[history->write_index++] = log;
    history->write_index %= MAX_LOGS;
    history->count = min(history->count+1, MAX_LOGS);
}

//#include "RTC.h"
//#include "protocol_manager.h"
//
//struct History make_history(char set_count[][4], char steps) {
//    struct History history;
//    
//    use_protocol(I2C);
//    RTC_read_time(history.time);
//    
//    for(int i = 0; i < 8; i++) {
//        history.sets[i] = 0;
//        for(int j = 0; j < 4; j++) {
//            history.sets[i] = history.sets[i] * 5 + set_count[i][j]; 
//        }
//    }
//    
//    history.steps = steps;
//    
//    return history;
//}
