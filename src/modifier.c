#include <stdio.h>
#include <string.h>

// This file creates an enum of all of the different types of modifiers
// i.e access modifiers (public, private), mutability modifiers (const, mutable), etc

typedef enum Modifiers = {
    STATIC,
    PUBLIC,
    PRIVATE,
    CONST,
    MUTABLE
}