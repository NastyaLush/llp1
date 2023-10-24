#include <stdbool.h>
#include "graph_stracture.h"
#include <unistd.h>

struct page_header
create_page_header(int const number, bool const is_free, bool const is_main, size_t const next, size_t const prev,
                   enum type_block const type_block, size_t const offset_for_free_block
) {
    struct page_header ph;
    ph.number = number;
    ph.is_free = is_free;
    ph.is_main = is_main;
    ph.next = next;
    ph.prev = prev;
    ph.type_block = type_block;
    ph.offset_for_free_block = offset_for_free_block;
    return ph;
}

struct page_header *get_page_header(void const *const page) {
    return (struct page_header *) page;
}

void change_page_header(struct page_header *const ph, bool const is_free, bool const is_main, size_t const next,
                        size_t const prev,
                        enum type_block const type_block, size_t const offset_for_free_block) {

    ph->is_free = is_free;
    ph->is_main = is_main;
    ph->next = next;
    ph->prev = prev;
    ph->type_block = type_block;
    ph->offset_for_free_block = offset_for_free_block;

}

struct vertex_table_header create_table_header(char const name[SIZE_OF_NAME],
                                               size_t const count_of_attr,
                                               size_t const last) {
    struct vertex_table_header th;
    th.last = last;
    strcpy(th.name, name);
    th.count_of_attr = count_of_attr;
    th.last_node_id = 0;
    return th;
}

struct vertex_table_header *get_vertex_table_header(void const *const page) {
    return (struct vertex_table_header *) page + sizeof(struct page_header);
}

struct vertex create_vertex(int const node_id) {
    struct vertex v;
    v.node_id = node_id;
    return v;
}

struct block_header
create_block_header(bool const is_full, size_t const size, struct link const next, struct link const prev) {
    struct block_header bh = {};
    bh.is_full = is_full;
    bh.size = size;
    bh.capacity = CAPACITY_OF_BLOCK;
    bh.next = next;
    bh.prev = prev;
    return bh;
}

struct edje_table_header create_edje_table_header(char const name[SIZE_OF_NAME]) {
    struct edje_table_header eh;
    strcpy(eh.name, name);
    return eh;
}

struct block_header *get_block_header(void const *const block) {
    return (struct block_header *) block;
}

struct link create_link_structure(size_t const page, size_t const offset) {
    struct link link1 = {};
    link1.page = page;
    link1.offset = offset;
    return link1;
}

struct link create_null_link_structure() {
    struct link link1 = {-1, -1};
    return link1;
}

struct attr_header *get_attr_header(void const *const page, int const num) {
    return (struct attr_header *) (page + sizeof(struct page_header) + sizeof(struct vertex_table_header));
}

struct vertex *get_vertex(void const *const block) {
    return (struct vertex *) (block + sizeof(struct block_header));
}

struct attr_body_for_write convert_attr_to_write(enum node_type const nt, struct attr_body const ab) {
    struct attr_body_for_write attr;
    switch (nt) {
        case STRING:
            attr.value.len_string = strlen(ab.value.string);
            break;
        case INT32:
            attr.value.int32 = ab.value.int32;
            break;
        case BOOLEAN:
            attr.value.boolean = ab.value.boolean;
            break;
        case REAL:
            attr.value.real = ab.value.real;
            break;
    }
    return attr;
}

struct attr_header_with_size *get_attr_header_with_size(void const *const pointer) {
    return (struct attr_header_with_size *) pointer;
}

struct attr_body_for_write *get_attr_body_for_write(void const *const pointer) {
    return (struct attr_body_for_write *) pointer;
}
