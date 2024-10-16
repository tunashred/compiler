#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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

int is_int_literal(const char* p_ch) {
    const char* p_temp = p_ch;
    while(isdigit(*p_temp)) {
        p_temp++;
    }
    return p_temp - p_ch;
}

void tokenize(const char* p_ch) {
    const char* start;
    Token* tk;
    char buf[MAX_STR + 1];
    for (;;) {
        int prev_tk_code = tokens[numTokens - 1].code;
        switch (*p_ch) {
            case '#':
                for(start = p_ch++; *p_ch != '\n'; p_ch++) {}
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
            case '*': // hmm, this means the language does not support pointers
                addTk(MUL);
                p_ch++;
                break;
            case '/':
                addTk(DIV);
                p_ch++;
                break;

            case '&':
                addTk(AND);
                p_ch++;
                break;
            case '|':
                addTk(OR);
                p_ch++;
                break;
            case '!':
                if(p_ch[1] == '=') {
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
                if(p_ch[1] == '=') {
                    addTk(GREATER_EQ);
                    p_ch += 2;
                } else {
                    addTk(GREATER);
                }

            case '\0':
                addTk(FINISH);
                return;

            default:
                if ( (isalpha(*p_ch) || *p_ch == '_')) {
                    for (start = p_ch++; isalnum(*p_ch) || *p_ch == '_'; p_ch++) {}
                    char* text = copy_slice(buf, start, p_ch);
                    if (!strcmp(text, "int")) {
                        addTk(TYPE_INT);
                    } else if (!strcmp(text, "float")) { // do we want double or float?
                        addTk(TYPE_REAL);
                    } else if (!strcmp(text, "char")) {
                        addTk(TYPE_STR);
                    } else { // this case occurs more often than the ones above. I should move it higher
                        tk = addTk(ID);
                        strcpy(tk->text, text);
                    }
                } else if (isdigit(*p_ch)) {
                    int end;
                    if( (end = is_int_literal(p_ch)) ) {
                        char* int_string = copy_slice(buf, p_ch, p_ch + end);
                        tk = addTk(LITERAL_INT);
                        tk->i = atoi(int_string);
                    }
                    p_ch += end;
                } else {
                    err("Invalid char: %c (%d)", *p_ch, *p_ch);
                }
        }
    }
}

const char* getTokenTypeName(token_types code) {
    switch (code) {
        case ID: return "ID";
        case VAR: return "VAR";
        case FUNCTION: return "FUNCTION";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case END: return "END";
        case RETURN: return "RETURN";
        case TYPE_INT: return "TYPE_INT";
        case TYPE_REAL: return "TYPE_REAL";
        case TYPE_STR: return "TYPE_STR";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case FINISH: return "FINISH";
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case ASSIGN: return "ASSIGN";
        case EQUAL: return "EQUAL";
        case NOT_EQ: return "NOT_EQ";
        case LESS: return "LESS";
        case GREATER: return "GREATER";
        case GREATER_EQ: return "GREATER_EQ";
        case LITERAL_INT: return "LITERAL_INT";
        case LITERAL_REAL: return "LITERAL_REAL";
        case LITERAL_STR: return "LITERAL_STR";
        case SPACE: return "SPACE";
        case COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

void showTokens() {
    for (int i = 0; i < numTokens; i++) {
        Token* tk = &tokens[i];
        printf("Line: %4d, code: %2d %8s\n", tk->line, tk->code, getTokenTypeName(tk->code));
    }
}
