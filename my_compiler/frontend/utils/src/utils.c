#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"

void err(const char* fmt, ...) {
    fprintf(stderr, "Error: ");
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void* safeMalloc(size_t nBytes) {
    void* p = malloc(nBytes);
    if (!p) {
        err("Couldn't allocate enough memory: %d bytes", nBytes);
    }
    return p;
}

char* loadFile(const char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        err("Can't open file %s", fileName);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    char*  buf   = (char*)safeMalloc(file_size + 1);
    size_t nRead = fread(buf, sizeof(char), file_size, file);

    fclose(file);
    if (file_size != nRead) {
        err("Cannot read all the content of %s", fileName);
    }
    buf[file_size] = '\0';
    return buf;
}

const char* contains(const char* str, const char* word) { // TODO: need a fancier version; Rabin-Karp or KMP
    return strstr(str, word);
}
