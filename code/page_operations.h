//
// Created by nastyalush on 10/10/23.
//

#ifndef LLPLAB_PAGE_OPERATIONS_H
#define LLPLAB_PAGE_OPERATIONS_H
void add_next_to_page(void const *const page, int next);
void add_page_header_to_page(void const *const page, size_t number, enum type_block tb, bool is_main);
struct page_header *get_page_header_by_num(struct file const *const f, int n);
void *get_page_by_num(struct file const *const f, int n);
bool find_free_page(struct file const *const f, int *num) ;
void *get_empty_page(struct file *const f, enum type_block tb, bool is_main) ;

#endif //LLPLAB_PAGE_OPERATIONS_H
