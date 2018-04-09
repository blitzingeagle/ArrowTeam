/*
 * File:   trie.c
 * Author: morris
 *
 * Created on April 6, 2018, 3:21 PM
 */

#include "trie.h"

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

