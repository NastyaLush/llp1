
#include "mmap_functions.h"
#include "table_operations.h"
#include "page_operations.h"
#include "iterator.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "io.h"

bool is_exist_free_block(struct vertex_table_header *vh) {
    return vh->first_free_block.page != create_null_link_structure().page;
}

// зачем тут page
void *
get_or_create_free_block(struct file *const f, void const *const main_page, void const *const page, void *new_page) {
    struct page_header *ph = get_page_header(page);
    struct vertex_table_header *vh = get_vertex_table_header(main_page);
    struct block_header bh = create_block_header(true, sizeof(struct block_header),
                                                 create_null_link_structure(),
                                                 create_null_link_structure());

    if (!is_exist_free_block(vh)) {
        if (PAGE_SIZE - ph->offset_for_free_block - CAPACITY_OF_BLOCK <= 0) {
            new_page = get_empty_page(f, VERTEX, false);

            struct page_header *new_ph = get_page_header(new_page);
            ph->next = new_ph->number;
            new_ph->prev = ph->number;
            vh->last = new_ph->number;
            ph = new_ph;
            bh = create_block_header(true, sizeof(struct block_header),
                                     create_link_structure(ph->number, ph->offset_for_free_block),
                                     create_link_structure(ph->number, ph->offset_for_free_block));
        }
        memcpy(new_page + ph->offset_for_free_block, &bh, sizeof(struct block_header));
        ph->offset_for_free_block += CAPACITY_OF_BLOCK;
        return new_page + ph->offset_for_free_block - CAPACITY_OF_BLOCK;
    } else {
        new_page = get_page_by_num(f, vh->first_free_block.page);
        struct block_header *old_bh = get_block_header(new_page + vh->first_free_block.offset);
        struct link linkk = old_bh->next;
        memcpy(new_page + vh->first_free_block.offset, &bh, sizeof(struct block_header));
        vh->first_free_block = linkk;
    }
}

void add_meta_inf_when_add_block(void const *old_page,
                                 void const *old_block, void const *new_page,
                                 void const *new_block) {
    struct page_header *old_ph = get_page_header(old_page);
    struct block_header *old_bh = get_block_header(old_block);
    struct block_header *new_bh = get_block_header(new_block);
    struct page_header *new_ph = get_page_header(new_page);


    old_bh->next = create_link_structure(new_ph->number, new_page - new_block);
    new_bh->prev = create_link_structure(old_ph->number, old_ph->offset_for_free_block - CAPACITY_OF_BLOCK);
}

void write_string(struct file *const f, void const *main_page, void const *page, void const *block,
                  char const *string) {
    void *old_page = page;
    void *old_block = block;

    void *new_block;
    void *new_page;

    struct page_header *ph = get_page_header(old_page);
    struct block_header *bh = get_block_header(old_block);

    unsigned long len = strlen(string);
    unsigned long len_written = 0;

    int free_mem;

    while (len_written != len) {
        free_mem = CAPACITY_OF_BLOCK - bh->size;
        if (free_mem >= len) {
            memcpy(old_block + bh->size, string + len_written, sizeof(char) * len);
            bh->size += sizeof(char) * len;
            len_written = len;
        } else {
            memcpy(old_block + bh->size, string + len_written, free_mem);
            bh->size += free_mem;
            len_written = free_mem;

            new_block = get_or_create_free_block(f, main_page, page, new_page);

            add_meta_inf_when_add_block(old_block, old_block, new_page, new_block);
            old_block = new_block;
            old_page = new_page;
            bh = get_block_header(new_block);
            ph = get_page_header(new_page);
        }
    }
}

void add_vertex(void const *const block, struct block_header *const bh, struct vertex const *const vertex) {
    memcpy(block + bh->size, vertex, sizeof(struct vertex));
    bh->size += sizeof(struct vertex);
}

