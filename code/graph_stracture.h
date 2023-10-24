#ifndef GRAPH_H
#define GRAPH_H

#include <string.h>
#include <stdint-gcc.h>
#include <stdbool.h>

#define SIZE_OF_NAME 10
#define CAPACITY_OF_BLOCK PAGE_SIZE/10
#define SIZE_OF_STRING 1024
#define COUNT_OF_ATTR 3
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define CAPACITY 200


enum node_type {
    INT32,
    REAL,
    STRING,
    BOOLEAN
};
enum request_type {
    INSERT,
    SELECT,
    UPDATE,
    DELETE
};
enum type_block {
    VERTEX,
    EDJE
};
struct link {
    uint32_t page;
    uint16_t offset;
};
struct block_header {
    bool is_full;
    size_t id_vertex;
    uint8_t size;
    uint16_t capacity;
    struct link next;
    struct link prev;
};

struct page_header {
    size_t number;
    bool is_free;
    bool is_main;
    size_t next;
    size_t prev;
    enum type_block type_block;
    uint16_t offset_for_free_block;
};
struct vertex_table_header {
    char name[SIZE_OF_NAME];
    struct link first_attr;
    size_t last_node_id;
    uint8_t count_of_attr;
    size_t last;
    struct link first_vertex;
    struct link last_vertex;
    struct link first_free_block;
    struct link last_free_block;
};

struct edje_table_header {
    char name[SIZE_OF_NAME];
    struct link first_free_block;
    struct link last_free_block;
    struct link last_edje;
};
struct attr_header {
    char name[SIZE_OF_NAME];
    enum node_type type;
};

//------------------------- end of meta inf
// ссылка на первое и последнее ребро
struct vertex {
    size_t node_id;
    struct link next_vertex;
    struct link prev_vertex;
    struct link first_edje;
    struct link last_edje;
};
struct attr_header_with_size {
    char name[SIZE_OF_NAME];
    enum node_type type;
//    size_t size;
};
struct attr_body {
    union {
        int32_t int32;
        float real;
        char *string;
        bool boolean;
    } value;
};
struct attr_body_for_write {
    union {
        int32_t int32;
        float real;
        unsigned int len_string;
        bool boolean;
    } value;
};
struct attr {
    struct attr_header_with_size header;
    struct attr_body body;
};


struct edje {
    char name[SIZE_OF_NAME];
    size_t from;
    size_t to;
    struct link next;
    struct link prev;
};

struct vertex_for_read{
    struct vertex vertex;
    struct attr * attrs;

};

struct page_header
create_page_header(int const number, bool const is_free, bool const is_main, size_t const next, size_t const prev,
                   enum type_block const type_block, size_t const offset_for_free_block
);

void change_page_header(struct page_header *ph, bool is_free, bool is_main, size_t next, size_t prev,
                        enum type_block type_block, size_t offset_for_free_block);

struct page_header *get_page_header(void const *const page);

struct vertex_table_header create_table_header(char const name[SIZE_OF_NAME],
                                               size_t const count_of_attr,
                                               size_t const last);

struct vertex create_vertex(int const node_id);

struct vertex *get_vertex(void const *const block);

struct vertex_table_header *get_vertex_table_header(void const *const page);

struct block_header
create_block_header(bool const is_full, size_t const size, struct link const next, struct link const prev);

struct block_header *get_block_header(void const *const block);

struct edje_table_header create_edje_table_header(char const name[SIZE_OF_NAME]);

struct link create_link_structure(size_t const page, size_t const offset);

struct link create_null_link_structure();

struct attr_header *get_attr_header(void const *const page, int const num);

struct attr_body_for_write convert_attr_to_write(enum node_type const nt, struct attr_body const ab);

struct attr_header_with_size *get_attr_header_with_size(void const *const pointer);

struct attr_body_for_write *get_attr_body_for_write(void const *const pointer);

struct request {

};
struct response {

};
#endif