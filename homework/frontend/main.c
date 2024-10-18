#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

int main() {
    char* buffer = loadFile("../lexer/test/sample_1.txt");
    tokenize(buffer);
    showPrettyTokens();

    free(buffer);
    return 0;
}