#include <stdlib.h>

#include "frontend/lexer/include/lexer.h"
// #include "parser.h"
#include "frontend/lexer/include/utils.h"
#include "frontend/utils/include/utils.h"

int main() {
    char* buffer = loadFile("../frontend/test/sample_1.txt");
    tokenize(buffer);
    printDebugTokens();

    free(buffer);
    return 0;
}