void add_node(struct file *const f, char const *const name_table, struct vertex *const vertex,
              struct attr const *const attrs) {
    void *main_page = find_main_table(f, name_table, VERTEX);
    if (main_page == NULL) {
        fprintf(stdout, "This table is not exist");
        return;
    }

    struct vertex_table_header *th = get_vertex_table_header(main_page);
    void *page = get_page_by_num(f, th->last);
    struct page_header *ph = get_page_header(page);

    int i = 0;
    void *new_page;
    void *block = get_or_create_free_block(f, main_page, page, new_page);
    page = new_page;
    struct block_header *bh = get_block_header(block);

    if (th->first_vertex.page == create_null_link_structure().page) {
        th->first_vertex = create_link_structure(get_page_header(page)->number, page - block);
    } else {
        get_vertex((get_page_by_num(f, th->last_free_block.page) +
                    th->last_free_block.offset))->next_vertex = create_link_structure(get_page_header(page)->number,
                                                                                      page - block);
    }
    vertex->prev_vertex = th->last_vertex;
    th->last_vertex = create_link_structure(get_page_header(page)->number, page - block);

    add_vertex(block, bh, vertex);
    while (i != th->count_of_attr) {
        if (attrs[i].header.type != STRING) {
            if (CAPACITY - bh->size > sizeof(struct attr_header) + sizeof(struct attr_body_for_write)) {
                memcpy(block + bh->size, &((attrs + i)->header), sizeof(struct attr_header));
                bh->size += sizeof(struct attr_header);
                struct attr_body_for_write attrib = convert_attr_to_write((attrs + i)->header.type, (attrs + i)->body);
                memcpy(block + bh->size, &attrib, sizeof(struct attr_body_for_write));
                bh->size += sizeof(struct attr_body_for_write);
            } else {
                void *new_block = get_or_create_free_block(f, main_page, page, new_page);
                add_meta_inf_when_add_block(page, block, new_page, new_block);
                page = new_page;
                block = new_block;
                bh = get_block_header(new_block);
                ph = get_page_header(page);

                memcpy(block + bh->size, &((attrs + i)->header), sizeof(struct attr_header));
                bh->size += sizeof(struct attr_header);
                struct attr_body_for_write attrib = convert_attr_to_write((attrs + i)->header.type, (attrs + i)->body);
                memcpy(block + bh->size, &attrib, sizeof(struct attr_body_for_write));
                bh->size += sizeof(struct attr_body_for_write);
            }
        } else {
            if (CAPACITY - bh->size > sizeof(struct attr_header) + sizeof(struct attr_body_for_write)) {
                memcpy(block + bh->size, &((attrs + i)->header), sizeof(struct attr_header));
                bh->size += sizeof(struct attr_header);
                struct attr_body_for_write attrib = convert_attr_to_write((attrs + i)->header.type, (attrs + i)->body);
                memcpy(block + bh->size, &attrib, sizeof(struct attr_body_for_write));
                bh->size += sizeof(struct attr_body_for_write);
            } else {
                void *new_block = get_or_create_free_block(f, main_page, page, new_page);
                add_meta_inf_when_add_block(page, block, new_page, new_block);
                page = new_page;
                block = new_block;

                bh = get_block_header(new_block);
                ph = get_page_header(page);

                memcpy(block + bh->size, &((attrs + i)->header), sizeof(struct attr_header));
                bh->size += sizeof(struct attr_header);
                struct attr_body_for_write attrib = convert_attr_to_write((attrs + i)->header.type, (attrs + i)->body);
                memcpy(block + bh->size, &attrib, sizeof(struct attr_body_for_write));
                bh->size += sizeof(struct attr_body_for_write);
            }
            write_string(f, main_page, page, block, attrs[i].body.value.string);
        }
        i++;
    }

    fprintf(stdout, "This table is exist");


}

void *get_block(struct file const *const f, int n_page, int offset) {
    void *page = get_page_by_num(f, n_page);
    return page + offset;
}

void read_str(struct file *f, void *block, char *string, int len) {
    struct block_header *bh = get_block_header(block);
    int offset = sizeof(struct block_header);

    if (CAPACITY_OF_BLOCK - offset >= len) {
        memcpy(string, block + offset, len);
    } else {
        memcpy(string, block + offset, CAPACITY_OF_BLOCK - offset);
        read_str(f, get_block(f, bh->next.page, bh->next.offset), string + CAPACITY_OF_BLOCK - offset,
                 (len - CAPACITY_OF_BLOCK + offset));
    }
}


void read_string(struct file *f, void *block, int len_str, int offset, char *string) {
    struct block_header *bh = get_block_header(block);
    if (CAPACITY_OF_BLOCK - offset >= len_str) {
        memcpy(string, block + offset, len_str);
    } else {
        memcpy(string, block + offset, CAPACITY_OF_BLOCK - offset);
        read_str(f, get_block(f, bh->next.page, bh->next.offset), string + CAPACITY_OF_BLOCK - offset,
                 (len_str - CAPACITY_OF_BLOCK + offset));
    }

}


struct attr_body
convert_attr_body_for_write_to_attr_body(struct attr_body_for_write *attr_body_for_write, enum type_block tb) {
    struct attr_body attr_body;
    switch (tb) {
        case REAL:
            attr_body.value.real = attr_body_for_write->value.real;
        case INT32:
            attr_body.value.int32 = attr_body_for_write->value.int32;
        case BOOLEAN:
            attr_body.value.boolean = attr_body_for_write->value.boolean;
    }
    return attr_body;

}

struct vertex_for_read *read_element(struct file *f, void *main_page, void *block) {

