#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main() {
    char* buffer = loadFile("../lexer/test/sample_2.txt");
    tokenize(buffer);
    printDebugTokens();
    parse();

    free(buffer);
    return 0;
}
