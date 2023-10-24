//
// Created by nastyalush on 10/9/23.
//

#ifndef LLPLAB_IO_H
#define LLPLAB_IO_H


#include <string.h>

struct file{
    int fdin;
    size_t size;
};
void get_file_structure(const char *const filename, int *res, struct file * const f);
#endif //LLPLAB_IO_H
