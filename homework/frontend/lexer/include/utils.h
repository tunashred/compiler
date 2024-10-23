#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"

_Noreturn void err(const char* fmt, ...);

void* safeMalloc(size_t nBytes);

char* loadFile(const char* fileName);

const char* contains(const char* str, const char* word);

const char* convertTokenCode(Token* tk, int mode);

void printTokens();

void printPrettyTokens();

void printDebugTokens();

#endif
