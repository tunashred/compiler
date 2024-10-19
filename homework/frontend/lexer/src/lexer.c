#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

Token tokens[MAX_TOKENS];
int   numTokens;

int line = 1;

Token* addTk(int code) {
    if (numTokens == MAX_TOKENS) {
        err("Reached maximum number of tokens\n");
    }
    Token* tk = &tokens[numTokens];
    tk->code  = code;
    tk->line  = line;
    numTokens++;
    return tk;
}

char* copy_slice(char* dst, const char* begin, const char* end) {
    char* new_str = dst;
    if (end - begin > MAX_STR) {
        err("String is too long at line %d\n", line);
    }

    while (begin != end) {
        *new_str++ = *begin++;
    }
    *new_str = '\0';
    return dst;
}

int scan_int(const char* start) {
    const char* current = start;
    while (isdigit(*current)) {
        current++;
    }

    return current - start;
}

int scan_real(const char* start) {
    const char* current               = start;
    bool        has_digits_before_dot = false, has_digits_after_dot = false;

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

    if (!has_digits_before_dot && !has_digits_after_dot) {
        return 0;
    }

    return current - start;
}

int scan_str(const char* start) {
    const char* current = start;
    current++;

    while (*current != '"') {
        current++;
    }

    return current - start;
}

void tokenize(const char* p_ch) {
    const char* start;
    Token*      tk;
    char        buffer[MAX_STR + 1];
    for (;;) {
        switch (*p_ch) {
            case '#':
                for (start = p_ch++; *p_ch != '\n'; p_ch++) {}
                line++;
                p_ch++;
                break;
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
                addTk(LPAR);
                p_ch++;
                break;
            case ')':
                addTk(RPAR);
                p_ch++;
                break;

            case '+': // maybe for increment too; ++
                addTk(ADD);
                p_ch++;
                break;
            case '-':
                addTk(SUB);
                p_ch++;
                break;
            case '*':
                addTk(MUL);
                p_ch++;
                break;
            case '/':
                addTk(DIV);
                p_ch++;
                break;

            case '&':
                if (p_ch[1] == '&') {
                    addTk(AND);
                }
                p_ch++;
                break;
            case '|':
                if (p_ch[1] == '|') {
                    addTk(OR);
                }
                p_ch++;
                break;
            case '!':
                if (p_ch[1] == '=') {
                    addTk(NOT_EQ);
                    p_ch += 2;
                } else {
                    addTk(NOT);
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
                addTk(LESS);
                p_ch++;
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

            default:
                if ((isalpha(*p_ch) || *p_ch == '_')) {
                    for (start = p_ch++; isalnum(*p_ch) || *p_ch == '_'; p_ch++) {}
                    char* text = copy_slice(buffer, start, p_ch);
                    if (!strcmp(text, "int")) {
                        addTk(TYPE_INT);
                    } else if (!strcmp(text, "float")) { // do we want double or float?
                        addTk(TYPE_REAL);
                    } else if (!strcmp(text, "char")) {
                        addTk(TYPE_STR);
                    } else if (!strcmp(text, "var")) {
                        addTk(VAR);
                    } else if (!strcmp(text, "function")) {
                        addTk(FUNCTION);
                    } else if (!strcmp(text, "if")) {
                        addTk(IF);
                    } else if (!strcmp(text, "else")) {
                        addTk(ELSE);
                    } else if (!strcmp(text, "while")) {
                        addTk(WHILE);
                    } else if (!strcmp(text, "end")) {
                        addTk(END);
                    } else if (!strcmp(text, "return")) {
                        addTk(RETURN);
                    } else { // this case occurs more often than the ones above. I should move it higher
                        tk = addTk(ID);
                        strcpy(tk->text, text);
                    }

                } else if (isdigit(*p_ch)) {
                    int   end;
                    char* temp_str;
                    if ((end = scan_real(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + end);
                        tk       = addTk(LITERAL_REAL);
                        tk->r    = atof(temp_str);
                    } else if ((end = scan_int(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + end);
                        tk       = addTk(LITERAL_INT);
                        tk->i    = atoi(temp_str);
                    } else if ((end = scan_str(p_ch))) {
                        temp_str = copy_slice(buffer, p_ch, p_ch + end - 1);
                        tk       = addTk(LITERAL_STR);
                        strcpy(tk->text, temp_str);
                    } else {
                        err("Invalid number literal at line %d\n", line);
                    }
                    p_ch += end;

                } else if (*p_ch == '"') {
                    int   end = scan_str(p_ch);
                    char* temp_str;
                    p_ch++;
                    temp_str = copy_slice(buffer, p_ch, p_ch + end - 1);
                    tk       = addTk(LITERAL_STR);
                    strcpy(tk->text, temp_str);
                    p_ch += end;
                } else {
                    err("Invalid char: %c (%d)", *p_ch, *p_ch);
                }
        }
    }
}
