#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

bool consume(int code);

bool baseType();

bool var_def();

bool factor();

bool expr_prefix();

bool expr_mul();

bool expr_add();

bool expr_bitwise();

bool expr_comp();

bool expr_assign();

bool expr_logic();

bool expr();

bool instr();

bool block();

bool func_definition();

bool func_arg();

bool func_args();

bool func_definition();

bool func_prototype();

bool instr_body();

bool func_declaration();

bool program();

void parse();

#endif