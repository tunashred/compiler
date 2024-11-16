#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS 4096
extern int numTokens;
#define MAX_STR 127

typedef struct {
    int code;
    int line;

    union {
        char   text[MAX_STR + 1];
        int    i;
        double r;
    };
} Token;

extern Token tokens[];

// clang-format off
typedef enum {
    ID,
    // keywords
    FUNCTION, IF, ELSE, WHILE, RETURN,
    TYPE_INT, TYPE_FLOAT, TYPE_STR, TYPE_VOID,
    // delimiters
    COMMA, COLON, SEMICOLON, L_ROUND_PAR, R_ROUND_PAR, L_BRACKET, R_BRACKET,
    // operators
    ADD, SUB, INC, DEC, MUL, DIV,
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT, ASSIGN, EQUAL, NOT_EQ, LESS, LESS_EQ, GREATER, GREATER_EQ,
    FUNC_RET_OP,
    // literals
    LITERAL_INT, LITERAL_FLOAT, LITERAL_STR,
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

void tokenize(const char* p_ch);

#endif
