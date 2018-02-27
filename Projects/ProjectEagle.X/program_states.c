#include <stdbool.h>
#include <string.h>

#include "program_states.h"
#include "RTC.h"
#include "history.h"

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

void FUNC_STATE_STANDBY(void) {
    if(program_status.operating) {
        printf("<1> to resume");
    } else {
        printf("<1> to start");
    }
    __lcd_newline();
    printf("<2> for history");
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
}

void FUNC_STATE_PROMPT_FASTENER_SET_QUANTITY(void) {
    printf("Set %d: %s", program_status.compartment_count_index + 1, program_status.buffer);
    __lcd_newline();
    if(program_status.max_quantity == 1) {
        printf("Qty (1) >");
    } else {
        printf("Qty (1-%d) >", program_status.max_quantity);
    }
    
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
    printf("Executing...");
    __lcd_newline();
    
//    TRISCbits.TRISC0 = 0;
//    LATCbits.LATC0 = 1;
    
//    PORTCbits.PORTC0;
}

void FUNC_STATE_HISTORY(void) {
    if(program_status.history_cnt) {
        for(int i = 0; i < program_status.history_cnt; i++) {
            lcd_set_cursor(i*8%16,i*8/16);
            printf("%d %02x/%02x", i+1, program_status.history[i].time[5], program_status.history[i].time[4]);
        }
    } else {
        printf("No logs present.");
        __lcd_newline();
    }
}

