#include <string.h>

#include "program_states.h"
#include "protocol_manager.h"
#include "canvas.h"
#include "RTC.h"
#include "history.h"
#include "trie.h"

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

void FUNC_STATE_STANDBY(void) {
    printf("<1> to ");
    printf(program_status.operating ? "resume" : "start");
    __lcd_newline();
    printf("<2> for history ");
    __lcd_display_control(1, 1, 1);
}

void FUNC_STATE_PROMPT_COMPARTMENT_COUNT(void) {
    printf("# assembly steps");
    __lcd_newline();
    printf("(4-8) >");
}

void FUNC_STATE_PROMPT_FASTENER_SET(void) {
    program_status.buffer[0] = '\0';
    program_status.buffer_index = 0;
    program_status.trie_ptr = &fastener_trie.nodes[0];
    
    program_status.B = 0;
    program_status.N = 0;
    program_status.S = 0;
    program_status.W = 0;
    
    printf("Fastener set %d", program_status.compartment_count_index + 1);
    __lcd_newline();
    printf("Fasteners >");
    
    use_protocol(SPI);
    char num_sets = program_status.compartment_count;
    char set = program_status.compartment_count_index;
    program_status.sector = (num_sets <= 6) ? set + (set / (num_sets - 3)) : set;
    highlight_sector(program_status.sector);
            
    sprintf(Canvas.footer_text, "%d STEP ASSEMBLY", num_sets);
    update_footer();
    
    update_sheet_data(program_status.B, program_status.N, program_status.S, program_status.W);
}

void FUNC_STATE_PROMPT_FASTENER_SET_QUANTITY(void) {
    printf("Set %d: %s", program_status.compartment_count_index + 1, program_status.buffer);
    __lcd_newline();
    if(program_status.max_quantity == 1) printf("Qty (1) >");
    else printf("Qty (1-%d) >", program_status.max_quantity);
}

void FUNC_STATE_PREVIEW_FASTENER_SET(void) {
    printf("Confirm Set %d", program_status.compartment_count_index + 1);
    __lcd_newline();
    printf("%dx%s >", program_status.set_count_tmp, program_status.buffer);
}

void FUNC_STATE_CONFIRM_SETS(void) {
    printf("Confirm sets?");
    __lcd_newline();
    printf("%d step assembly", program_status.compartment_count);
}

void FUNC_STATE_REVIEW_SET(void) {
    printf("Set %d:", program_status.compartment_count_index + 1);
    __lcd_newline();
    printf("B:%d N:%d S:%d W:%d ", program_status.set_count[program_status.compartment_count_index][0], program_status.set_count[program_status.compartment_count_index][1], program_status.set_count[program_status.compartment_count_index][2], program_status.set_count[program_status.compartment_count_index][3]);
}

void FUNC_STATE_EXECUTE(void) {
    printf("Executing... ");
    __lcd_newline();
}

void FUNC_STATE_COMPLETION(void) {
    printf("Done: %d steps", program_status.compartment_count);
    __lcd_newline();
    printf("Time: %ld sec", program_status.time_elapsed);
    
    use_protocol(SPI);
    sprintf(Canvas.footer_text, "B:%d N:%d S:%d W:%d", program_status.overflow.B, program_status.overflow.N, program_status.overflow.S, program_status.overflow.W);
    update_footer();
}

void FUNC_STATE_HISTORY(void) {
    if(program_status.history.count) {
        for(unsigned char i = 0; i < program_status.history.count; i++) {
            lcd_set_cursor(i*8%16,i*8/16);
            struct tm *ptm = gmtime(&program_status.history.logs[(program_status.history.first_log+i)%MAX_LOGS].time_start);
            
            printf("%d %02d/%02d", i+1, ptm->tm_mon, ptm->tm_mday);
        }
    } else {
        printf("No logs present.");
        __lcd_newline();
    }
}

void FUNC_STATE_LOG_PAGE_1(void) {
    unsigned char i = program_status.history_index;
    struct tm *ptm = gmtime(&program_status.history.logs[(program_status.history.first_log+i)%MAX_LOGS].time_start);
    time_t time_start = program_status.history.logs[(program_status.history.first_log+i)%MAX_LOGS].time_start;
    time_t time_end = program_status.history.logs[(program_status.history.first_log+i)%MAX_LOGS].time_end;
    
    printf("%02d:%02d %d sec", ptm->tm_hour, ptm->tm_min, time_end-time_start);
    __lcd_newline();
    printf("%d step assembly    ", program_status.history.logs[i].num_sets);
    
    use_protocol(SPI);
    sprintf(Canvas.footer_text, "B:%d N:%d S:%d W:%d", program_status.overflow.B, program_status.overflow.N, program_status.overflow.S, program_status.overflow.W);
    update_footer();
}

