#include "history.h"

#include "RTC.h"

struct History make_history(char **set_count, char steps) {
    struct History history;
    RTC_read_time(history.time);
    
    for(int i = 0; i < 8; i++) {
        history.sets[i] = 0;
        for(int j = 0; j < 4; j++) {
            history.sets[i] = history.sets[i] * 10 + set_count[i][j]; 
        }
    }
    
    return history;
}
