#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//DJB2 hash algorithm
unsigned long hash(char * key, unsigned long hash_limit)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % hash_limit;
}