//gcc main.c lexer.c -o run to compile

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main(){
    FILE *file;
    file = fopen("test.gr", "r");

    if(!file){
        printf("No file found\n");
        exit(1);
    }
    Token *tokens = lexer(file);

    // Print all the tokens
    for (size_t i = 0; tokens[i].type != END_OF_TOKENS; i++){
        print_token(tokens[i]);
    }

    return 0;
}
