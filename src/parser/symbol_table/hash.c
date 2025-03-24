#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//DJB2 hash algorithm
unsigned long hash_key(char * key, unsigned long hash_limit)
{
    unsigned long hash = 5381;
    unsigned long c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c;

    return hash % hash_limit;
}