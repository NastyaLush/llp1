


//
// Created by nastyalush on 10/15/23.
//
#include "iterator.h"

void *get_block_by_page_and_offset(void *page, int offset) {
    return page + offset;
}

void *get_next_block(struct block_iterator *blockIterator) {
    struct page_header *ph = get_page_header(blockIterator->page);
    struct block_header *bh = get_block_header(blockIterator->block);
    struct vertex *v = get_vertex(blockIterator->block);
    if (v->next_vertex.page != create_null_link_structure().page) {
        blockIterator->page = get_page_by_num(blockIterator->f, v->next_vertex.page);
        blockIterator->block = get_block_by_page_and_offset(blockIterator->page, v->next_vertex.offset);

    } else {
        return NULL;
    }
}

struct block_iterator init_block_iterator(struct file const *const f, void *main_page) {
    struct block_iterator block_iterator;
    struct vertex_table_header* vh = get_vertex_table_header(main_page);
    block_iterator.f = f;
    block_iterator.page = main_page;
    block_iterator.block = main_page+sizeof(struct page_header) + sizeof(struct vertex_table_header)+sizeof(struct  attr_header)*vh->count_of_attr;
}