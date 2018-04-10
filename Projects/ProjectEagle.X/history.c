#include "history.h"

#include "eep.h"

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

void read_history(struct History *history) {
    if(eep_read_octet(0) == LOG_BEGIN_ADR) {
        history->write_index = eep_read_octet(1);
        history->count = eep_read_octet(2);
        history->first_log = eep_read_octet(3);
        for(unsigned char i = 0; i < MAX_LOGS; i++) {
            unsigned char adr = LOG_BEGIN_ADR + i * LOG_SIZE;
            history->logs[i].time_start = eep_read_long(adr);
            history->logs[i].time_end = eep_read_long(adr+4);
            for(unsigned char j = 0; j < 8; j++) {
                unsigned char enc = eep_read_octet(adr+8+j);
                history->logs[i].set_enum[j] = (enc & 0b11111);
                history->logs[i].qty[j] = (enc >> 5) & 0b111;
            }
            eep_read(adr+16, history->logs[i].overflow);
            
            history->logs[i].num_sets = eep_read_octet(SETS_BEGIN_ADR + i);
        }
    } else {
        history->write_index = 0;
        history->count = 0;
        history->first_log = 0;
    }
}

void write_history(struct History history) {
    eep_write_octet(0, LOG_BEGIN_ADR);
    eep_write_octet(1, history.write_index);
    eep_write_octet(2, history.count);
    eep_write_octet(3, history.first_log);
    for(unsigned char i = 0; i < MAX_LOGS; i++) {
        unsigned char adr = LOG_BEGIN_ADR + i * LOG_SIZE;
        eep_write_long(adr, history.logs[i].time_start);
        eep_write_long(adr+4, history.logs[i].time_end);
        for(unsigned char j = 0; j < 8; j++) {
            unsigned char enc = ((history.logs[i].qty[j] & 0b111) << 5) | (history.logs[i].set_enum[j] & 0b11111);
            eep_write_octet(adr+8+j, enc);
        }
        eep_write(adr+16, history.logs[i].overflow);
        
        eep_write_octet(SETS_BEGIN_ADR + i, history.logs[i].num_sets);
    }
}

void add_log(struct Log log, struct History *history) {
    history->logs[history->write_index++] = log;
    history->write_index %= MAX_LOGS;
    if(history->count == MAX_LOGS) {
        history->first_log = (history->first_log + 1) % MAX_LOGS;
    } else {
        history->count++;
    }
}
