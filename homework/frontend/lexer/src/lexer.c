#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

Token tokens[MAX_TOKENS];
int numTokens;

int line = 1;

Token* addTk(int code) {
    if (numTokens == MAX_TOKENS) {
        err("Reached maximum number of tokens\n");
    }
    Token* tk = &tokens[numTokens];
    tk->code = code;
    tk->line = line;
    numTokens++;
    return tk;
}

char* copy_slice(char* dst, const char* begin, const char* end) {
    char* new_str = dst;
    if (end - begin > MAX_STR) {
        err("String is too long\n");
    }

    while (begin != end) {
        *new_str++ = *begin++;
    }
    *new_str = '\0';
    return dst;
}

void tokenize(const char* p_ch) {
    const char* start;
    Token* tk;
    char buf[MAX_STR + 1];
    for (;;) {
        switch (*p_ch) {
            case '#':
                for(start = p_ch++; *p_ch != '\n'; p_ch++) {}
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

            case '=':
                if (p_ch[1] == '=') {
                    addTk(EQUAL);
                    p_ch += 2;
                } else {
                    addTk(ASSIGN);
                    p_ch++;
                }
                break;

            case '\0':
                addTk(FINISH);
                return;

            default:
                if (isalpha(*p_ch) || *p_ch == '_') {
                    for (start = p_ch++; isalnum(*p_ch) || *p_ch == '_'; p_ch++) {}
                    char* text = copy_slice(buf, start, p_ch);
                    if (!strcmp(text, "int")) {
                        addTk(TYPE_INT);
                    } else if (!strcmp(text, "float")) { // do we want double or float?
                        addTk(TYPE_REAL);
                    } else if (!strcmp(text, "char")) {
                        addTk(TYPE_STR);
                    } else {
                        tk = addTk(ID);
                        strcpy(tk->text, text);
                    }
                } else {
                    err("Invalid char: %c (%d)", *p_ch, *p_ch);
                }
        }
    }
}

void showTokens() {
    for (int i = 0; i < numTokens; i++) {
        Token* tk = &tokens[i];
        printf("Line: %d, code: %d\n", tk->line, tk->code);
    }
}
