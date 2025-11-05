//gcc -o lex .\lex_main.c .\lexer.c

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

static void free_tokens(Token *tokens){
  for (size_t i = 0; tokens[i].type != END_OF_TOKENS; ++i){
    if (tokens[i].type != KEYWORD && tokens[i].value){
      free(tokens[i].value);
    }
  }
  free(tokens);
}

int main(int argc, char **argv){
  const char *path = (argc > 1) ? argv[1] : "sample.goish";
  FILE *f = fopen(path, "rb");
  if (!f){
    fprintf(stderr, "Could not open '%s'\n", path);
    return 1;
  }
  Token *tokens = lexer(f);
  if (!tokens){
    fprintf(stderr, "Lexing failed.\n");
    fclose(f);
    return 2;
  }
  for (size_t i = 0; tokens[i].type != END_OF_TOKENS; ++i){
    print_token(tokens[i]);
  }
  free_tokens(tokens);
  fclose(f);
  return 0;
}
