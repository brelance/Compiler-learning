#include "compiler.h"
#include "scanner.h"
#include <stdio.h>

void compile(const char* source) {
    int line = -1;
    initScanner(source);
    for(;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d", token.line);
            line = token.line;
        } else {
            printf("    | ");
        }
        printf("%2d '%.*s'\n", token.type,
            token.length, token.start); 
        
        if (token.type == TOKEN_EOF) break;
    }

}