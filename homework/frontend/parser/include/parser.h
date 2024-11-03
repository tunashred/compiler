#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "lexer.h"

bool consume(int code);

bool baseType();

bool def_var();

bool factor();

bool expr_prefix();

bool expr_mul();

bool expr_add();

bool expr_comp();

bool expr_assign();

bool expr_logic();

bool expr();

bool instr();

bool block();

bool def_func();

bool func_param();

bool func_params();

bool program();

// parse the extracted tokens
void parse();

#endif
