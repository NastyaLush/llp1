#include "io.h"
#include <unistd.h>
#include <stdio.h>
#include "graph_stracture.h"
#include "mmap_functions.h"

//
// Created by nastyalush on 10/10/23.
//
void add_next_to_page(void const *const page, int next) {
    memcpy(page + sizeof(char) * SIZE_OF_NAME, &next, sizeof(size_t));
}

void add_page_header_to_page(void const *const page, size_t number, enum type_block tb, bool is_main) {
    struct page_header ph = create_page_header(number,
                                               false, is_main, -1, -1, tb, sizeof(struct page_header));

    memcpy(page, &ph, sizeof(struct page_header));
}

struct page_header *get_page_header_by_num(struct file const *const f, int n) {
    int res;
    if (PAGE_SIZE * n < f->size)
        return get_page_header(mmap_memory_for_page(&res, f->fdin, PAGE_SIZE, PAGE_SIZE * n));
    else return NULL;
}

void *get_page_by_num(struct file const *const f, int n) {
    int res;
    if (PAGE_SIZE * n < f->size)
        return mmap_memory_for_page(&res, f->fdin, PAGE_SIZE, PAGE_SIZE * n);
    else return NULL;
}

bool find_free_page(struct file const *const f, int *num) {
    int counter = 0;
    struct page_header *ph;
    do {
        ph = get_page_header_by_num(f, counter);
        counter++;
    } while (ph != NULL && !ph->is_free);
    if (ph != NULL) {
        *num = counter;
        return true;
    }
    return false;
}
//ссылки не меняются

void *get_empty_page(struct file *const f, enum type_block tb, bool is_main) {
    int num;
    void *page;
    if (find_free_page(f, &num)) {
        fprintf(stdout, "find free page\n");
        page = get_page_by_num(f, num);
        struct page_header *ph = get_page_header_by_num(f, num);
        change_page_header(ph, false, is_main, -1, -1, tb, sizeof(struct page_header));
    } else {
        fprintf(stdout, "create new page\n");
        page = alloc_memory_for_new_page(f);
        add_page_header_to_page(page, f->size / PAGE_SIZE - 1, tb, is_main);
    }
    return page;
}