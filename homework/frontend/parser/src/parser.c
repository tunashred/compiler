#include "parser.h"

#include <stdarg.h>
#include <stdbool.h>

#include "lexer.h"
#include "semantic_analyzer/include/domain.h"
#include "types_analyzer/include/types.h"
#include "utils.h"

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
            ret.type = TYPE_INT;
            consume(tokens[iTk].code);
            return true;
        case TYPE_REAL:
            ret.type = TYPE_REAL;
            consume(tokens[iTk].code);
            return true;
        case TYPE_STR:
            ret.type = TYPE_STR;
            consume(tokens[iTk].code);
            return true;

        default:
            err("Invalid type, at line %d", tokens[iTk].line);
    }
}

bool def_var() {
    int start = iTk;
    if (consume(VAR)) {
        if (!consume(ID)) {
            err("Variable declaration missing identifier, at line %d", tokens[iTk].line);
        }
        const char* name = consumed->text;
        Symbol* symbol = searchInCurrentDomain(name);
        if (symbol) {
            err("Symbol redefinition: %s, at line %d", name, tokens[iTk].line);
        }
        symbol = addSymbol(name, KIND_VAR);
        symbol->local = crtFn != NULL;
        if (!consume(COLON)) {
            err("Variable declaration must contain ':' after identifier, at line %d", tokens[iTk].line);
        }
        if (baseType()) {
            symbol->type = ret.type;
            if (consume(SEMICOLON)) {
                return true;
            } else if (tokens[iTk].code == ASSIGN) {
                err("Variable initialization is forbidden, at line %d", tokens[iTk].line);
            }
            err("Missing semicolon, at line %d", tokens[iTk].line);
        }
        err("Unknown variable type, at line %d", tokens[iTk].line);
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
    if (consume(SUB) || consume(NOT)) {}
    // wtf, this allows case SUB LITERAL_STR or NOT LITERAL_STR
    if (!factor()) {
        return false;
    }
    return true;
}

bool expr_mul() {
    if (expr_prefix()) {
        while (consume(MUL) || consume(DIV)) {
            Ret leftType = ret;
            if (leftType.type == TYPE_STR) {
                err("Operands of '*' or '/' expression cannot be string type, at line %d", tokens[iTk].line);
            } 
            int last_code = tokens[iTk - 1].code;
            if (!expr_prefix()) {
                if (last_code == MUL) {
                    err("Missing expression after operand '*', at line %d", tokens[iTk].line);
                }
                err("Missing expression after operand '/', at line %d", tokens[iTk].line);
            }
            if (leftType.type != ret.type) {
                err("Different types between operands of '*' or '/' expression, at line %d", tokens[iTk].line);
            }
            ret.lval = false;
        }
        return true;
    }
    return false;
}

bool expr_add() {
    if (expr_mul()) {
        while (consume(ADD) || consume(SUB)) {
            int last_code = tokens[iTk - 1].code;
            Ret leftType = ret;
            if (leftType.type == TYPE_STR) {
                err("Operands of '+' or '-' expression cannot be string type, at line %d", tokens[iTk].line);
            }
            if (!expr_mul()) {
                if (last_code == ADD) {
                    err("Missing expression after operand '+', at line %d", tokens[iTk].line);
                }
                err("Missing expression after operand '-', at line %d", tokens[iTk].line);
            }
            if (leftType.type != ret.type) {
                err("Different types between operands of expression '+' or '-', at line %d", tokens[iTk].line);
            }
            ret.lval = false;
        }
        return true;
    }
    return false;
}

bool expr_comp() {
    if (expr_add()) {
        // TODO: need to revise operator precedence table
        while (consume(EQUAL) || consume(NOT_EQ) || consume(LESS) || consume(GREATER) || consume(GREATER_EQ)) {
            Ret leftType = ret;
            if (!expr_add()) {
                err("Missing expression after comparation operand, at line %d", tokens[iTk].line);
            }
            if (leftType.type != ret.type) {
                err("Different types between operands at comparation expression, at line %d", tokens[iTk].line);
            }
            setRet(TYPE_INT, false);
        }
        return true;
    }
    return false;
}

bool expr_assign() {
    int start = iTk;
    if (consume(ID)) {
        const char* name = consumed->text;
        if (consume(ASSIGN)) {
            if (!expr_comp()) {
                err("Expected expression at assigment, at line %d", tokens[iTk].line);
            }
            Symbol* symbol = searchSymbol(name);
            if (!symbol) {
                err("Undefined variable: %s, at line %d", name, tokens[iTk].line);
            }
            if (symbol->kind == KIND_FN) {
                err("Function %s cannot be used as destination for assignment, at line %d", name, tokens[iTk].line);
            }
            if (symbol->type != ret.type) {
                err("Source and destination must be same type at assignment expression, at line %d", tokens[iTk].line);
            }
            ret.lval = false;
            return true;
        }
    }
    iTk = start;
    
    return expr_comp();
}

bool expr_logic() {
    if (expr_assign()) {
        while (consume(AND) || consume(OR)) {
            Ret leftType = ret;
            if (leftType.type == TYPE_STR) {
                err("The left operand of logic expression cannot be string, at line %d", tokens[iTk].line);
            }
            if (!expr_assign()) {
                err("Malformed expression logic, at line %d", tokens[iTk].line);
            }
            if (ret.type == TYPE_STR) {
                err("The right operand of logic expression cannot be string, at line %d", tokens[iTk].line);
            }
            setRet(TYPE_INT, false);
        }
        return true;
    }
    return false;
}

bool expr() { return expr_logic(); }

bool block();

bool instr() {
    int start = iTk;
    if (expr()) {
        // do I need to move it out of the nest?
        if (!consume(SEMICOLON)) {
            err("Missing semicolon after expression, at line %d", tokens[iTk].line);
        }
        return true;
    }
    consume(SEMICOLON);

    if (consume(IF)) {
        if (!consume(LPAR)) {
            err("If statement start with '(', at line %d", tokens[iTk].line);
        }
        if (!expr()) {
            err("If statement is not provided an expression, at line %d", tokens[iTk].line);
        }
        if (ret.type == TYPE_STR) {
            err("The if condition must have int or float, at line %d", tokens[iTk].line);
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
        if (!consume(END)) {
            // unsafe?
            err("If statement missing 'end' scope terminator, at line %d", tokens[iTk].line);
        }
        return true;
    }

    if (consume(RETURN)) {
        if (!expr()) {
            err("Expected expression at return, at line %d", tokens[iTk].line);
        }
        if (!crtFn) {
            err("Return can be used only inside a function, at line %d", tokens[iTk].line);
        }
        if (ret.type != crtFn->type) {
            err("Return type does not match function type, at line %d", tokens[iTk].line);
        }
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
        if (ret.type == TYPE_STR) {
            err("The while condition must verify int or real values, at line %d", tokens[iTk].line);
        }
        if (!consume(RPAR)) {
            err("Missing right paranthesis at \"while\" statement, at line %d", tokens[iTk].line);
        }
        if (!block()) {
            // maybe at some point, I want to allow this
            err("\"while\" statement body is empty, at line %d", tokens[iTk].line);
        }
        if (!consume(END)) {
            // unsafe?
            err("While loop missing 'end' scope terminator, at line %d", tokens[iTk].line);
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

bool func_param() {
    if (consume(ID)) {
        const char* name = consumed->text;
        Symbol* symbol = searchInCurrentDomain(name);
        if (symbol) {
            err("Function argument redefinition: %s, at line %d", name, tokens[iTk].line);
        }
        symbol = addSymbol(name, KIND_ARG);
        Symbol* s_func_param = addFnArg(crtFn, name);
        if (consume(COLON) && baseType()) {
            symbol->type = ret.type;
            s_func_param->type = ret.type;
            return true;
        }
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
        const char* name = consumed->text;
        Symbol* symbol = searchInCurrentDomain(name);
        if (symbol) {
            err("Function redefinition: %s, at line %d", name, tokens[iTk].line);
        }
        crtFn = addSymbol(name, KIND_FN);
        crtFn->args = NULL;
        addDomain();
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

        crtFn->type = ret.type;

        while (def_var()) {}
        block();

        delDomain();
        crtFn = NULL;

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

bool program() {
    addDomain();
    addPredefinedFns();
    def_func();
    for (;;) {
        if (def_var()) {
        } else if (def_func()) {
        } else if (block()) {
        } else
            break;
    }
    delDomain();
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
