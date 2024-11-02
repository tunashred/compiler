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

bool baseType() {
    switch (tokens[iTk].code) {
        case TYPE_INT:
        case TYPE_REAL:
        case TYPE_STR:
            consume(tokens[iTk].code);
            return true;
        
        default:
            err("Invalid variable type, at line %d", tokens[iTk].line);
    }
}

bool def_var() {
    int start = iTk;
    if (consume(VAR) && consume(ID) && consume(COLON)) {
        if (baseType()) {
            consume(tokens[iTk].code);
            if (consume(SEMICOLON)) {
                return true;
            }
        }
    }
    iTk = start;
    return false;
}

bool func_param() {
    if (consume(ID) && consume(COLON)) {
        if (baseType()) {
            return true;
        }
    }
    return false;
}

bool func_params() {
    if (func_param()) {
        while (consume(COMMA)) {
            if (!func_param()) {
                err("Invalid parameter definition, at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

bool block() {
    return true;
}

bool def_func() {
    int start = iTk;
    if (consume(FUNCTION) && consume(ID) && consume(LPAR)) {
        if (tokens[iTk].code != RPAR) {
            func_param();
        }
        if (consume(RPAR) && consume(COLON) && baseType()) {
            while(def_var()) {}
            // will see later how to call block() here
            block();
            
            // how should i detect if the function is terminated?
            if (consume(END)) {
                return true;
            }
        }
    }

    iTk = start;
    return false;
}

// program ::= ( defVar | defFunc | block )* FINISH
// commenting stuff which is not implemented yet
bool program() {
    def_func();
    // for (;;) {
    //     if (def_var()) {
    //     } else if (def_func()) {
    //     } else if (block()) {
    //     } else
    //         break;
    // }
    // if (consume(FINISH)) {
    //     return true;
    // } else
    //     err("syntax error");
    // return false;
}

void parse() {
    iTk = 0;
    program();
}
