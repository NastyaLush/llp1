//
// Created by nastyalush on 10/9/23.
//

#ifndef LLPLAB_TABLE_OPERATIONS_H
#define LLPLAB_TABLE_OPERATIONS_H
void create_table(struct file * const f, size_t node_id, char name[SIZE_OF_NAME], size_t count_of_attr,
                  struct attr_header *name_attr);
void drop_table(struct file const * const f, char const name[SIZE_OF_NAME]);
void *find_main_table(struct file const * const f, const char name_table[SIZE_OF_NAME], enum type_block tb);

#endif //LLPLAB_TABLE_OPERATIONS_H
