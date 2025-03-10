// This file contains methods that are used to convert certain token types to others.
// Example: 'a' = 97

#include <stdlib.h>

int charToNum(char c) {
    return (int)c;
}

char numToChar(int n) {
    return (char)n;
}

int hexToInt(char *hex) {
    return (int)strtol(hex, NULL, 16);
}

int octalToInt(char *octal) {
    return (int)strtol(octal, NULL, 8);
}