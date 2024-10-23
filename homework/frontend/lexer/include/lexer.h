#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS 4096
extern int   numTokens;
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
} token_type;

// clang-format on

typedef enum { NAME, SYMBOL } token_print_mode;

Token* addTk(int code);

char* copy_slice(char* dst, const char* begin, const char* end);

int scan_int(const char* start);

int scan_real(const char* start);

int scan_str(const char* start);

Token* add_literal_tk(const char* start, int len, int tk_code);

void tokenize(const char* p_ch);

#endif
