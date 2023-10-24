
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "graph_stracture.h"
#include "io.h"
#include "mmap_functions.h"
#include "page_operations.h"


void
add_vertex_table_header_to_page(void const * const page, struct page_header * const ph, char name[SIZE_OF_NAME], size_t count_of_attr) {
    struct vertex_table_header th = create_table_header(name, count_of_attr, ph->number);

    memcpy(page + ph->offset_for_free_block, &th, sizeof(struct vertex_table_header));
    ph->offset_for_free_block += sizeof(struct vertex_table_header);
}

void *create_and_get_empty_table(struct file * const f, char const name[SIZE_OF_NAME], size_t count_of_attr) {
    void *page = get_empty_page(f, VERTEX, true);
    struct page_header *ph = get_page_header(page);
    add_vertex_table_header_to_page(page, ph, name, count_of_attr);
    return page;
}

void *create_and_get_empty_edje_table(struct file * const f, char const name[SIZE_OF_NAME]) {
    void *page = get_empty_page(f, EDJE, true);
    struct page_header *ph = get_page_header(page);
    struct edje_table_header eh = create_edje_table_header(name);

    memcpy(page + ph->offset_for_free_block, &eh, sizeof(struct edje_table_header));
    ph->offset_for_free_block += sizeof(struct edje_table_header);

    return page;
}


void add_attr_header_to_memory(void const * const page, struct attr_header const * const name_attr) {
    struct page_header *ph = get_page_header(page);
    memcpy(page + ph->offset_for_free_block, name_attr,
           sizeof(struct attr_header));
}

void add_attrs_header_to_memory(void const * const page, int count_of_attr, struct attr_header *name_attr) {
    for (int i = 0; i < count_of_attr; i++) {
        add_attr_header_to_memory(page, name_attr + i);
    }
}


void create_table(struct file * const f, size_t node_id, char name[SIZE_OF_NAME], size_t count_of_attr,
                  struct attr_header *name_attr) {
    void *page = create_and_get_empty_table(f, name, count_of_attr);
    struct page_header *ph = get_page_header(page);
    if (ph->offset_for_free_block + count_of_attr * sizeof(struct attr_header) > PAGE_SIZE) {
        ph->is_free=true;
        fprintf(stderr, "все данные не поместятся на одну страницу");
        return;
    }
    add_attrs_header_to_memory(page, count_of_attr, name_attr);

    create_and_get_empty_edje_table(f, name);
    fprintf(stdout, "f");
}

void *find_main_table(struct file const * const f, const char name_table[SIZE_OF_NAME], enum type_block tb) {
    int counter = 0;
    void *page = get_page_by_num(f, counter);
    char *potentialNameTable = NULL;
    while (page != NULL) {
        struct page_header *ph = get_page_header(page);
        struct vertex_table_header *pv = get_vertex_table_header(page);
        if (!ph->is_free && ph->is_main && ph->type_block == tb) {
            potentialNameTable = pv->name;
            if (strcmp(name_table, potentialNameTable) == 0)return page;

        }
        page = get_page_by_num(f, ++counter);
    }
    return NULL;
}

void drop_table(struct file const * const f, char const name[SIZE_OF_NAME]) {
    void *page = find_main_table(f, name, VERTEX);
    if (page == NULL)return;

    struct page_header *ph = get_page_header(page);
    ph->is_free = true;
    while (ph->next != -1) {
        ph = get_page_header_by_num(f, ph->next);
        ph->is_free = true;
    }
    page = find_main_table(f, name, EDJE);
    if (page == NULL)return;

    ph = get_page_header(page);
    ph->is_free = true;
    while (ph->next != -1) {
        ph = get_page_header_by_num(f, ph->next);
        ph->is_free = true;
    }
}

