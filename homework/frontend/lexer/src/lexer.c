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

void tokenize(const char* pch) {
    const char* start;
    Token* tk;
    char buf[MAX_STR + 1];
    for (;;) {
        switch (*pch) {
            case ' ':
            case '\t':
                pch++;
                break;
            case '\r':
                if (pch[1] == '\n') {
                    pch++;
                }
            case '\n':
                line++;
                pch++;
                break;
            case '\0':
                addTk(FINISH);
                return;
            case ',':
                addTk(COMMA);
                pch++;
                break;
            case '=':
                if (pch[1] == '=') {
                    addTk(EQUAL);
                    pch += 2;
                } else {
                    addTk(ASSIGN);
                    pch++;
                }
                break;
            default:
                if (isalpha(*pch) || *pch == '_') {
                    for (start = pch++; isalnum(*pch) || *pch == '_'; pch++) {}
                    char* text = copy_slice(buf, start, pch);
                    if (strcmp(text, "int") == 0) {
                        addTk(TYPE_INT);
                    } else {
                        tk = addTk(ID);
                        strcpy(tk->text, text);
                    }
                } else {
                    err("Invalid char: %c (%d)", *pch, *pch);
                }
        }
    }
}

void showTokens() {
    for (int i = 0; i < numTokens; i++) {
        Token* tk = &tokens[i];
        printf("%d\n", tk->code);
    }
}
