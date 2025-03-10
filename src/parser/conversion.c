// This file contains methods that are used to convert certain token types to others.
// Example: 'a' = 97

#include <stdlib.h>
#include <stdbool.h>

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

bool isHex(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F'))) {
            return false;
        }
    }
    return true;
}

bool isOctal(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!(str[i] >= '0' && str[i] <= '7')) {
            return false;
        }
    }
    return true;
}
