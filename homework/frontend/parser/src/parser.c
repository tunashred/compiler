#include <stdarg.h>
#include <stdbool.h>

#include "lexer.h"
#include "utils.h"

int    iTk;
// for better error information and handling
// it would be useful to memorise the last consumed *big* token
// that means: ID, FUNCTION, IF, WHILE
// example: function inside function?
Token* consumed;

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
    if (consume(VAR)) {
        if (!consume(ID)) {
            err("Variable declaration must contain an identifier, at line %d", tokens[iTk].line);
        }
        if (!consume(COLON)) {
            err("Variable definition must contain ':' after identifier, at line %d", tokens[iTk].line);
        }
        if (baseType()) {
            if (consume(SEMICOLON)) {
                return true;
            }
            err("Unknown variable type, at line %d", tokens[iTk].line);
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
        } else if (tokens[iTk].code == ID) {
            err("Ambiguous listing of identifiers, at line %d", tokens[iTk].line);
        }
        return true;
    }

    iTk = start;
    return false;
}

// is this where I should handle signed numbers?
bool expr_prefix() {
    // maybe here it would pass the code of the operator which prefixes the potential literal val
    // and then factor() would handle its signedness
    if (consume(SUB) || consume(NOT)) { }
    // wtf, this allows case SUB LITERAL_STR or NOT LITERAL_STR
    if (!factor()) {
        return false;
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
        return true;
    }
    return false;
}

bool expr_add() {
    if (expr_mul()) {
        while (consume(ADD) || consume(SUB)) {
            if (!expr_mul()) {
                err("Missing expression after operand '+' or '-', at line %d", tokens[iTk].line);
            }
        }
        return true;
    }
    return false;
}

bool expr_comp() {
    if (expr_add()) {
        while (consume(EQUAL) || consume(NOT_EQ) || consume(LESS) || consume(GREATER) || consume(GREATER_EQ)) {
            if (!expr_add()) {
                err("Missing expression after comparation operand, at line %d", tokens[iTk].line);
            }
        }
        return true;
    }
    return false;
}

bool expr_assign() {
    int start = iTk;
    if (consume(ID)) {
        if (!consume(ASSIGN)) {
            iTk = start;
        }
    }
    if (expr_comp()) {
        return true;
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
        return true;
    }
    return false;
}

bool expr() {
    return expr_logic();
}

bool block();

bool instr() {
    int start = iTk;
    // I think I would get an error if the *optional* expression is missing
    if (expr()) {
        // do I need to move it out of the nest?
        if (!consume(SEMICOLON)) {
            // this would be bad for char arrays
            // but this will depend on the way of implementing these
            if (tokens[iTk].code == ASSIGN) {
                err("Invalid assignment logic, at line %d", tokens[iTk].line);
            }
            err("Missing semicolon after expression, at line %d", tokens[iTk].line);
        }
        return true;
    }

    if (consume(IF)) {
        if (!consume(LPAR)) {
            err("If statement start with '(', at line %d", tokens[iTk].line);
        }
        if (!expr()) {
            err("If statement is not provided an expression, at line %d", tokens[iTk].line);
        }
        if (!consume(RPAR)) {
            err("If statement must end with ')', at line %d", tokens[iTk].line);
        }

        block();

        if (consume(ELSE)) {
            if (!block()) {
                err("Else statement missing block, at line %d", tokens[iTk].line);
            }
        }
        if (consume(END)) {
            return true;
        } else {
            // unsafe?
            err("If statement missing 'end' scope terminator, at line %d", tokens[iTk].line);
        }
    }

    if (consume(RETURN)) {
        expr();
        if (!consume(SEMICOLON)) {
            err("Return statement missing ';', at line %d", tokens[iTk].line);
        }
        return true;
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
            // maybe at some point, I want to allow this
            err("\"while\" statement body is empty, at line %d", tokens[iTk].line);
        }
        if (!consume(END)) {
            return true;
        } else {
            // unsafe?
            err("While loop missing 'end' scope terminator, at line %d", tokens[iTk].line);
        }
    }

    iTk = start;
    return false;
}

bool block() {
    bool has_instr = false;
    while (instr()) {
        has_instr = true;
    }
    return has_instr;
}

bool func_param() {
    if (consume(ID) && consume(COLON) && baseType()) {
        return true;
    }
    err("Missing variable type at declaration, at line %d", tokens[iTk].line);
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

// should I allow function prototypes?
bool def_func() {
    int start = iTk;
    if (consume(FUNCTION)) {
        if (!consume(ID)) {
            err("Function missing identifier, at line %d", tokens[iTk].line);
        }
        if (!consume(LPAR)) {
            err("Function declaration missing '(', at line %d", tokens[iTk].line);
        }
        if (tokens[iTk].code != RPAR) {
            func_params();
        }

        if (!consume(RPAR) && consume(COLON) && baseType()) {
            err("Function declaration missing ')', at line %d", tokens[iTk].line);    
        }
        if (!consume(COLON)) {
            err("Function return type must be preceded by ':', at line %d", tokens[iTk].line);
        }
        if (!baseType()) {
            err("Unknown function return type, at line %d", tokens[iTk].line);
        }
        
        while(def_var()) {}
        block();
        
        if (consume(END)) {
            return true;
        } else {
            // not sure it's the correct case to throw error here.. unsafe?
            // also, the line is not accurate..
            err("Function missing 'end' scope terminator, at line %d", tokens[iTk].line);
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
