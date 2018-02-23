#include "program_states.h"

void FUNC_STATE_STANDBY(void) {
    printf("Load fasteners &");
    __lcd_newline();
    printf("Press to start.");
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
}

void FUNC_STATE_REVIEW_SET(void) {
    printf("Set %d:", program_status.compartment_count_index + 1);
    __lcd_newline();
    printf("B:%d N:%d S:%d W:%d ", program_status.set_count[program_status.compartment_count_index][0], program_status.set_count[program_status.compartment_count_index][1], program_status.set_count[program_status.compartment_count_index][2], program_status.set_count[program_status.compartment_count_index][3]);
}

void FUNC_STATE_EXECUTE(void) {
    printf("Executing...");
    __lcd_newline();
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
    
    init_fastener_trie();
    program_status.compartment_count_index = 0;
    program_status.buffer_index = 0;
    program_status.trie_ptr = &fastener_trie.nodes[0];
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

