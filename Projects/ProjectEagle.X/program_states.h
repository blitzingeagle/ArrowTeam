/* 
 * File:   program_states.h
 * Author: Morris Chen
 *
 * Created on February 05, 2018, 5:44 PM
 */

#ifndef PROGRAM_STATES_H
#define	PROGRAM_STATES_H

#include "lcd.h"
#include "history.h"

#include <stdbool.h>

enum PROG_STATES {
    STATE_STANDBY,
    STATE_PROMPT_COMPARTMENT_COUNT,
    STATE_PROMPT_FASTENER_SET,
    STATE_PROMPT_FASTENER_SET_QUANTITY,
    STATE_PREVIEW_FASTENER_SET,
    STATE_CONFIRM_SETS,
    STATE_REVIEW_SET,
    STATE_EXECUTE,
    STATE_HISTORY,
    STATE_LOG_PAGE_1,
    STATE_SET_TIME,
    NUM_STATES
} program_state;

struct PROG_STATUS {
    char compartment_count;
    char compartment_count_index;
    char set_count_tmp;
    char set_count[8][4];
    unsigned char set_enum;
    unsigned char set_qty;
    char buffer[8];
    char buffer_index;
    struct trie_node *trie_ptr;
    char B, N, S, W;
    char max_quantity;
    unsigned char history_cnt;
    unsigned char history_index;
    struct History history[4];
    bool operating;
    unsigned char edit_time_idx;
    unsigned char time[7];
} program_status;

void (*PROG_FUNC[NUM_STATES])(void);

void FUNC_STATE_STANDBY(void);
void FUNC_STATE_PROMPT_COMPARTMENT_COUNT(void);
void FUNC_STATE_PROMPT_FASTENER_SET(void);
void FUNC_STATE_PROMPT_FASTENER_SET_QUANTITY(void);
void FUNC_STATE_PREVIEW_FASTENER_SET(void);
void FUNC_STATE_CONFIRM_SETS(void);
void FUNC_STATE_REVIEW_SET(void);
void FUNC_STATE_EXECUTE(void);
void FUNC_STATE_HISTORY(void);
void FUNC_STATE_LOG_PAGE_1(void);
void FUNC_STATE_SET_TIME(void);

void init_program_states(void);
void program_states_interrupt(unsigned char key);

#define TRIE_ROOT_ADR &fastener_trie.nodes[0]

struct trie_node {
    struct trie_node *parent;
    struct trie_node *children[4];
    char children_count;
    char letter;
};

struct trie {
    struct trie_node nodes[23];
} fastener_trie;

void init_fastener_trie(void);
char trie_node_leaf_index(char c);

#endif	/* GLCD_PIC_H */
