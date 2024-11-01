#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

int    iTk;
Token* consumed;

// considering to change param to Token*
bool consume(int code) {
    if (tokens[iTk].code == code) {
        consumed = &tokens[iTk++];
        return true;
    }
    return false;
}

bool baseType(Token* tk) {
    switch (tk->code) {
        case TYPE_INT:
        case TYPE_REAL:
        case TYPE_STR:
            return true;
        
        default:
            err("Invalid variable type, at line %d", tk->line);
    }
}

bool def_var(Token *tk) {
    int start = iTk;
    if (consume(VAR) && consume(ID) && consume(COLON)) {
        if (baseType(tk)) {
            consume(tk->code);
            if (consume(SEMICOLON)) {
                return true;
            }
        }
    }
    iTk = start;
    return false;
}

bool func_param(Token* tk) {
    if (consume(ID) && consume(COLON)) {
        if (baseType(tk)) {
            consume(tk->code);
            return true;
        }
    }
    return false;
}

// TODO: a bit questionable.. incomplete parameter(s) case
bool func_params(Token* tk) {
    if (func_param(tk)) {
        do {
            if (tk->code != COMMA) {
                break;
            }
            consume(tk->code);
            if (!func_param(tk)) {
                err("Invalid parameter definition, at line %d", tk->line);
            }
            consume(tk->code);
        } while(1);
    }
    // means we accept functions with no params
    return true;
}

// // program ::= ( defVar | defFunc | block )* FINISH
// bool program() {
//     for (;;) {
//         if (defVar()) {
//         } else if (defFunc()) {
//         } else if (block()) {
//         } else
//             break;
//     }
//     if (consume(FINISH)) {
//         return true;
//     } else
//         err("syntax error");
//     return false;
// }

// void parse() {
//     iTk = 0;
//     program();
// }
