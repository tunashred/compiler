#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int    iTk;
Token* consumed;

bool consume(int code) {
    if (tokens[iTk].code == code) {
        consumed = &tokens[iTk++];
        return true;
    }
    return false;
}

// // program ::= ( defVar | defFunc | block )* FINISH
// bool program() {
//     for (;;) {
//         if (defVar()) {
//         } else if (defFunc()) {
//         } else if (block()) {
//         } else
//             break;
//     }
//     if (consume(FINISH)) {
//         return true;
//     } else
//         err("syntax error");
//     return false;
// }

// void parse() {
//     iTk = 0;
//     program();
// }
