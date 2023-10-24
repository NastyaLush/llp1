
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "graph_responses.h"
#include "io.h"

int open_or_create_file(const char *const filename, int *res) {
    int fdin;
    *res = FILE_OPENED;
    if ((fdin = open(filename, O_RDWR)) == -1) {
        fprintf(stdout, "impossible to open file %s\n", filename);
        fprintf(stdout, "creating new file\n");
        if ((fdin = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
            fprintf(stderr, "%d", fdin);
            fprintf(stderr, "impossible to create %s\n", filename);
            *res = FAILED;
            return -1;
        }
    }
    return fdin;
}


void get_file_structure(const char *const filename, int *res, struct file * const f) {

    struct stat stat_buf;
    int fdin = open_or_create_file(filename, res);
    if (*res == FAILED)
        return;
    if (fstat(fdin, &stat_buf) < 0) {
        fprintf(stderr, "fstat error\n");
        *res = FAILED;
        return;
    }
    f->fdin = fdin;
    f->size = stat_buf.st_size;

}


void close_file(const int fdin) {
    if (close(fdin)) {
        fprintf(stderr, "impossible to close file\n");
    }
}

void read_db(const char *const filename) {
    int res;
    struct file f;
    get_file_structure(filename, &res, &f);
    if (res == FAILED) {
        fprintf(stderr, "%s", "impossible to execute program");
        return;
    }
    if (res == FILE_CREATED) {
        fprintf(stdout, "%s", "create_new_db");
    }

}
