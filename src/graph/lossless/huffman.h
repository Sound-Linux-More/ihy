#ifndef DEF_HUFFMAN
#define DEF_HUFFMAN

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <utils/queue.h>

typedef struct huffman_tree
{
    unsigned char		letter;
    int				occurrence;
    int				code;
    int				codelength;
    struct huffman_tree		*fg;
    struct huffman_tree		*fd;
} huffman_tree;

/* pretty print huffman */
void huffman_pretty(huffman_tree *B, int code, int code_size);

/* build the huffman tree, and return it */
huffman_tree *build_huffman(const void *varray, const size_t n);

/* encode the data
 * n is the size of varray and will be, when the function returns, the
 * size of the array returned
 */
int8_t *huffman_encode(const void *varray, size_t *n, huffman_tree *H);

/* release the memory used by a huffman tree */
void destroy_huffman(huffman_tree *B);

#endif