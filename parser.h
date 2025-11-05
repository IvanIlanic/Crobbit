#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"

typedef struct Node {
  char *value;
  TokenType type;
  struct Node *right;
  struct Node *left;
} Node;

Node *parser(Token *tokens);
void print_tree(Node *node, int indent, char *identifier);

#endif // PARSER_H_
