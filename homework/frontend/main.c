#include "lexer.h"
#include "utils.h"

int main() {
    char* buffer = loadFile("../lexer/test/sample_2.txt");
    tokenize(buffer);
    showTokens();

    free(buffer);
    return 0;
}