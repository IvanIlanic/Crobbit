#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stddef.h>

typedef enum {
  BEGINNING,
  INT,
  KEYWORD,
  SEPARATOR,
  OPERATOR,
  IDENTIFIER,
  STRING,
  COMP,
  END_OF_TOKENS,
} TokenType;

typedef struct {
  TokenType type;
  char *value;
  size_t line_num;
} Token;

extern size_t line_number;

void print_token(Token token);

/* Returns a heap-allocated array of Tokens terminated by END_OF_TOKENS. */
Token *lexer(FILE *file);

#endif /* LEXER_H */
