#include <string.h>
#include <assert.h>
#include <nmmintrin.h>
#include "hash.h"

int hash1(word_t* word)
{
    assert(word);

    return 0;
}

int hash2(word_t* word)
{
    assert(word);

    return (int) word->word[0];
}

int hash3(word_t* word)
{
    assert(word);

    return (int) word->word_len % HASH_LIST_SIZE;
}

int hash4(word_t* word)
{
    assert(word);

    int sum = 0;

    for (int i = 0; word->word[i] != '\0'; i++)
        sum += word->word[i];

    return sum % HASH_LIST_SIZE;
}

int hash5(word_t* word)
{
    assert(word);

    int hash = 0;

    for (int i = 0; word->word[i] != '\0'; i++)
    {
        hash = (hash << 1) ^ word->word[i];
    }

    return hash % HASH_LIST_SIZE;
}

int hash6(word_t* word)
{
    assert(word);

    const int polynomial = 0xEDB88320;
    int crc = 0xFFFFFFFF;

    for (size_t i = 0; i < word->word_len; i++) {
        crc ^= (int) word->word[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
    }

    return (crc ^ 0xFFFFFFFF) % HASH_LIST_SIZE;
}

int hash6_optimised(word_t* word)
{
    assert(word);
    unsigned int crc = 0xFFFFFFFF;
    const char* data = word->word;
    size_t len = word->word_len;
    size_t i = 0;

    for (; i + 4 <= len; i += 4) {
        crc = _mm_crc32_u32(crc, *(unsigned int*)(data + i));
    }

    for (; i < len; i++) {
        crc = _mm_crc32_u8(crc, data[i]);
    }

    return (crc ^ 0xFFFFFFFF) % HASH_LIST_SIZE;
}

