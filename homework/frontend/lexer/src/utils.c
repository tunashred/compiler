#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void err(const char* fmt, ...) {
    fprintf(stderr, "Error: ");
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void* safeMalloc(size_t nBytes) {
    void* p = malloc(nBytes);
    if (!p) {
        err("Couldn't allocate enough memory: %d bytes", nBytes);
    }
    return p;
}

char* loadFile(const char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        err("Can't open file %s", fileName);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    char*  buf   = (char*)safeMalloc(file_size + 1);
    size_t nRead = fread(buf, sizeof(char), file_size, file);

    fclose(file);
    if (file_size != nRead) {
        err("Cannot read all the content of %s", fileName);
    }
    buf[file_size] = '\0';
    return buf;
}

const char* contains(const char* str, const char* word) { // TODO: need a fancier version; Rabin-Karp or KMP
    return strstr(str, word);
}

const char* convertTokenCode(Token* tk, int mode) { // TODO: there's gotta be a better way
    static char buffer[MAX_STR + 1];
    switch (tk->code) {
        case ID:
            if (mode == SYMBOL) {
                return tk->text;
            }
            return "ID";

        case COMMA:
            if (mode == SYMBOL) {
                return ",";
            }
            return "COMMA";

        case COLON:
            if (mode == SYMBOL) {
                return ":";
            }
            return "COLON";

        case SEMICOLON:
            if (mode == SYMBOL) {
                return ";";
            }
            return "SEMICOLON";

        case LPAR:
            if (mode == SYMBOL) {
                return "(";
            }
            return "LPAR";

        case RPAR:
            if (mode == SYMBOL) {
                return ")";
            }
            return "RPAR";

        case ADD:
            if (mode == SYMBOL) {
                return "+";
            }
            return "ADD";

        case SUB:
            if (mode == SYMBOL) {
                return "-";
            }
            return "SUB";

        case MUL:
            if (mode == SYMBOL) {
                return "*";
            }
            return "MUL";

        case DIV:
            if (mode == SYMBOL) {
                return "/";
            }
            return "DIV";

        case ASSIGN:
            if (mode == SYMBOL) {
                return "=";
            }
            return "ASSIGN";

        case EQUAL:
            if (mode == SYMBOL) {
                return "==";
            }
            return "EQUAL";

        case NOT_EQ:
            if (mode == SYMBOL) {
                return "!=";
            }
            return "NOT_EQ";

        case LESS:
            if (mode == SYMBOL) {
                return "<";
            }
            return "LESS";

        case GREATER:
            if (mode == SYMBOL) {
                return ">";
            }
            return "GREATER";

        case GREATER_EQ:
            if (mode == SYMBOL) {
                return ">=";
            }
            return "GREATER_EQ";

        case LITERAL_INT:
            if (mode == SYMBOL) {
                snprintf(buffer, sizeof(buffer), "%d", tk->i);
                return buffer;
            }
            return "LITERAL_INT";

        case LITERAL_REAL:
            if (mode == SYMBOL) {
                snprintf(buffer, sizeof(buffer), "%f", tk->r);
                return buffer;
            }
            return "LITERAL_REAL";

        case LITERAL_STR:
            if (mode == SYMBOL) {
                return tk->text;
            }
            return "LITERAL_STR";

        case VAR:
            return "VAR";
        case FUNCTION:
            return "FUNCTION";
        case IF:
            return "IF";
        case ELSE:
            return "ELSE";
        case WHILE:
            return "WHILE";
        case END:
            return "END";
        case RETURN:
            return "RETURN";
        case TYPE_INT:
            return "TYPE_INT";
        case TYPE_REAL:
            return "TYPE_REAL";
        case TYPE_STR:
            return "TYPE_STR";
        case FINISH:
            return "FINISH";
        case AND:
            return "AND";
        case OR:
            return "OR";
        case NOT:
            return "NOT";
        case SPACE:
            return "SPACE";
        case COMMENT:
            return "COMMENT";
        default:
            return "UNKNOWN";
    }
}

void printTokens() {
    for (int i = 0; i < numTokens; i++) {
        Token* tk = &tokens[i];
        printf("Line: %4d, code: %2d %8s\n", tk->line, tk->code, convertTokenCode(tk, NAME));
    }
}

void printPrettyTokens() {
    int current_line = 0;
    for (int i = 0; i < numTokens; i++) {
        if (current_line < tokens[i].line) {
            current_line = tokens[i].line;
            printf("\n%4d:", tokens[i].line);
        }

        printf(" %s", convertTokenCode(&tokens[i], NAME));
    }
    printf("\n\n");
}

void printDebugTokens() {
    int current_line = 0;
    for (int i = 0; i < numTokens; i++) {
        if (current_line < tokens[i].line) {
            current_line = tokens[i].line;
            printf("\n%4d:", tokens[i].line);
        }

        if (tokens[i].code == LITERAL_STR) {
            printf(" \"%s\"", convertTokenCode(&tokens[i], SYMBOL));
        } else {
            printf(" %s", convertTokenCode(&tokens[i], SYMBOL));
        }
    }
    printf("\n\n");
}