void FUNC_STATE_SET_TIME(void) {
    unsigned char *time = program_status.time;
    
    __lcd_home();
    printf(program_status.edit_time_idx ? "  [ %02x/%02x/%02x ]  " : "    %02x/%02x/%02x    ", time[6],time[5],time[4]); // Print date in YY/MM/DD
    __lcd_newline();
    printf(program_status.edit_time_idx ? "  [ %02x/%02x/%02x ]  " : "    %02x/%02x/%02x    ", time[2],time[1],time[0]); // HH:MM:SS
//    if(program_status.edit_time_idx) {
//        __lcd_home();
//        printf("  [ %02x/%02x/%02x ]  ", time[6],time[5],time[4]); // Print date in YY/MM/DD
//        __lcd_newline();
//        printf("  [ %02x:%02x:%02x ]  ", time[2],time[1],time[0]); // HH:MM:SS
//    } else {
//        __lcd_home();
//        printf("    %02x/%02x/%02x    ", time[6],time[5],time[4]); // Print date in YY/MM/DD
//        __lcd_newline();
//        printf("    %02x:%02x:%02x    ", time[2],time[1],time[0]); // HH:MM:SS
//    }
    
    __lcd_display_control(1,1,1);
    switch(program_status.edit_time_idx) {
        case 1: lcd_set_cursor(4,0); break;
        case 2: lcd_set_cursor(5,0); break;
        case 3: lcd_set_cursor(7,0); break;
        case 4: lcd_set_cursor(8,0); break;
        case 5: lcd_set_cursor(10,0); break;
        case 6: lcd_set_cursor(11,0); break;
        case 7: lcd_set_cursor(4,1); break;
        case 8: lcd_set_cursor(5,1); break;
        case 9: lcd_set_cursor(7,1); break;
        case 10: lcd_set_cursor(8,1); break;
        case 11: lcd_set_cursor(10,1); break;
        case 12: lcd_set_cursor(11,1); break;
        default: __lcd_display_control(1,0,0);
    }
}

void init_program_states(void) {
    program_state = STATE_STANDBY;
    PROG_FUNC[STATE_STANDBY] = FUNC_STATE_STANDBY;
    PROG_FUNC[STATE_PROMPT_COMPARTMENT_COUNT] = FUNC_STATE_PROMPT_COMPARTMENT_COUNT;
    PROG_FUNC[STATE_PROMPT_FASTENER_SET] = FUNC_STATE_PROMPT_FASTENER_SET;
    PROG_FUNC[STATE_PROMPT_FASTENER_SET_QUANTITY] = FUNC_STATE_PROMPT_FASTENER_SET_QUANTITY;
    PROG_FUNC[STATE_PREVIEW_FASTENER_SET] = FUNC_STATE_PREVIEW_FASTENER_SET;
    PROG_FUNC[STATE_CONFIRM_SETS] = FUNC_STATE_CONFIRM_SETS;
    PROG_FUNC[STATE_REVIEW_SET] = FUNC_STATE_REVIEW_SET;
    PROG_FUNC[STATE_EXECUTE] = FUNC_STATE_EXECUTE;
    PROG_FUNC[STATE_COMPLETION] = FUNC_STATE_COMPLETION;
    PROG_FUNC[STATE_HISTORY] = FUNC_STATE_HISTORY;
    PROG_FUNC[STATE_LOG_PAGE_1] = FUNC_STATE_LOG_PAGE_1;
    PROG_FUNC[STATE_SET_TIME] = FUNC_STATE_SET_TIME;
    
    init_fastener_trie();
    program_status.compartment_count_index = 0;
    program_status.buffer_index = 0;
    program_status.trie_ptr = &fastener_trie.nodes[0];
    program_status.operating = 0;
}

void reset_fastener_prompt() {
    // Reset the B, N, S, and W for each compartment to 0
    program_status.B = 0;
    program_status.N = 0;
    program_status.S = 0;
    program_status.W = 0;
    // Fill buffer with null terminators
    memset(program_status.buffer, '\0', 8);
    program_status.buffer_index = 0;
}