    size_t count_of_attr = get_vertex_table_header(main_page)->count_of_attr;
    struct vertex_for_read read_vertex;
    int read_attr = 0;
    int size_attr = sizeof(struct attr_header_with_size) + sizeof(struct attr_body_for_write);
    int busy_block = sizeof(struct block_header) + sizeof(struct vertex);
    struct block_header *bh = get_block_header(block);
    struct vertex *v = get_vertex(block);
    read_vertex.vertex = *v;
    while (read_attr < count_of_attr) {
        struct attr atribut;
        if (CAPACITY_OF_BLOCK - busy_block >= size_attr) {
            struct attr_header_with_size *attrHeaderWithSize = get_attr_header_with_size(block + busy_block);
            atribut.header = *attrHeaderWithSize;
            struct attr_body_for_write *attr_body_for_write = get_attr_body_for_write(block + busy_block);
            atribut.body = convert_attr_body_for_write_to_attr_body(attr_body_for_write, attrHeaderWithSize->type);
            busy_block += size_attr;
            if (attrHeaderWithSize->type == STRING) {
                char string[attr_body_for_write->value.len_string];
                read_string(f, block, attr_body_for_write->value.len_string, busy_block, string);
                atribut.body.value.string = string;
            }
            read_attr++;
        } else {
            block = get_block(f, bh->next.page, bh->next.offset);
            bh = get_block_header(block);
            busy_block = sizeof(struct block_header);
        }
    }
}

void select_nodes_from_table(struct file *f, char name[SIZE_OF_NAME]) {
    void *page = find_main_table(f, name, VERTEX);
    struct block_iterator block_iterator = init_block_iterator(f, page);
    void *block_vertex = block_iterator.block;
    while (block_vertex != NULL) {
        read_element(f, page, block_vertex);
    }

//    struct page_header *ph = get_page_header(page);
//    struct vertex_table_header *vh = get_vertex_table_header(page);
//    for (int i = 0; i < vh->count_of_attr; i++) {
//        struct attr_header *ah = get_attr_header(page, i);
//    }
//    size_t busy = sizeof(struct page_header) + sizeof(struct vertex_table_header) +
//                  vh->count_of_attr * sizeof(struct attr_header);
//    if (ph->offset_for_free_block <= busy) return;
//    void *block = page - busy;
//    struct block_header *bh = get_block_header(page - busy);
//    busy += CAPACITY_OF_BLOCK;
//
//
//    while (ph->offset_for_free_block > busy) {
//        struct block_header *bh = get_block_header(page - busy);
//        busy += sizeof(struct block_header);
//        struct vertex *v = get_vertex(page - busy);
//        busy += sizeof(struct vertex);
//    }


}


void *get_vertex_block_by_id(struct block_iterator *block_iterator, int id_vertex) {

    void *block = get_next_block(&block_iterator);

    while (block != NULL) {
        struct block_header *bh = get_block_header(block);
        if (bh->id_vertex == id_vertex) {
            return block;
        }
        block = get_next_block(&block_iterator);
    }
    return NULL;
}

struct vertex_for_read *get_vertex_by_id(struct file *const f, char const *const name_table, int id_vertex) {
    void *page = find_main_table(f, name_table, VERTEX);
    struct block_iterator block_iterator = init_block_iterator(f, page);
    void *block = get_vertex_block_by_id(&block_iterator, id_vertex);
    return read_element(f, page, block);
}


void delete_vertex(struct file *const f, char const *const name_table, int id_vertex) {
    int page_n, offset;
    void *page = find_main_table(f, name_table, VERTEX);
    struct vertex_table_header *vh = get_vertex_table_header(page);
    struct link l = vh->last_free_block;
    struct block_header *old_bh = get_block_header(get_block(f, l.page, l.offset));

    struct block_iterator block_iterator = init_block_iterator(f, page);


    void *block = get_vertex_block_by_id(&block_iterator, id_vertex);
    struct block_header *bh = get_block_header(block);

    struct vertex *vertex = get_vertex(block);
    if (vertex->prev_vertex.page = create_null_link_structure().page) {
        if (vertex->next_vertex.page != create_null_link_structure().page) {
            void *block_next = get_page_by_num(f, vertex->next_vertex.page) + vertex->next_vertex.offset;
            get_vertex(block_next)->prev_vertex = create_link_structure(vertex->next_vertex.page,
                                                                        vertex->next_vertex.offset);
        } else {
            vh->first_vertex = create_link_structure(vertex->next_vertex.page, vertex->next_vertex.offset);
        }
    } else {
        void *block_prev = get_page_by_num(f, vertex->prev_vertex.page) + vertex->prev_vertex.offset;
        void *block_next;
        if (vertex->next_vertex.page = create_null_link_structure().page) {
            get_vertex(block_prev)->next_vertex = create_null_link_structure();
        } else {
//            void *block_next = get_page_by_num(f, vertex->next_vertex.page) + vertex->next_vertex.offset;
            get_vertex(block_prev)->next_vertex = create_link_structure(vertex->next_vertex.page,
                                                                        vertex->next_vertex.offset);
        }
    }


    bh->is_full = false;

    page_n = get_page_header(block_iterator.page)->number;
    offset = block_iterator.block - block_iterator.page;
    old_bh->next = create_link_structure(page_n, offset);

    while (bh->next.page != create_null_link_structure().page) {
        page_n = bh->next.page;
        offset = bh->next.offset;
        block = get_block(f, bh->next.page, bh->next.offset);
        bh = get_block_header(block);
        bh->is_full = false;
    }
    vh->last_free_block = create_link_structure(page_n, offset);
}

