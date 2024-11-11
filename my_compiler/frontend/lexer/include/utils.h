#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include "lexer.h"

const char* convertTokenCode(Token* tk, int mode);

void printTokens();

void printPrettyTokens();

void printDebugTokens();

#endif
