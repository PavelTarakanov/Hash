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

errors make_hash_table(char* buffer, hash_table_t* hash_table,
                       long unsigned int buffer_len, FILE* html_dump_address)
{
    assert(buffer);
    assert(hash_table);

    char* word_begin = buffer;
    size_t word_len = 0;
    char* buffer_end = buffer + buffer_len;
    bool flag = false;
    word_t* word = NULL;

    while (buffer < buffer_end)
    {
        if ((isblank(*buffer) || *buffer == '\n') && (flag == true))
        {
            flag = false;
            *buffer = '\0';
            word = (word_t*) calloc(1, sizeof(word_t));
            word->word = word_begin;
            word->word_len = word_len;
            word_len = 0;

            //printf("hash(%s) = %d, len = %lu\n", word->word, (hash_table->hash_func)(word), word->word_len);
            hash_table_add(hash_table, word, html_dump_address);
        }
        else
            if (flag == false)
            {
                word_begin = buffer;
                flag = true;
            }


        buffer++;

        if (flag == true)
            word_len++;
    }

    return NO_ERRORS;
}

errors hash_table_add(hash_table_t* hash_table, word_t* word, FILE* html_dump_address)
{
    assert(hash_table);
    assert(word);

    char dump_file_name[] = "dump.txt";
    int word_hash = hash_table->hash_func(word);
    int list_number = (hash_table->list_array[word_hash]).list_array[0].next;

    for (unsigned int i = 0; i < (hash_table->list_array[word_hash]).real_list_len; i++)
    {
        if (strcmp(word->word, (hash_table->list_array[word_hash]).list_array[list_number].data->word) == 0)
        {
            free(word);
            return NO_ERRORS;
        }
        list_number = (hash_table->list_array[word_hash]).list_array[list_number].next;
    }

    list_insert(&(hash_table->list_array[word_hash]), word, (hash_table->list_array[word_hash]).real_list_len, dump_file_name, html_dump_address);

    return NO_ERRORS;
}
