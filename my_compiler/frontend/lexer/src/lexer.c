#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "frontend/utils/include/utils.h"
#include "lexer.h"

Token tokens[MAX_TOKENS];
int   numTokens;

int line = 1;

Token* add_token(int code) {
    if (numTokens == MAX_TOKENS) {
        err("Reached maximum number of tokens: %d", MAX_TOKENS);
    }
    if (code < ID || code > FINISH) {
        err("Invalid token code %d, at line %d", code, line);
    }
    Token* tk = &tokens[numTokens];
    tk->code  = code;
    tk->line  = line;
    numTokens++;
    return tk;
}

char* copy_slice(char* dst, const char* begin, const char* end) {
    size_t len = (size_t)(end - begin);
    if (len > MAX_STR) {
        err("String is too long, at line: %d", line);
    }

    strncpy(dst, begin, len);
    dst[len] = '\0';
    return dst;
}

int scan_int(const char* start) {
    const char* current = start;
    if (isdigit(current[1])) {
        current++;
    }
    while (isdigit(*current)) {
        current++;
    }
    // needs further testing.. discovered I need this for case "123abc = 12;"
    if (isalpha(*current)) {
        err("Invalid identifier, at line %d", line);
    }
    return (int)(current - start);
}

int scan_real(const char* start) {
    const char* current               = start;
    bool        has_digits_before_dot = false, has_digits_after_dot = false;
    if (isdigit(current[1])) {
        current++;
    }

    while (isdigit(*current)) {
        current++;
        has_digits_before_dot = true;
    }

    if (*current == '.') {
        current++;
    } else {
        return 0;
    }

    while (isdigit(*current)) {
        current++;
        has_digits_after_dot = true;
    }
    if (isalpha(*current)) {
        err("Invalid identifier, at line %d", line);
    }

    if (!has_digits_before_dot || !has_digits_after_dot) {
        err("Malformed floating point number, at line: %d", line);
    }

    return (int)(current - start);
}

int scan_str(const char* start) {
    const char* current = start + 1;
    while (*current != '"' && *current != '\0') {
        if (*current == '\n') {
            err("String does not end at line it begins, at line: %d", line);
        }

        if (*current == '\\') {
            current++;
            if (*current == 'n' || *current == 'r' || *current == 't' || *current == '\\' || *current == '"') {
                current++;
            } else {
                err("Invalid escape sequence, at line: %d", line);
            }
        } else {
            current++;
        }
    }
    return (int)(current - start);
}

Token* add_literal_tk(const char* start, int len, int tk_code) {
    if (!start || len <= 0) {
        err("Bad arguments when trying to add token, at line: %d\n", line);
        return NULL;
    }

    Token* tk;
    char   buffer[MAX_STR + 1];
    char*  temp_str = copy_slice(buffer, start, start + len);

    tk = add_token(tk_code);

    switch (tk_code) {
        case LITERAL_INT:
            tk->i = atoi(temp_str);
            break;

        case LITERAL_FLOAT:
            tk->r = (float)atof(temp_str);
            break;
    }
    return tk;
}

