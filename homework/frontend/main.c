#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main() {
    char* buffer = loadFile("../lexer/test/sample_1.txt");
    tokenize(buffer);
    printTokens();
    parse();

    free(buffer);
    return 0;
}
