/* 
 * File:   trie.h
 * Author: morris
 *
 * Created on April 6, 2018, 3:20 PM
 */

#ifndef TRIE_H
#define	TRIE_H

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

#endif	/* TRIE_H */

