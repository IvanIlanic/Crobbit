//# From the folder with your files
//gcc -o ast .\main.c .\lexer.c .\parser.c

//# Run (uses sample.goish by default)
//.\ast

//# Or specify a file
//.\ast .\sample.goish

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

static void free_tree(Node *n){
  if(!n) return;
  free_tree(n->left);
  free_tree(n->right);
  if (n->value) free(n->value);
  free(n);
}

static void free_tokens(Token *tokens){
  for (size_t i = 0; tokens[i].type != END_OF_TOKENS; ++i){
    if (tokens[i].type != KEYWORD && tokens[i].value){
      free(tokens[i].value);
    }
  }
  free(tokens);
}

int main(int argc, char **argv){
  const char *path = argc > 1 ? argv[1] : "sample.goish";
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
  Node *root = parser(tokens);
  puts("=== AST ===");
  print_tree(root, 0, "root");
  free_tree(root);
  free_tokens(tokens);
  fclose(f);
  return 0;
}
