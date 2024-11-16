#include <stdarg.h>
#include <stdio.h>

#include "utils.h"
#include "lexer.h"

const char* convertTokenCode(Token* tk, int mode) {
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

        case L_ROUND_PAR:
            if (mode == SYMBOL) {
                return "(";
            }
            return "L_ROUND_PAR";

        case R_ROUND_PAR:
            if (mode == SYMBOL) {
                return ")";
            }
            return "R_ROUND_PAR";

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

        case LITERAL_FLOAT:
            if (mode == SYMBOL) {
                snprintf(buffer, sizeof(buffer), "%f", tk->r);
                return buffer;
            }
            return "LITERAL_FLOAT";

        case LITERAL_STR:
            if (mode == SYMBOL) {
                return tk->text;
            }
            return "LITERAL_STR";

        case FUNCTION:
            return "FUNCTION";
        case IF:
            return "IF";
        case ELSE:
            return "ELSE";
        case WHILE:
            return "WHILE";
        case RETURN:
            return "RETURN";
        case TYPE_INT:
            return "TYPE_INT";
        case TYPE_FLOAT:
            return "TYPE_FLOAT";
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
        case SINGLE_LINE_COMMENT:
            return "SINGLE_LINE_COMMENT";
        default:
            return "UNKNOWN";
    }
}

void printTokens() {
    for (int i = 0; i < numTokens; i++) {
        Token* tk = &tokens[i];
        printf("%4d ", tk->line);
        if (tk->code == ID) {
            printf("ID:%s\n", tk->text);
        } else if (tk->code == LITERAL_INT) {
            printf("INT:%d\n", tk->i);
        } else if (tk->code == LITERAL_FLOAT) {
            printf("REAL:%f\n", tk->r);
        } else if (tk->code == LITERAL_STR) {
            printf("STR:%s\n", tk->text);
        } else {
            printf("%s\n", convertTokenCode(tk, NAME));
        }
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
