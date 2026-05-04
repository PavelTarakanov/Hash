#ifndef LIST_H
#define LIST_H

#include <cstdio>
#include <sys/stat.h>

const int LEFT_BIRD = 2147483647;
const int START_LIST_SIZE = 10;
const int MAX_COMMAND_SIZE = 1000;

struct word_t{
    char* word;
    size_t word_len;
};

struct list_array_t {word_t* data;
                     unsigned int next;
                     unsigned int prev;
                     bool elem_status;};

struct list_t {list_array_t* list_array;
               int free_head;
               unsigned int list_len;
               unsigned int real_list_len;
};

bool list_init(list_t* list, unsigned int list_size);
int list_upgrade(list_t* list);
void list_free(list_t* list);
int list_verify(list_t* list);
int list_dump(list_t* list, char* const dump_file_name, FILE* const html_dump_address);
int list_delete(list_t* list, unsigned int elem, char* const dump_file_name, FILE* const html_dump_address);
int list_insert(list_t* list, word_t* value, unsigned int elem, char* const dump_file_name, FILE* const html_dump_address);
void start_html_dump(FILE* html_dump_address);

bool check_file_founded(int argc, int number_of_files);

#endif //LIST_H
