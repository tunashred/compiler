#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    ID
    // keywords
    ,
    TYPE_INT
    // delimiters
    ,
    COMMA,
    FINISH
    // operators
    ,
    ASSIGN,
    EQUAL
};

#define MAX_STR 127

typedef struct {
    int code;
    int line;

    union {
        char text[MAX_STR + 1];
        int i;
        double r;
    };
} Token;

#define MAX_TOKENS 4096
extern Token tokens[];
extern int numTokens;

void tokenize(const char* pch);
void showTokens();

#endif