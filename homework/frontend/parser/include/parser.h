#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "lexer.h"

bool consume(int code);

bool baseType();

bool def_var();

bool func_param();

bool func_params();

bool block();

bool def_func();

bool instr();

bool expr();

bool expr_logic();

bool expr_assign();

bool expr_comp();

bool expr_add();

bool expr_mul();

bool expr_prefix();

bool factor();

bool program();

// parse the extracted tokens
void parse();

#endif
