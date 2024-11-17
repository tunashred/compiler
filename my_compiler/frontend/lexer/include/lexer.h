#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#define MAX_TOKENS 4096
extern int numTokens;
#define MAX_STR 127

typedef struct {
    int code;
    int line;

    union {
        char c;
        char  text[MAX_STR + 1];
        int   i;
        float r;
    };
} Token;

extern Token tokens[];

// clang-format off
typedef enum {
    ID,
    // keywords
    FUN, IF, ELIF,  ELSE, WHILE, RETURN,
    TYPE_INT, TYPE_FLOAT, TYPE_CHARRAY, TYPE_VOID,
    // delimiters
    SINGLE_QUOTE, DOUBLE_QUOTE, COMMA, COLON, SEMICOLON,
    L_ROUND_PAR, R_ROUND_PAR, L_SQUARE_PAR, R_SQUARE_PAR, L_BRACKET, R_BRACKET,
    // operators
    ADD, SUB, INC, DEC, MUL, DIV,
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT, ASSIGN, EQUAL, NOT_EQ, LESS, LESS_EQ, GREATER, GREATER_EQ,
    FUNC_RET_OP,
    // literals
    LITERAL_INT, LITERAL_FLOAT, LITERAL_CHAR, LITERAL_STR,
    // non-tokens
    SPACE, SINGLE_LINE_COMMENT, MULTI_LINE_COMMENT,
    FINISH
} token_type;

// clang-format on

typedef enum { NAME, SYMBOL } token_print_mode;

Token* add_token(int code);

char* copy_slice(char* dst, const char* begin, const char* end);

int scan_int(const char* start);

int scan_real(const char* start);

int scan_str(const char* start);

Token* add_literal_tk(const char* start, int len, int tk_code);

void tokenize(const char* p_ch, size_t buffer_size);

#endif
