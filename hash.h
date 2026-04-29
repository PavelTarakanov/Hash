#ifndef HASH_H
#define HASH_H

#include <cstdio>
#include <sys/stat.h>
#include "list.h"

const unsigned int NUMBER_OF_FILES = 2;
const unsigned int HASH_LIST_SIZE = 4001;

struct hash_table_t{
    unsigned int list_array_len;
    list_t* list_array;
    int (*hash_func)(char*);
};

enum errors{
    NO_ERRORS = 0,
    LIST_ADDRESS_ERROR = 1,
    LEFT_BIRD_ERROR = 2,
    ACCORDANCE_ERROR = 3,
    FILES_NOT_FOUNED = 4,
    FILE_OPENING_ERROR = 5,
    FILE_CLOSING_ERROR = 6,
    ERROR_WHILE_MAKING_GRAPH = 7,
    ALLOCATION_ERROR = 8,
    COMMAND_MAKING_ERROR = 9,
    FILE_STATISTICS_ERROR = 10,
    TEXT_READING_ERROR = 11,
    HASH_TABLE_MAKING_ERROR = 12
};

int hash1(char* word);
int hash2(char* word);
int hash3(char* word);
int hash4(char* word);
int hash5(char* word);

errors read_text(FILE* input_address, struct stat* statistics, char** buffer,
                 long unsigned int* buffer_len);
errors make_hash_table(char* buffer, hash_table_t* hash_table,
                       long unsigned int buffer_len, FILE* html_dump_address);
errors hash_table_add(hash_table_t* hash_table, char* word, FILE* html_dump_address)
;


errors hash_table_init(hash_table_t** hash_table, int (*hash)(char*),
                       unsigned int list_array_len);
void hash_table_destroy(hash_table_t* hash_table);

#endif //TEXT_PREPARE_H