void update(struct file *const f, char const *const name_table, int id_vertex) {
    void *page = find_main_table(f, name_table, VERTEX);
    struct block_iterator block_iterator = init_block_iterator(f, page);
    get_vertex_block_by_id(&block_iterator, id_vertex);
    //далее делай что хочешь
}



struct edje *get_edje_block_by_page_and_num(struct file *const f, int page, int offset) {
    return get_page_by_num(f, page) + offset;
}

struct edje *get_edje_block(struct file *const f, void const *const main_edje_page, void *new_page) {
    struct edje *free_edje;
    struct page_header *ph = get_page_header(main_edje_page);
    struct edje_table_header *edjeTableHeader = main_edje_page + sizeof(struct page_header);
    if (edjeTableHeader->first_free_block.page != create_null_link_structure().page) {
        free_edje = get_edje_block_by_page_and_num(f, edjeTableHeader->first_free_block.page,
                                                   edjeTableHeader->first_free_block.offset);
        new_page = get_page_by_num(f, edjeTableHeader->first_free_block.page);
        edjeTableHeader->first_free_block = free_edje->next;
        return free_edje;
    } else {
        new_page = get_page_by_num(f, edjeTableHeader->last_edje.page);
        struct page_header *last_ph = get_page_header(new_page);
        if (PAGE_SIZE - last_ph->offset_for_free_block > sizeof(struct edje)) {
            free_edje = new_page + last_ph->offset_for_free_block;
            last_ph->offset_for_free_block += sizeof(struct edje);
            edjeTableHeader->last_edje = create_link_structure(edjeTableHeader->last_edje.page,
                                                               last_ph->offset_for_free_block - sizeof(struct edje));
            return free_edje;
        } else {
            new_page = get_empty_page(f, EDJE, false);

            struct page_header *new_ph = get_page_header(new_page);

            last_ph->next = new_ph->number;
            new_ph->prev = last_ph->number;
            edjeTableHeader->last_edje = create_link_structure(new_ph->number, new_ph->offset_for_free_block);
            new_ph->offset_for_free_block += sizeof(struct edje);

            return new_page + new_ph->offset_for_free_block;
        }
    }

}

void add_edje(struct file *const f, char const *const name_table, int id_vertex, struct edje *edje_impl) {
    void *main_vertex_page = find_main_table(f, name_table, VERTEX);
    void *main_edje_page = find_main_table(f, name_table, EDJE);
    struct page_header *ph = get_page_header(main_edje_page);
    struct block_iterator block_iterator = init_block_iterator(f, main_vertex_page);
    void *block = get_vertex_block_by_id(&block_iterator, id_vertex);
    struct vertex *vertex = get_vertex(block);

    void *new_page;
    void *edje_block = get_edje_block(f, main_edje_page, new_page);
    struct link last_edje_link = vertex->last_edje;
    if (vertex->first_edje.page == create_null_link_structure().page) {
        vertex->first_edje = create_link_structure(get_page_header(new_page)->number, edje_block - new_page);
        vertex->last_edje = create_link_structure(get_page_header(new_page)->number, edje_block - new_page);
    } else {
        struct edje *last_edje = get_page_by_num(f, last_edje_link.page) + last_edje_link.offset;
        last_edje->next = create_link_structure(get_page_header(new_page)->number, edje_block - new_page);
        vertex->last_edje = last_edje->next;
        edje_impl->prev = last_edje_link;
    }
    memcpy(edje_block, edje_impl, sizeof(struct edje));
}
void delete_edje(struct file *const f, char const *const name_table, int id_vertex, struct edje edje_for_delete){


}
//void delete_edje() {
//    // from to это id, при перемещении
//    // хранить указатель на свободное место и далее next и закрывать дыры при вставке ребра
//}
//
//void delete_node_from_table() {
//
//}
//
//void read_vertex()
//        add_node
//        void update_node(){
//    read_vertex()
//    update_vertex()
//    add_node()
//}
//
//void update_edje() {
//    //очев
//}