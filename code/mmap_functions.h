//
// Created by nastyalush on 10/10/23.
//

#ifndef LLPLAB_MMAP_FUNCTIONS_H
#define LLPLAB_MMAP_FUNCTIONS_H

#include <stdbool.h>
#include "graph_stracture.h"

void *mmap_memory_for_page(int *res, int const fdin, size_t const size, size_t const offset);
bool resize_file(struct file * const f, size_t const size);
void *alloc_memory_for_new_page(struct file * const f);
#endif //LLPLAB_MMAP_FUNCTIONS_H
