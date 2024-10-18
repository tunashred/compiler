#ifndef LEXER_H
#define LEXER_H

typedef enum {
    ID,
    // keywords
    VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN,
    TYPE_INT, TYPE_REAL, TYPE_STR,
    // delimiters
    COMMA, COLON, SEMICOLON, LPAR, RPAR, FINISH,
    // operators
    ADD, SUB, MUL, DIV,
    AND, OR, NOT, ASSIGN, EQUAL, NOT_EQ, LESS, GREATER, GREATER_EQ,
    // literals
    LITERAL_INT, LITERAL_REAL, LITERAL_STR,
    // non-tokens
    SPACE, COMMENT
} token_types;

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

Token* addTk(int code);

char* copy_slice(char* dst, const char* begin, const char* end);

int scan_int(const char* start);

int scan_real(const char* start);

int scan_str(const char* start);

void tokenize(const char* pch);

const char* getTokenTypeName(token_types code);

void showTokens();

void showPrettyTokens();

#endif
