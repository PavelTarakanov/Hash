#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "list.h"
#include "hash.h"
#include "file_using.h"

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

    if (hash_table_init(&hash_table, hash6_optimised, HASH_LIST_SIZE))
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

errors hash_table_init(hash_table_t** hash_table, int (*hash)(word_t*),
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

    if (fread(*buffer, sizeof(char), statistics->st_size, input_address) == 0)
        return TEXT_READING_ERROR;
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

    char* word_begin = buffer;
    size_t word_len = 0;
    char* buffer_end = buffer + buffer_len;
    bool flag = false;
    word_t* word = (word_t*) calloc(1, sizeof(word_t));

    while (buffer < buffer_end)
    {
        if ((isblank(*buffer) || *buffer == '\n') && (flag == true))
        {
            flag = false;
            *buffer = '\0';
            word->word = word_begin;
            word->word_len = word_len;
            word_len = 0;
            //printf("hash(%s) = %d\n", word->word, (hash_table->hash_func)(word));
            for (int i = 0; i < 300; i++)
                if (find_word(hash_table, word))
                {
                    printf("Word not founded\n");
                    return WORD_FOUNDING_ERROR;
                }

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

    free(word);

    return NO_ERRORS;
}

errors find_word(hash_table_t* hash_table, word_t* word)
{
    assert(hash_table);
    assert(word);

    int word_hash = hash_table->hash_func(word);
    int list_number = (hash_table->list_array[word_hash]).list_array[0].next;

    for (unsigned int i = 0; i < (hash_table->list_array[word_hash]).real_list_len; i++)
    {
        if (word->word_len == (hash_table->list_array[word_hash]).list_array[list_number].data->word_len)
            if (strcmp_asm(word->word, (hash_table->list_array[word_hash]).list_array[list_number].data->word) == 0)
                return NO_ERRORS;
        list_number = (hash_table->list_array[word_hash]).list_array[list_number].next;
    }

    return WORD_FOUNDING_ERROR;
}

int strcmp_asm(const char* str1, const char* str2)
{
    assert(str1);
    assert(str2);

    int result;

    __asm__ volatile (
        ".intel_syntax noprefix\n"
        "1:\n"
        "mov al, [rdi]\n"
        "mov bl, [rsi]\n"
        "cmp al, bl\n"
        "jne 2f\n"
        "test al, al\n"
        "je 3f\n"
        "inc rdi\n"
        "inc rsi\n"
        "jmp 1b\n"
        "2:\n"
        "movzx eax, al\n"
        "movzx ecx, bl\n"
        "sub eax, ecx\n"
        "jmp 4f\n"
        "3:\n"
        "xor eax, eax\n"
        "4:\n"
        ".att_syntax prefix\n"
        : "=a" (result)
        : "D" (str1), "S" (str2)
        : "ecx", "ebx", "cc", "memory"
    );

    return result;
}
