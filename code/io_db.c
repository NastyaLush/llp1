//#include <fcntl.h>
//#include <sys/mman.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <sys/stat.h>
//#include </mnt/c/Users/yfcni/onedrive/LLpLab/code/graphcopy.h>
//#include </mnt/c/Users/yfcni/onedrive/LLpLab/code/graph.c>
//#ifndef RW
//#define RW
//#define MIN_SIZE 1024
//#define FILE_CREATED 0
//#define FILE_OPENED 1
//#define FILE_OPEN_OR_CREATE_FAILED 2
//#define FAILED 3
//#define SUCCESS 4
//
//void *read_file(const char *const filename, int *res);
//// todo можно инициализировать аллкатор сразу!
//// дописать create_table
//
//int open_or_create_file(const char *const filename, int *res)
//{
//  int fdin;
//  *res = FILE_OPENED;
//  if ((fdin = open(filename, O_RDWR)) == -1)
//  {
//    *res = FILE_CREATED;
//    fprintf(stdout, "impossible to open file %s\n", filename);
//    fprintf(stdout, "creating new file\n");
//    if ((fdin = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
//    {
//      fprintf(stderr, "%d", fdin);
//      fprintf(stderr, "impossible to create %s\n", filename);
//      *res = FILE_OPEN_OR_CREATE_FAILED;
//      return -1;
//    }
//  }
//  return fdin;
//}
//bool resize_file(int fd, int size)
//{
//  if (ftruncate(fd, MIN_SIZE) == -1)
//  {
//    fprintf(stderr, "%s", "ftruncate");
//    return false;
//  }
//  return true;
//}
//void *read_file(const char *const filename, int *res)
//{
//
//  void *src;
//  struct stat statbuf;
//  int fdin = open_or_create_file(filename, res);
//
//  if (res == FILE_OPEN_OR_CREATE_FAILED)
//    return;
//  if (fstat(fdin, &statbuf) < 0)
//  {
//    fprintf(stderr, "fstat error\n");
//    res = FAILED;
//    return;
//  }
//  if (res == FILE_CREATED)
//  {
//    if (!resize_file(fdin, MIN_SIZE))
//    {
//      fprintf(stderr, "resize error\n");
//      res = FAILED;
//      return;
//    }
//  }
//
//  if ((src = mmap(0, MIN_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdin, 0)) == MAP_FAILED)
//  {
//    fprintf(stderr, "mmap failed\n");
//    res = FAILED;
//    return;
//  }
//  fprintf(stdout, "file mmaped\n");
//  // close_file(fdin);
//  return src;
//}
//
//void create_table(void *mmaped_f, size_t node_id, char name[SIZE_OF_NAME], size_t count_of_attr, struct attr_header name_attr[])
//{
//  struct page_header ph;
//  ph.name = name;
//  ph.next = {};
//  ph.type_block = type_block.VERTEX;
//  ph.offset_for_free_block = 0;
//
//  memcpy(mmaped_f, &ph, sizeof(struct page_header));
//
//  struct vertexes v;
//  v.node_id node_id;
//  v.name = name;
//  memcpy(mmaped_f + sizeof(struct page_header), &v, sizeof(struct vertex));
//  int added_attr = 0;
//  while (MIN_SIZE > added_attr * sizeof(struct attr_header) &&  added_attr<count_of_attr)
//  {
//    memcpy(mmaped_f + sizeof(struct page_header) + added_attr * sizeof(struct attr_header), name_attr[added_attr], sizeof(struct attr_header));
//  }
//  if(added_attr!=count_of_attr){
//    //add new page
//  }
//}
//
//void close_file(const int fdin)
//{
//  if (close(fdin))
//  {
//    fprintf(stderr, "impossible to close file\n");
//  }
//}
//
//void read_db(const char *const filename)
//{
//  bool is_exist;
//  int res;
//  void *mmaped_f = read_file(filename, &res);
//  if (res == FAILED)
//  {
//    fprintf(stderr, "%s", "impossible to execute programm");
//    return;
//  }
//  if (res == FILE_CREATED)
//  {
//    fprintf(stdout, "%s", "create_new_db");
//    create_simple_db(filename, mmaped_f);
//  }
//  struct attr_header name_attr[2]= {{node_type.REAL, "attr1"}, {node_type.BOOLEAN, "attr2"}};
//   create_table( mmaped_f, 0, "table1", 2, name_attr);
//}
//#endif