void program_states_interrupt(unsigned char key) {
    unsigned char state_changed = 1;

    switch(program_state) {
        case STATE_STANDBY: // Key pressed on standby mode
            if(key == '1') {
                program_state = program_status.operating ? STATE_EXECUTE : STATE_PROMPT_COMPARTMENT_COUNT; // Proceed to prompt for number of compartments
            } else if(key == '2') {
                program_state = STATE_HISTORY;
            } else if(key == '3') {
                program_status.edit_time_idx = 0;
                program_state = STATE_SET_TIME;
            }
            break;
        case STATE_PROMPT_COMPARTMENT_COUNT: // Key pressed while prompting for compartment count
            if('4' <= key && key <= '8') { // Check if input admissible
                program_status.compartment_count = key - '0'; // Store compartment count
                program_status.compartment_count_index = 0; // Start compartment index at 0
                memset(program_status.set_count, 0, sizeof(program_status.set_count[0][0]) * 8 * 4);
                
                reset_fastener_prompt();
                program_state = STATE_PROMPT_FASTENER_SET; // Proceed to prompt for fastener set contents
            } else if(key == '*') {
                program_state = STATE_STANDBY;
            }
            break;
        case STATE_PROMPT_FASTENER_SET: // Key pressed while prompting for fastener set contents
            if('A' <= key && key <= 'Z') { // Check if input is uppercase alphabet
                char leaf_index = trie_node_leaf_index(key);
                if(leaf_index < 4) {
                    program_status.buffer[program_status.buffer_index++] = key;
                    putch(key);
                    switch(key) {
                        case 'B': program_status.B++; break;
                        case 'N': program_status.N++; break;
                        case 'S': program_status.S++; break;
                        case 'W': program_status.W++; break;
                    }
                    program_status.trie_ptr = program_status.trie_ptr->children[leaf_index];
                }
                state_changed = 0;
            } else if(key == '*' && program_status.buffer_index > 0) {
                lcd_shift_cursor(1, 0);
                putch(' ');
                lcd_shift_cursor(1, 0);
                char x = program_status.buffer[--program_status.buffer_index];
                switch(x) {
                    case 'B': program_status.B--; break;
                    case 'N': program_status.N--; break;
                    case 'S': program_status.S--; break;
                    case 'W': program_status.W--; break;
                }
                program_status.buffer[program_status.buffer_index] = '\0';
                program_status.trie_ptr = program_status.trie_ptr->parent;
                state_changed = 0;
            } else if(key == '#' && program_status.buffer_index > 0) {
                if(strcmp(program_status.buffer,"BB") != 0 && strcmp(program_status.buffer,"BNN") != 0) {
                    program_status.max_quantity = 4;
                    if(program_status.B > 0) program_status.max_quantity = min(4/(program_status.B + program_status.N + program_status.S + program_status.W), 2/program_status.B);
                    if(program_status.N > 0) program_status.max_quantity = min(program_status.max_quantity, 3/program_status.N);
                    if(program_status.S > 0) program_status.max_quantity = min(program_status.max_quantity, 2/program_status.S);
                    if(program_status.W > 0) program_status.max_quantity = min(program_status.max_quantity, 4/program_status.W);
                    
                    program_status.set_enum[program_status.compartment_count_index] = program_status.trie_ptr - TRIE_ROOT_ADR;
                    
                    program_state = STATE_PROMPT_FASTENER_SET_QUANTITY;
                } else {
                    state_changed = 0;
                }
            } else {
                state_changed = 0;
            }
            use_protocol(SPI);
            update_sheet_data(program_status.B, program_status.N, program_status.S, program_status.W);
            break;
        case STATE_PROMPT_FASTENER_SET_QUANTITY:
            if('1' <= key && key <= ('0' + program_status.max_quantity)) {
                program_status.set_qty[program_status.compartment_count_index] = key - '0';
                program_status.set_count_tmp = key - '0';
                
                program_state = STATE_PREVIEW_FASTENER_SET;
            } else {
                state_changed = 0;
            }
            break;
        case STATE_PREVIEW_FASTENER_SET:
            if(key == '#') {
                while(program_status.trie_ptr != program_status.trie_ptr->parent) {
                    char c = program_status.trie_ptr->letter;
                    char p = (c <= 'N') ? ((c == 'B') ? 0 : 1) : ((c == 'S') ? 2 : 3);
                    program_status.set_count[program_status.compartment_count_index][p] += program_status.set_count_tmp;
                    program_status.trie_ptr = program_status.trie_ptr->parent;
                }
                if(++program_status.compartment_count_index < program_status.compartment_count) {
                    reset_fastener_prompt();
                    program_state = STATE_PROMPT_FASTENER_SET;
                } else {
                    program_state = STATE_CONFIRM_SETS;
                }
            } else if(key == '*') {
                reset_fastener_prompt();
                program_state = STATE_PROMPT_FASTENER_SET;
            } else {
                state_changed = 0;
            }
            break;
        case STATE_CONFIRM_SETS:
            if(key == '*') {
                program_state = STATE_PROMPT_COMPARTMENT_COUNT;
            } else if(key == '#') {
                program_status.operating = 1;
                program_state = STATE_EXECUTE;
            } else if('1' <= key && key <= '0'+program_status.compartment_count) {
                program_status.compartment_count_index = key - '1';
                program_state = STATE_REVIEW_SET;
            } else {
                state_changed = 0;
            }
            break;
        case STATE_REVIEW_SET:
            program_state = STATE_CONFIRM_SETS;
            break;
        case STATE_COMPLETION:
            // TODO
            if('1' <= key && key <= '8') {
                program_status.sector = key - '1';
                
                unsigned char sector = program_status.sector;
                unsigned char num_sets = program_status.compartment_count;
                unsigned char set = (num_sets <= 6) ? sector - (sector+1)/(num_sets-2) : sector;
                set = (((num_sets <= 6) ? set + (set / (num_sets - 3)) : set) == sector) ? set : 8;
                
                use_protocol(SPI);
                highlight_sector(program_status.sector);
                
                if(set < 8) {
                    update_sheet_data(program_status.set_count[set][0], program_status.set_count[set][1], program_status.set_count[set][2], program_status.set_count[set][3]);
                } else {
                    update_sheet_data(0, 0, 0, 0);
                }
                state_changed = 0;
            } else {
                program_state = STATE_STANDBY;
            }
            break;
        case STATE_HISTORY:
            if('1' <= key && key <= '9') {
                program_status.history_index = key - '1';
                program_state = (program_status.history_index < program_status.history.count) ? STATE_LOG_PAGE_1 : STATE_STANDBY;
                
                if(program_status.history_index < program_status.history.count) {
                    program_status.history_index = (program_status.history.first_log+program_status.history_index)%MAX_LOGS;
                    program_state = STATE_LOG_PAGE_1;
                } else {
                    program_state = STATE_STANDBY;
                }
                
            } else {
                program_state = STATE_STANDBY;
            }
            break;
        case STATE_SET_TIME:
            if(key == '*') {
                if(program_status.edit_time_idx == 0) {
                    TMR0IE = 1;
                    program_state = STATE_STANDBY;
                } else {
                    program_status.edit_time_idx--;
                    if(program_status.edit_time_idx == 0) {
                        TMR0IE = 1;
                    } else {
                        TMR0IE = 0;
                    }
                }
            } else if(key == '#') {
                if(program_status.edit_time_idx == 0) {
                    TMR0IE = 0;
                    program_status.edit_time_idx++;
                } else if(program_status.edit_time_idx == 13) {
                    use_protocol(I2C);
                    RTC_set_time(program_status.time);
                    TMR0IE = 1;
                    program_state = STATE_STANDBY;
                } else {
                    TMR0IE = 0;
                    program_status.edit_time_idx++;
                }
            } else if('0' <= key && key <= '9') {
                TMR0IE = 0;
                unsigned char digit = key - '0';
                unsigned char *time = program_status.time;
                switch(program_status.edit_time_idx) {
                    case 1: time[6] = (0x0F & time[6]) | (0xF0 & (digit << 4)); break;
                    case 2: time[6] = (0x0F & digit) | (0xF0 & time[6]); break;
                    case 3: time[5] = (0x0F & time[5]) | (0xF0 & (digit << 4)); break;
                    case 4: time[5] = (0x0F & digit) | (0xF0 & time[5]); break;
                    case 5: time[4] = (0x0F & time[4]) | (0xF0 & (digit << 4)); break;
                    case 6: time[4] = (0x0F & digit) | (0xF0 & time[4]); break;
                    case 7: time[2] = (0x0F & time[2]) | (0xF0 & (digit << 4)); break;
                    case 8: time[2] = (0x0F & digit) | (0xF0 & time[2]); break;
                    case 9: time[1] = (0x0F & time[1]) | (0xF0 & (digit << 4)); break;
                    case 10: time[1] = (0x0F & digit) | (0xF0 & time[1]); break;
                    case 11: time[0] = (0x0F & time[0]) | (0xF0 & (digit << 4)); break;
                    case 12: time[0] = (0x0F & digit) | (0xF0 & time[0]); break;
                }
                if(program_status.edit_time_idx < 13) {
                    program_status.edit_time_idx++;
                }
                
            }
            break;
        default:
            program_state = STATE_STANDBY;
    }

    if(state_changed) {
        __lcd_clear();
        __lcd_home();
        PROG_FUNC[program_state]();
    }
}

char trie_node_leaf_index(char c) {
    for(int i = 0; i < 4; i++) {
        if(program_status.trie_ptr->children[i] != NULL && program_status.trie_ptr->children[i]->letter == c)
            return i;
    }
    return 4;
}
