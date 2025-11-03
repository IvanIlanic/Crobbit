#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

typedef struct Node {
  char *value;
  TokenType type;
  struct Node *right;
  struct Node *left;
} Node;