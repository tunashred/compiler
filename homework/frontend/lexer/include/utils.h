#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stddef.h>
#include <stdbool.h>

#include "lexer.h"

_Noreturn void err(const char* fmt, ...);

void* safeMalloc(size_t nBytes);

char* loadFile(const char* fileName);

const char* contains(const char* str, const char* word);

const char* getTokenTypeName(token_types code, int mode);

void printTokens();

void printPrettyTokens();

void printDebugTokens();

#endif