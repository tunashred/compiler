#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stddef.h>

_Noreturn void err(const char* fmt, ...);

void* safeMalloc(size_t nBytes);

char* loadFile(const char* fileName);

#endif