void FUNC_STATE_HISTORY_PAGE_1(void) {
    char i = program_status.history_index;
    printf("%02x/%02x %02x:%02x:%02x", program_status.history[i].time[5], program_status.history[i].time[4], program_status.history[i].time[2], program_status.history[i].time[1], program_status.history[i].time[0]);
    __lcd_newline();
    printf("%d step assembly", program_status.history[i].steps);
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
    PROG_FUNC[STATE_HISTORY] = FUNC_STATE_HISTORY;
    PROG_FUNC[STATE_HISTORY_PAGE_1] = FUNC_STATE_HISTORY_PAGE_1;
    
    init_fastener_trie();
    program_status.compartment_count_index = 0;
    program_status.buffer_index = 0;
    program_status.trie_ptr = &fastener_trie.nodes[0];
    program_status.history_cnt = 0;
    program_status.operating = false;
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

#include "GLCD_PIC.h"
#include "px_ascii.h"
#include "protocol_manager.h"

void program_states_interrupt(unsigned char key) {
    bool state_changed = true;

    switch(program_state) {
        case STATE_STANDBY: // Key pressed on standby mode
            if(key == '1') {
                program_state = program_status.operating ? STATE_EXECUTE : STATE_PROMPT_COMPARTMENT_COUNT; // Proceed to prompt for number of compartments
            } else if(key == '2') {
                program_state = STATE_HISTORY;
            }
            break;
        case STATE_PROMPT_COMPARTMENT_COUNT: // Key pressed while prompting for compartment count
            if('4' <= key && key <= '8') { // Check if input admissible
                program_status.compartment_count = key - '0'; // Store compartment count
                program_status.compartment_count_index = 0; // Start compartment index at 0
                memset(program_status.set_count, 0, sizeof(program_status.set_count[0][0]) * 8 * 4);
                reset_fastener_prompt();
                program_state = STATE_PROMPT_FASTENER_SET; // Proceed to prompt for fastener set contents
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
                state_changed = false;
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
                state_changed = false;
            } else if(key == '#' && program_status.buffer_index > 0) {
                if(strcmp(program_status.buffer,"BB") != 0 && strcmp(program_status.buffer,"BNN") != 0) {
                    program_status.max_quantity = 4;
                    if(program_status.B > 0) program_status.max_quantity = min(4/(program_status.B + program_status.N + program_status.S + program_status.W), 2/program_status.B);
                    if(program_status.N > 0) program_status.max_quantity = min(program_status.max_quantity, 3/program_status.N);
                    if(program_status.S > 0) program_status.max_quantity = min(program_status.max_quantity, 2/program_status.S);
                    if(program_status.W > 0) program_status.max_quantity = min(program_status.max_quantity, 4/program_status.W);

                    program_state = STATE_PROMPT_FASTENER_SET_QUANTITY;
                } else {
                    state_changed = false;
                }
            } else {
                state_changed = false;
            }
            use_protocol(SPI);
            char buffer[22];
            glcdDrawRectangle(0, GLCD_SIZE_VERT, 114, 128, WHITE);
            sprintf(buffer, "B:%d N:%d S:%d W:%d Tot:%d", program_status.B, program_status.N, program_status.S, program_status.W, program_status.buffer_index);
            print_px_string(1, 114, buffer);
            use_protocol(I2C);
            break;
        case STATE_PROMPT_FASTENER_SET_QUANTITY:
            if('1' <= key && key <= ('0' + program_status.max_quantity)) {
                program_status.set_count_tmp = key - '0';
                program_state = STATE_PREVIEW_FASTENER_SET;
            } else {
                state_changed = false;
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
                state_changed = false;
            }
            break;
        case STATE_CONFIRM_SETS:
            if(key == '*') {
                program_state = STATE_PROMPT_COMPARTMENT_COUNT;
            } else if(key == '#') {
                program_status.operating = true;
                program_state = STATE_EXECUTE;
            } else if('1' <= key && key <= '0'+program_status.compartment_count) {
                program_status.compartment_count_index = key - '1';
                program_state = STATE_REVIEW_SET;
            } else {
                state_changed = false;
            }
            break;
        case STATE_REVIEW_SET:
            program_state = STATE_CONFIRM_SETS;
            break;
        case STATE_HISTORY:
            if('1' <= key && key <= '9') {
                program_status.history_index = key - '1';
                if(program_status.history_index < program_status.history_cnt)
                    program_state = STATE_HISTORY_PAGE_1;
                else
                    program_state = STATE_STANDBY;
            } else {
                program_state = STATE_STANDBY;
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

void init_children(char *letters, struct trie_node *node, struct trie_node **trie_ptr) {
    int i = -1;
    while(letters[++i] != '\0') {
    	node->children[i] = (*trie_ptr)++;
    	node->children[i]->letter = letters[i];
    	node->children[i]->parent = node;
    }
    
    node->children_count = i;
}

void init_fastener_trie(void) {
    struct trie_node *trie_ptr = &fastener_trie.nodes[0];
    trie_ptr->letter = 'A';
    trie_ptr->parent = &fastener_trie.nodes[0];
    trie_ptr++;
    init_children("BNSW", &fastener_trie.nodes[0], &trie_ptr);
    init_children("BNSW", &fastener_trie.nodes[1], &trie_ptr);
    init_children("NSW", &fastener_trie.nodes[5], &trie_ptr);
    init_children("NW", &fastener_trie.nodes[6], &trie_ptr);
    init_children("W", &fastener_trie.nodes[7], &trie_ptr);
    init_children("W", &fastener_trie.nodes[8], &trie_ptr);
    init_children("W", &fastener_trie.nodes[9], &trie_ptr);
    init_children("W", &fastener_trie.nodes[10], &trie_ptr);
    init_children("NW", &fastener_trie.nodes[12], &trie_ptr);
    init_children("W", &fastener_trie.nodes[13], &trie_ptr);
    init_children("W", &fastener_trie.nodes[14], &trie_ptr);
    init_children("W", &fastener_trie.nodes[15], &trie_ptr);
}

char trie_node_leaf_index(char c) {
    for(int i = 0; i < 4; i++) {
        if(program_status.trie_ptr->children[i] != NULL && program_status.trie_ptr->children[i]->letter == c)
            return i;
    }
    return 4;
}


