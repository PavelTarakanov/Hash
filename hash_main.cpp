#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "hash.h"
#include "file_using.h"

//TODO KCachgrind valgrind

int main(int argc, char* argv[])
{
    FILE* html_dump_address = NULL;
    FILE* text_address = NULL;
    FILE* finding_text_address = NULL;
    struct stat statistics = {};
    char* buffer = NULL;
    unsigned long int buffer_len = 0;
    char* finding_buffer = NULL;
    unsigned long int finding_buffer_len = 0;
    hash_table_t* hash_table = NULL;

    if (hash_table_init(&hash_table, hash5, HASH_LIST_SIZE))
        return HASH_TABLE_MAKING_ERROR;

    if (check_file_founded(argc, NUMBER_OF_FILES))
        return FILES_NOT_FOUNED;

    if (check_file_opening(argv[1], &html_dump_address, "w"))
        return FILE_OPENING_ERROR;

    if (check_file_opening(argv[2], &text_address, "r"))
        return FILE_OPENING_ERROR;

    if (check_file_opening(argv[3], &finding_text_address, "r"))
        return FILE_OPENING_ERROR;

    if (read_text(text_address, &statistics, &buffer, &buffer_len))
        return TEXT_READING_ERROR;

    if (make_hash_table(buffer, hash_table, buffer_len, html_dump_address))
        return HASH_TABLE_MAKING_ERROR;

    if (read_text(finding_text_address, &statistics, &finding_buffer, &finding_buffer_len))
        return TEXT_READING_ERROR;

    if (find_words(hash_table, finding_buffer, finding_buffer_len))
        return ERROR_WHILE_FINDING_WORDS;

    if (check_file_closing(html_dump_address))
        printf("Error while clossing file. Programm was finished\n");

    if (check_file_closing(text_address))
        printf("Error while clossing file. Programm was finished\n");

    if (check_file_closing(finding_text_address))
        printf("Error while clossing file. Programm was finished\n");

    free(buffer);
    free(finding_buffer);
    hash_table_destroy(hash_table);

    return NO_ERRORS;
}

errors hash_table_init(hash_table_t** hash_table, int (*hash)(char*),
                       unsigned int list_array_len)
{
    *hash_table = (hash_table_t*) calloc(1, sizeof(hash_table_t));

    if (*hash_table == NULL)
        return ALLOCATION_ERROR;

    (*hash_table)->list_array_len = list_array_len;
    (*hash_table)->list_array = (list_t*) calloc(list_array_len, sizeof(list_t));

    if ((*hash_table)->list_array == NULL)
        return ALLOCATION_ERROR;

    for (unsigned int i = 0; i < list_array_len; i++)
    {
        list_init(&(((*hash_table)->list_array)[i]), START_LIST_SIZE);
    }

    (*hash_table)->hash_func = hash;

    return NO_ERRORS;
}

errors read_text(FILE* input_address, struct stat* statistics, char** buffer,
                long unsigned int* buffer_len)
{
    if(fstat(fileno(input_address), statistics))
        return FILE_STATISTICS_ERROR;

    *buffer = (char*) calloc(statistics->st_size + 1, sizeof(char));
    *buffer_len = statistics->st_size;

    if (*buffer == NULL)
        return ALLOCATION_ERROR;

    fread(*buffer, sizeof(char), statistics->st_size, input_address);
    (*buffer)[statistics->st_size] = '\0';

    return NO_ERRORS;
}

void hash_table_destroy(hash_table_t* hash_table)
{
    for (unsigned int i = 0; i < hash_table->list_array_len; i++)
        list_free(&(hash_table->list_array[i]));

    free(hash_table->list_array);
    free(hash_table);
}

errors find_words(hash_table_t* hash_table, char* buffer, unsigned long int buffer_len)
{
    assert(hash_table);
    assert(buffer);

    char* word = buffer;
    char* buffer_end = buffer + buffer_len;
    bool flag = false;

    while (buffer < buffer_end)
    {
        if ((isblank(*buffer) || *buffer == '\n') && (flag == true))
        {
            flag = false;
            *buffer = '\0';
            //printf("hash(%s) = %d\n", word, (hash_table->hash_func)(word));
            if (find_word(hash_table, word))
            {
                printf("Word not founded\n");
                return WORD_FOUNDING_ERROR;
            }
        }
        else
            if (flag == false)
            {
                word = buffer;
                flag = true;
            }


        buffer++;
    }

    return NO_ERRORS;
}

errors find_word(hash_table_t* hash_table, char* word)
{
    assert(hash_table);
    assert(word);

    int word_hash = hash_table->hash_func(word);
    int list_number = (hash_table->list_array[word_hash]).list_array[0].next;

    for (unsigned int i = 0; i < (hash_table->list_array[word_hash]).real_list_len; i++)
    {
        if (strcmp(word, (hash_table->list_array[word_hash]).list_array[list_number].data) == 0)
            return NO_ERRORS;
        list_number = (hash_table->list_array[word_hash]).list_array[list_number].next;
    }

    return WORD_FOUNDING_ERROR;
}
