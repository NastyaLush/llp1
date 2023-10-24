//
// Created by nastyalush on 10/15/23.
//

#ifndef LLPLAB_ITERATOR_H
#define LLPLAB_ITERATOR_H

#include "graph_stracture.h"
#include "page_operations.h"
#include <unistd.h>

struct block_iterator{
    struct file * f;
    void* page;
    void* block;

};
void *get_next_block(struct block_iterator* blockIterator);
struct block_iterator init_block_iterator(struct file const * const f, void* main_page);
#endif //LLPLAB_ITERATOR_H
