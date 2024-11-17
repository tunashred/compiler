#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "frontend/lexer/include/lexer.h"
#include "frontend/lexer/include/utils.h"
#include "frontend/utils/include/utils.h"

/*  Ideas for improving error reporting
    1. The parser shouldn't stop at the first error occurence
    2. Instead of "syntax error" print at end of if-elseif, I could bring some improvements:
        - check the current token (maybe it's neighbors too) and try to squeeze more information
*/

int iTk;
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
        case TYPE_FLOAT:
        case TYPE_CHARRAY:
            consume(tokens[iTk].code);
            return true;

        default:
            return false;
    }
}

bool expr();

bool var_def() {
    int start = iTk;
    if (baseType()) {
        if (!consume(ID)) {
            err("Variable declaration missing identifier, at line %d", tokens[iTk].line);
        }

        if (consume(ASSIGN)) {
            if (!expr()) {
                err("Expected expression at assignment, at line %d", tokens[iTk].line);
            }
        }

        while(consume(COMMA)) {
            if (!consume(ID)) {
                err("Expected identified at variable declaration, at line %d", tokens[iTk].line);
            }
            if (consume(ASSIGN)) {
                if (!expr()) {
                    err("Expected expression at assignment, at line %d", tokens[iTk].line);
                }
            }
        }
        if (!consume(SEMICOLON)) {
            err("Expected ';' at end of variable declaration, at line %d", tokens[iTk].line);
        }
        return true;
    }

    iTk = start;
    return false;
}

