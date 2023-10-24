
#include <string.h>
#include <stdio.h>
#include "code/io.h"
#include "code/graph_responses.h"
#include "code/graph_stracture.h"
#include "code/table_operations.h"
#include "code/node_operations.h"

int main() {
    int res;
    struct file f;
    get_file_structure("first_db.db", &res, &f);
    if (res == FAILED) {
        fprintf(stderr, "%s", "impossible to execute program");
        return 0;
    }
    if (res == FILE_CREATED) {
        fprintf(stdout, "%s", "create_new_db");
    }
    struct attr_header ahh[]={{"d", BOOLEAN}, {"t", INT32}};
    create_table(&f, 0,"tta",2,ahh);
//    drop_table(&f, "tta");
//    create_table(&f, 0,"tta",4,ahh);

    struct vertex v;
    v.node_id = 0;

    struct attr_header_with_size ah;
    strncpy(ah.name, "b", SIZE_OF_NAME);
    ah.type = BOOLEAN;

    struct attr_body a;
    a.value.boolean = true;
    struct attr attr1={ah,a};

    struct attr_header_with_size ah2={};
    strncpy(ah.name, "t", SIZE_OF_NAME);
    ah.type = INT32;

    struct attr_body a2={};
    a.value.int32 = 5;
    struct attr attr2={ah2,a2};

    struct attr attrs[2] = {attr1, attr2};

//    add_node(&f, "tta", &v, attrs);

}