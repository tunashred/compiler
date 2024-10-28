#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

bool consume(int code);

bool program();

// parse the extracted tokens
void parse();

#endif
