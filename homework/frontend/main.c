#include "lexer.h"
#include "utils.h"

int main() {
    printf("test\n");

    char* buffer = loadFile("../lexer/test/sample_1.txt");
    tokenize(buffer);
    showTokens();

    free(buffer);
    return 0;
}