bool factor() {
    int start = iTk;
    if (consume(LITERAL_INT) || consume(LITERAL_FLOAT) || consume(LITERAL_CHAR) || consume(LITERAL_CHARRAY)) {
        return true;
    // TODO: check if this could be decollapsed for more error printing
    } else if (consume(L_ROUND_PAR) && expr() && consume(R_ROUND_PAR)) {
        return true;
    } else if (consume(ID)) {
        if (consume(L_ROUND_PAR)) {
            if (expr()) {
                while (consume(COMMA)) {
                    if (!expr()) {
                        err("Invalid expression inside function call, at line %d", tokens[iTk].line);
                    }
                }
            }
            if (!consume(R_ROUND_PAR)) {
                err("Expression missing ')', at line %d", tokens[iTk].line);
            }
        // TODO: check if this should be moved to a better place
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
    if (consume(ADD) || consume(SUB) || consume(LOGICAL_NOT) || consume(BITWISE_NOT)) {}
    // wtf, this allows case SUB LITERAL_CHARRAY or NOT LITERAL_CHARRAY
    if (!factor()) {
        return false;
    }
    return true;
}

bool expr_mul() {
    if (expr_prefix()) {
        while (consume(MUL) || consume(DIV)) {
            int last_code = tokens[iTk - 1].code;
            if (!expr_prefix()) {
                if (last_code == MUL) {
                    err("Missing expression after operand '*', at line %d", tokens[iTk].line);
                }
                err("Missing expression after operand '/', at line %d", tokens[iTk].line);
            }
        }
        return true;
    }
    return false;
}

bool expr_add() {
    if (expr_mul()) {
        while (consume(ADD) || consume(SUB)) {
            int last_code = tokens[iTk - 1].code;
            if (!expr_mul()) {
                if (last_code == ADD) {
                    err("Missing expression after operand '+', at line %d", tokens[iTk].line);
                }
                err("Missing expression after operand '-', at line %d", tokens[iTk].line);
            }
        }
        return true;
    }
    return false;
}

bool expr_bitwise() {
    if (expr_add()) {
        while (consume(BITWISE_AND) || consume(BITWISE_XOR) || consume(BITWISE_OR)) {
            int last_code = tokens[iTk - 1].code;
            if (!expr_add()) {
                if (last_code == BITWISE_AND) {
                    err("Missing expression after operand '&', at line %d", tokens[iTk].line);
                } else if (last_code == BITWISE_XOR) {
                    err("Missing expression after operand '^', at line %d", tokens[iTk].line);
                } else {
                    err("Missing expression after operand '|', at line %d", tokens[iTk].line);
                }
            }
        }
        return true;
    }
    return false;
}

bool expr_comp() {
    if (expr_bitwise()) {
        // TODO: need to revise operator precedence table
        // TODOx2: add more informative errors here
        while (consume(EQUAL) || consume(NOT_EQ) || consume(LESS) || consume(GREATER) || consume(GREATER_EQ)) {
            if (!expr_bitwise()) {
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
        while (consume(LOGICAL_AND) || consume(LOGICAL_OR)) {
            // TODO: more informative error message like above
            if (!expr_assign()) {
                err("Malformed expression logic, at line %d", tokens[iTk].line);
            }
        }
        return true;
    }
    return false;
}

bool expr() { return expr_logic(); }

bool block();

bool instr_body() {
    int start = iTk;
    if (consume(L_BRACKET)) {
        if (!block()) {
            // do i need this?
            err("Expected block of code, at line %d", tokens[iTk].line);
        }
        if (!consume(R_BRACKET)) {
            err("Expected '}' after block, at line %d", tokens[iTk].line);
        }
        return true;
    }
    iTk = start;
    return false;
}

bool instr() {
    int start = iTk;
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

    if (consume(WHILE)) {
        if (consume(L_ROUND_PAR)) {
            baseType();
            if (!consume(ID)) {
                err("Expected variable name at while initialization, at line %d", tokens[iTk].line);
            }
            if (!consume(ASSIGN)) {
                err("Expected assigment operator '=', at line %d", tokens[iTk].line);
            }
            if (!expr()) {
                err("Invalid expression used at assigment, at line %d", tokens[iTk].line);
            }
            if (!consume(R_ROUND_PAR)) {
                err("Missing ')' after variable initialization, at line %d", tokens[iTk].line);
            }
        }
        if (!expr()) {
            err("Missing expression inside \"while\" statement, at line %d", tokens[iTk].line);
        }
        if (!instr_body()) {
            // maybe at some point, I want to allow this
            err("\"while\" statement body is empty, at line %d", tokens[iTk].line);
        }
        return true;
    }

    if (consume(IF)) {
        if (!consume(L_ROUND_PAR)) {
            err("If statement start with '(', at line %d", tokens[iTk].line);
        }
        if (!expr()) {
            err("If statement is not provided an expression, at line %d", tokens[iTk].line);
        }
        if (!consume(R_ROUND_PAR)) {
            err("If statement must end with ')', at line %d", tokens[iTk].line);
        }

        instr_body();

        if (consume(ELIF)) {
            if (!consume(L_ROUND_PAR)) {
                err("Elif statement start with '(', at line %d", tokens[iTk].line);
            }
            if (!expr()) {
                err("Elif statement is not provided an expression, at line %d", tokens[iTk].line);
            }
            if (!consume(R_ROUND_PAR)) {
                err("Elif statement must end with ')', at line %d", tokens[iTk].line);
            }
            instr_body();
        }

        if (consume(ELSE)) {
            instr_body();
        }

        return true;
    }

    if (consume(RETURN)) {
        expr();
        if (!consume(SEMICOLON)) {
            err("Return statement missing ';', at line %d", tokens[iTk].line);
        }
        return true;
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

bool func_arg() {
    if (baseType() && consume(ID)) {
        return true;
    }
    err("Missing variable type at declaration, at line %d", tokens[iTk].line);
    return false;
}

bool func_args() {
    if (func_arg()) {
        while (consume(COMMA)) {
            if (!func_arg()) {
                err("Invalid parameter definition, at line %d", tokens[iTk].line);
            }
        }
    }
    return true;
}

// should I allow function prototypes?
bool func_definition() {
    int start = iTk;
    if (consume(FUN)) {
        if (!consume(ID)) {
            err("Function missing identifier, at line %d", tokens[iTk].line);
        }
        if (!consume(L_ROUND_PAR)) {
            err("Function declaration missing '(', at line %d", tokens[iTk].line);
        }
        if (tokens[iTk].code != R_ROUND_PAR) {
            func_args();
        }

        if (!consume(R_ROUND_PAR)) {
            err("Function declaration missing ')', at line %d", tokens[iTk].line);
        }
        if (!consume(FUNC_RET_OP)) {
            err("Function return type must be preceded by '->', at line %d", tokens[iTk].line);
        }
        if (!baseType() && !consume(TYPE_VOID)) {
            err("Unknown function return type, at line %d", tokens[iTk].line);
        }
        return true;
    }

    iTk = start;
    return false;
}

bool func_prototype() {
    int start = iTk;
    if (func_definition()) {
        if (consume(SEMICOLON)) {
            return true;
        }
        iTk = start;
    }
    return false;
}

bool func_declaration() {
    if (func_definition()) {
        if (instr_body()) {
            return true;
        }
    }
    return false;
}

bool program() {
    // func_definition();
    for (;;) {
        if (var_def()) {
        } else if (func_prototype()) {
        } else if (func_declaration()) {
        } else if (block()) {
        } else
            break;
    }
    if (consume(FINISH)) {
        return true;
    } else
        err("Syntax error at line, %d", tokens[iTk].line);
    return false;
}

void parse() {
    iTk = 0;
    program();
}
