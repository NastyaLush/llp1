#include <strings.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "graph_responses.h"
#include "io.h"
#include "graph_stracture.h"

void *mmap_memory_for_page(int *res, int const fdin, size_t const size, size_t const offset) {
    void *src;
    if ((src = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdin, offset)) == MAP_FAILED) {
        fprintf(stderr, "mmap failed\n");
        *res = FAILED;
        return NULL;
    }
    return src;
}


bool resize_file(struct file * const f, size_t const size) {
    if (ftruncate(f->fdin, size) == -1) {
        fprintf(stderr, "%s", "ftruncate");
        return false;
    }
    f->size = size;
    return true;
}
void *alloc_memory_for_new_page(struct file * const f) {
    int res;
    resize_file(f, f->size + PAGE_SIZE);
    return mmap_memory_for_page(&res, f->fdin, PAGE_SIZE, f->size - PAGE_SIZE);
}

