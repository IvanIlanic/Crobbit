#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef LEXER_H_
#define LEXER_H_

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

void print_token(Token token);
Token *generate_keyword_or_identifier(char *current, int *current_index);
Token *generate_number(char *current, int *current_index);
Token *generate_separator_or_operator(char *current, int *current_index, TokenType type);
Token *generate_string_token(char *current, int *current_index);
Token *lexer(FILE *file);

#endif // LEXER_H_
