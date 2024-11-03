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

bool expr();

bool factor() {
    int start = iTk;
    if (consume(LITERAL_INT) || consume(LITERAL_REAL) || consume(LITERAL_STR)) {
        return true;
    } else if (consume(LPAR) && expr() && consume(RPAR)) {
        return true;
    } else if (consume(ID)) {
        if (consume(LPAR)) {
            if (expr()) {
                while (consume(COMMA)) {
                    if (!expr()) {
                        err("Invalid expression, at line %d", tokens[iTk].line);
                    }
                }
            }
            if (!consume(RPAR)) {
                err("Expression missing ')', at line %d", tokens[iTk].line);
            }
            return true;
        }
    }

    iTk = start;
    return false;
}

// is this where I should handle signed numbers?
bool expr_prefix() {
    // maybe here it would pass the code of the operator which prefixes the potential literal val
    // and then factor() would handle its signedness
    if (consume(SUB) || consume(NOT)) { }
    if (!factor()) {
        err("Invalid prefix?, at line %d", tokens[iTk].line);
    }
    return true;
}

bool expr_mul() {
    if (expr_prefix()) {
        while (consume(MUL) || consume(DIV)) {
            if (!expr_prefix()) {
                // it would be nice to have more explicit error, like:
                // Missing expression after operand '/' or '*'
                // Missing expression at division/multiplication
                err("Missing expression after operand '*' or '/', at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

bool expr_add() {
    if (expr_mul()) {
        while (consume(ADD) || consume(SUB)) {
            if (!expr_mul()) {
                err("Missing expression after operand '+' or '-', at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

bool expr_comp() {
    if (expr_add()) {
        while (consume(EQUAL) || consume(NOT_EQ) || consume(LESS) || consume(GREATER) || consume(GREATER_EQ)) {
            if (!expr_add()) {
                err("Missing expression after comparation operand, at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

bool expr_assign() {
    if (consume(ID)) {
        if (!consume(ASSIGN)) {
            err("Invalid assign expression, at line %d", tokens[iTk].line);
        }
    }
    if (!expr_comp()) {
        // not sure if I ever need this
        err("Something wrong inside expr_assign(), at line %d", tokens[iTk].line);
    }
    return false;
}

bool expr_logic() {
    if (expr_assign()) {
        while (consume(AND) || consume(OR)) {
            if (!expr_assign()) {
                err("Malformed expression logic, at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

bool expr() {
    return expr_logic();
}

bool block();

bool instr() {
    int start = iTk;
    // I think I would get an error if the *optional* expression is missing
    if (expr()) {
        if(!consume(SEMICOLON)) {
            err("Missing semicolon after expression, at line %d", tokens[iTk].line);
        }
        return true;
    }

    if (consume(IF) && consume(LPAR) && expr() && consume(RPAR) && block()) {
        if (consume(ELSE)) {
            if (!block()) {
                err("Else statement missing block, at line %d", tokens[iTk].line);
            }
        }
        if (!consume(END)) {
            // questionable about throwing error from here
            // mainly because I dont know how to detect not terminated if statements
        }
        return true;
    }

    if (consume(RETURN)) {
        if (block()) {
            if (consume(SEMICOLON)) {
                return true;
            }
        }
        err("Malformed return statement, at line %d", tokens[iTk].line);
    }

    if (consume(WHILE)) {
        if (!consume(LPAR)) {
            err("Missing left paranthesis after \"while\" statement, at line %d", tokens[iTk].line);
        }
        if (!expr()) {
            err("Missing expression inside \"while\" statement, at line %d", tokens[iTk].line);
        }
        if (!consume(RPAR)) {
            err("Missing right paranthesis at \"while\" statement, at line %d", tokens[iTk].line);
        }
        if (!block()) {
            err("\"while\" statement missing block, at line %d", tokens[iTk].line);
        }
        if (!consume(END)) {

        }
        return true;
    }

    iTk = start;
    return false;
}

bool block() {
    bool has_instr = false;
    while ( (has_instr = instr()) ) { }
    return has_instr;
}

bool func_param() {
    if (consume(ID) && consume(COLON)) {
        if (baseType()) {
            return true;
        }
        err("Missing variable type at declaration, at line %d", tokens[iTk].line);
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
    for (;;) {
        if (def_var()) {
        } else if (def_func()) {
        } else if (block()) {
        } else
            break;
    }
    if (consume(FINISH)) {
        return true;
    } else
        err("syntax error");
    return false;
}

void parse() {
    iTk = 0;
    program();
}
