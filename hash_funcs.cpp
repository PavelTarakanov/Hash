#include <string.h>
#include <assert.h>
#include "hash.h"

int hash1(char* word)
{
    assert(word);

    return 0;
}

int hash2(char* word)
{
    assert(word);

    return (int) word[0];
}

int hash3(char* word)
{    assert(word);

    return (int) strlen(word) % HASH_LIST_SIZE;
}

int hash4(char* word)
{
    assert(word);

    size_t len = strlen(word);
    int sum = 0;

    for (size_t i = 0; i < len; i++)
        sum += (int) word[i];

    return sum % HASH_LIST_SIZE;
}

int hash5(char* word)
{
    assert(word);

    size_t len = strlen(word);
    int hash = 0;

    for (size_t i = 0; i < len; i++)
    {
        hash = hash << 1;
        hash = hash ^ (int) word[i];
    }

    return hash % HASH_LIST_SIZE;
}
