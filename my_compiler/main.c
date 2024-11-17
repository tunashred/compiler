#include <stdlib.h>
#include <stdio.h>

#include "frontend/lexer/include/lexer.h"
#include "frontend/parser/include/parser.h"
#include "frontend/lexer/include/utils.h"
#include "frontend/utils/include/utils.h"

int main() {
    size_t buffer_size;
    char* buffer = loadFile("../frontend/test/sample_2.txt", &buffer_size);
    tokenize(buffer, buffer_size);
    printTokens();
    printf("\n\n");
    printPrettyTokens();
    printf("\n\n");
    printDebugTokens();

    parse();

    free(buffer);
    return 0;
}