void tokenize(const char* p_ch, size_t buffer_size) {
    const char* start;
    const char* temp_str;
    int         len;
    Token*      tk;
    char        buffer[MAX_STR + 1];

    for (;;) {
        switch (*p_ch) {
            case ' ':
            case '\t':
                p_ch++;
                break;

            case '\r':
                if (p_ch[1] == '\n') {
                    p_ch++;
                }
            case '\n':
                line++;
                p_ch++;
                break;
            /*
                DELIMITERS
            */
            case ',':
                add_token(COMMA);
                p_ch++;
                break;
            case ':':
                add_token(COLON);
                p_ch++;
                break;
            case ';':
                add_token(SEMICOLON);
                p_ch++;
                break;
            case '(':
                add_token(L_ROUND_PAR);
                p_ch++;
                break;
            case ')':
                add_token(R_ROUND_PAR);
                p_ch++;
                break;
            case '{':
                add_token(L_BRACKET);
                p_ch++;
                break;
            case '}':
                add_token(R_BRACKET);
                p_ch++;
                break;

            /*
                OPERATORS
            */

            case '+':
                if (p_ch[1] == '+') {
                    add_token(INC);
                    p_ch += 2;
                } else {
                    add_token(ADD);
                    p_ch++;
                }
                break;
            case '-':
                if (p_ch[1] == '-') {
                    add_token(DEC);
                    p_ch += 2;
                } else if (p_ch[1] == '>') {
                    add_token(FUNC_RET_OP);
                    p_ch += 2;
                } else {
                    add_token(SUB);
                    p_ch++;
                }
                break;
            case '*':
                add_token(MUL);
                p_ch++;
                break;
            case '/':
                if (p_ch[1] == '/') {
                    for (start = p_ch++; *p_ch != '\n'; p_ch++) {}
                    line++;
                    p_ch++;
                    break;
                } else if (p_ch[1] == '*') {
                    start = p_ch++;
                    while (1) {
                        if (p_ch[1] == '\0') {
                            return;
                        }
                        if (*p_ch == '*' && p_ch[1] == '/') {
                            break;
                        } else if (*p_ch == '/' && p_ch[1] == '*') {
                            err("Nested multi-line comments are not allowed, at line %d", line);
                        } else if (*p_ch == '\n') {
                            line++;
                        }
                        p_ch++;
                    }
                    p_ch += 2;
                    break;
                }
                add_token(DIV);
                p_ch++;
                break;

            case '~':
                add_token(BITWISE_NOT);
                p_ch++;
                break;
            case '^':
                add_token(BITWISE_XOR);
                p_ch++;
                break;
            case '&':
                if (p_ch[1] == '&') {
                    add_token(LOGICAL_AND);
                    p_ch += 2;
                } else {
                    add_token(BITWISE_AND);
                    p_ch++;
                }
                break;
            case '|':
                if (p_ch[1] == '|') {
                    add_token(LOGICAL_OR);
                    p_ch += 2;
                } else {
                    add_token(BITWISE_OR);
                    p_ch++;
                }
                break;
            case '!':
                if (p_ch[1] == '=') {
                    add_token(NOT_EQ);
                    p_ch += 2;
                } else {
                    add_token(LOGICAL_NOT);
                    p_ch++;
                }
                break;

            case '=':
                if (p_ch[1] == '=') {
                    add_token(EQUAL);
                    p_ch += 2;
                } else {
                    add_token(ASSIGN);
                    p_ch++;
                }
                break;
            case '<':
                if (p_ch[1] == '=') {
                    add_token(LESS_EQ);
                    p_ch += 2;
                } else {
                    add_token(LESS);
                    p_ch++;
                }
                break;
            case '>':
                if (p_ch[1] == '=') {
                    add_token(GREATER_EQ);
                    p_ch += 2;
                } else {
                    add_token(GREATER);
                    p_ch++;
                }
                break;

            case '\0':
                add_token(FINISH);
                return;

            /*
                IDENTIFIERS AND KEYWORDS
            */

            default:
                if ((isalpha(*p_ch) || *p_ch == '_')) {
                    for (start = p_ch++; isalnum(*p_ch) || *p_ch == '_'; p_ch++) {}
                    char* text = copy_slice(buffer, start, p_ch);
                    if (!strcmp(text, "int")) {
                        add_token(TYPE_INT);
                    } else if (!strcmp(text, "float")) {
                        add_token(TYPE_FLOAT);
                    } else if (!strcmp(text, "charray")) {
                        add_token(TYPE_CHARRAY);
                    } else if (!strcmp(text, "void")) {
                        add_token(TYPE_VOID);
                    } else if (!strcmp(text, "fun")) {
                        add_token(FUN);
                    } else if (!strcmp(text, "if")) {
                        add_token(IF);
                    } else if (!strcmp(text, "elif")) {
                        add_token(ELIF);
                    } else if (!strcmp(text, "else")) {
                        add_token(ELSE);
                    } else if (!strcmp(text, "while")) {
                        add_token(WHILE);
                    } else if (!strcmp(text, "return")) {
                        add_token(RETURN);
                    } else {
                        tk = add_token(ID);
                        strcpy(tk->text, text);
                    }

                /*
                    LITERALS
                */

                } else if (isdigit(*p_ch)) {
                    if ((len = scan_real(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + len);
                        tk       = add_token(LITERAL_FLOAT);
                        tk->r    = (float)atof(temp_str);
                    } else if ((len = scan_int(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + len);
                        tk       = add_token(LITERAL_INT);
                        tk->i    = atoi(temp_str);
                    } else {
                        err("Invalid number literal, at line %d", line);
                    }
                    p_ch += len;

                } else if (*p_ch == '"') {
                    len = scan_str(p_ch);
                    p_ch++;
                    temp_str = copy_slice(buffer, p_ch, p_ch + len - 1);
                    tk       = add_token(LITERAL_STR);
                    strcpy(tk->text, temp_str);
                    p_ch += len;
                } else {
                    err("Invalid char '%c', at line: %d", *p_ch, line);
                }
        }
    }
}
