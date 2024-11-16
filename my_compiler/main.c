#include <stdlib.h>
#include <stdio.h>

#include "frontend/lexer/include/lexer.h"
#include "frontend/lexer/include/utils.h"
#include "frontend/utils/include/utils.h"

int main() {
    size_t buffer_size;
    char* buffer = loadFile("../frontend/test/sample_1.txt", &buffer_size);
    tokenize(buffer, buffer_size);
    printTokens();
    printf("\n\n");
    printPrettyTokens();
    printf("\n\n");
    printDebugTokens();

    free(buffer);
    return 0;
}