#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "frontend/utils/include/utils.h"
#include "lexer.h"

Token tokens[MAX_TOKENS];
int   numTokens;

int line = 1;

Token* addTk(int code) {
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

    tk = addTk(tk_code);

    switch (tk_code) {
        case LITERAL_INT:
            tk->i = atoi(temp_str);
            break;

        case LITERAL_FLOAT:
            tk->r = atof(temp_str);
            break;
    }
    return tk;
}

void tokenize(const char* p_ch) {
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
                addTk(COMMA);
                p_ch++;
                break;
            case ':':
                addTk(COLON);
                p_ch++;
                break;
            case ';':
                addTk(SEMICOLON);
                p_ch++;
                break;
            case '(':
                addTk(L_ROUND_PAR);
                p_ch++;
                break;
            case ')':
                addTk(R_ROUND_PAR);
                p_ch++;
                break;
            case '{':
                addTk(L_BRACKET);
                p_ch++;
                break;
            case '}':
                addTk(R_BRACKET);
                p_ch++;
                break;

            /*
                OPERATORS
            */

            case '+':
                if (p_ch[1] == '+') {
                    addTk(INC);
                    p_ch += 2;
                } else {
                    addTk(ADD);
                    p_ch++;
                }
                break;
            case '-':
                if (p_ch[1] == '-') {
                    addTk(DEC);
                    p_ch += 2;
                } else if (p_ch[1] == '>') {
                    addTk(FUNC_RET_OP);
                    p_ch += 2;
                } else {
                    addTk(SUB);
                    p_ch++;
                }
                break;
            case '*':
                addTk(MUL);
                p_ch++;
                break;
            case '/':
                if (p_ch[1] == '/') {
                    for (start = p_ch++; *p_ch != '\n'; p_ch++) {}
                    line++;
                    p_ch++;
                    break;
                }
                addTk(DIV);
                p_ch++;
                break;

            case '~':
                addTk(BITWISE_NOT);
                p_ch++;
                break;
            case '^':
                addTk(BITWISE_XOR);
                p_ch++;
                break;
            case '&':
                if (p_ch[1] == '&') {
                    addTk(LOGICAL_AND);
                    p_ch += 2;
                } else {
                    addTk(BITWISE_AND);
                    p_ch++;
                }
                break;
            case '|':
                if (p_ch[1] == '|') {
                    addTk(LOGICAL_OR);
                    p_ch += 2;
                } else {
                    addTk(BITWISE_OR);
                    p_ch++;
                }
                break;
            case '!':
                if (p_ch[1] == '=') {
                    addTk(NOT_EQ);
                    p_ch += 2;
                } else {
                    addTk(LOGICAL_NOT);
                    p_ch++;
                }
                break;

            case '=':
                if (p_ch[1] == '=') {
                    addTk(EQUAL);
                    p_ch += 2;
                } else {
                    addTk(ASSIGN);
                    p_ch++;
                }
                break;
            case '<':
                if (p_ch[1] == '=') {
                    addTk(LESS_EQ);
                    p_ch += 2;
                } else {
                    addTk(LESS);
                    p_ch++;
                }
                break;
            case '>':
                if (p_ch[1] == '=') {
                    addTk(GREATER_EQ);
                    p_ch += 2;
                } else {
                    addTk(GREATER);
                    p_ch++;
                }
                break;

            case '\0':
                addTk(FINISH);
                return;

            /*
                IDENTIFIERS AND KEYWORDS
            */

            default:
                if ((isalpha(*p_ch) || *p_ch == '_')) {
                    for (start = p_ch++; isalnum(*p_ch) || *p_ch == '_'; p_ch++) {}
                    char* text = copy_slice(buffer, start, p_ch);
                    if (!strcmp(text, "int")) {
                        addTk(TYPE_INT);
                    } else if (!strcmp(text, "float")) {
                        addTk(TYPE_FLOAT);
                    } else if (!strcmp(text, "str")) {
                        addTk(TYPE_STR);
                    } else if (!strcmp(text, "void")) {
                        addTk(TYPE_VOID);
                    } else if (!strcmp(text, "fun")) {
                        addTk(FUNCTION);
                    } else if (!strcmp(text, "if")) {
                        addTk(IF);
                    } else if (!strcmp(text, "else")) {
                        addTk(ELSE);
                    } else if (!strcmp(text, "while")) {
                        addTk(WHILE);
                    } else if (!strcmp(text, "return")) {
                        addTk(RETURN);
                    } else {
                        tk = addTk(ID);
                        strcpy(tk->text, text);
                    }

                /*
                    LITERALS
                */

                } else if (isdigit(*p_ch)) {
                    if ((len = scan_real(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + len);
                        tk       = addTk(LITERAL_FLOAT);
                        tk->r    = atof(temp_str);
                    } else if ((len = scan_int(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + len);
                        tk       = addTk(LITERAL_INT);
                        tk->i    = atoi(temp_str);
                    } else {
                        err("Invalid number literal, at line %d", line);
                    }
                    p_ch += len;

                } else if (*p_ch == '"') {
                    len = scan_str(p_ch);
                    p_ch++;
                    temp_str = copy_slice(buffer, p_ch, p_ch + len - 1);
                    tk       = addTk(LITERAL_STR);
                    strcpy(tk->text, temp_str);
                    p_ch += len;
                } else {
                    err("Invalid char '%c', at line: %d", *p_ch, line);
                }
        }
    }
}
