#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

_Noreturn void err(const char* fmt, ...);

void* safeMalloc(size_t nBytes);

char* loadFile(const char* fileName, size_t* ret_file_size);

const char* contains(const char* str, const char* word);

#endif