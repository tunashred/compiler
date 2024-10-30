#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "lexer.h"

bool consume(int code);

bool baseType(Token* tk);

bool def_var(Token *tk);

bool func_param(Token* tk);

bool func_params(Token* tk);

bool program();

// parse the extracted tokens
void parse();

#endif
