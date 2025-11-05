#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

size_t line_number = 1;

static char *xstrdup(const char *s){
  if(!s) return NULL;
  size_t n = strlen(s) + 1;
  char *p = (char*)malloc(n);
  if (p) memcpy(p, s, n);
  return p;
}

void print_token(Token token){
  printf("TOKEN VALUE: '");
  if (token.value){
    for(int i = 0; token.value[i] != '\0'; i++){
      printf("%c", token.value[i]);
    }
  }
  printf("'\nline number: %zu", token.line_num);

  switch(token.type){
    case INT:        printf(" TOKEN TYPE: INT\n"); break;
    case KEYWORD:    printf(" TOKEN TYPE: KEYWORD\n"); break;
    case SEPARATOR:  printf(" TOKEN TYPE: SEPARATOR\n"); break;
    case OPERATOR:   printf(" TOKEN TYPE: OPERATOR\n"); break;
    case IDENTIFIER: printf(" TOKEN TYPE: IDENTIFIER\n"); break;
    case STRING:     printf(" TOKEN TYPE: STRING\n"); break;
    case COMP:       printf(" TOKEN TYPE: COMPARATOR\n"); break;
    case END_OF_TOKENS: printf(" END OF TOKENS\n"); break;
    case BEGINNING:  printf("BEGINNING\n"); break;
  }
}

static void skip_line_comment(const char *s, int *i){
  (*i) += 2;
  while (s[*i] != '\0' && s[*i] != '\n') (*i)++;
}

static void skip_block_comment(const char *s, int *i){
  (*i) += 2;
  while (s[*i] != '\0'){
    if (s[*i] == '\n') line_number++;
    if (s[*i] == '*' && s[*i+1] == '/'){ (*i) += 2; return; }
    (*i)++;
  }
}

static Token *generate_number(char *current, int *current_index){
  Token *token = (Token*)malloc(sizeof(Token));
  token->line_num = line_number;
  token->type = INT;

  char *value = (char*)malloc(sizeof(char) * 64);
  int value_index = 0;

  while(isdigit((unsigned char)current[*current_index])){
    value[value_index++] = current[*current_index];
    (*current_index)++;
    if (value_index >= 63) break;
  }
  value[value_index] = '\0';
  token->value = value;
  return token;
}

static Token *generate_keyword_or_identifier(char *current, int *current_index){
  Token *token = (Token*)malloc(sizeof(Token));
  token->line_num = line_number;

  char *keyword = (char*)malloc(sizeof(char) * 64);
  int keyword_index = 0;

  if (isalpha((unsigned char)current[*current_index]) || current[*current_index] == '_'){
    keyword[keyword_index++] = current[*current_index];
    (*current_index)++;
  }

  while(current[*current_index] != '\0' &&
        (isalnum((unsigned char)current[*current_index]) || current[*current_index] == '_')){
    if (keyword_index >= 63) break;
    keyword[keyword_index++] = current[*current_index];
    (*current_index)++;
  }
  keyword[keyword_index] = '\0';

  /* minimal keyword set used by your program */
  if(strcmp(keyword, "int") == 0){
    token->type = KEYWORD; token->value = "INT";
  }
  else if(strcmp(keyword, "float64") == 0){
    token->type = KEYWORD; token->value = "FLOAT64";
  }
  else if(strcmp(keyword, "if") == 0){
    token->type = KEYWORD; token->value = "IF";
  }
  else if(strcmp(keyword, "else") == 0){
    token->type = KEYWORD; token->value = "ELSE";
  }
  else if(strcmp(keyword, "for") == 0){
    token->type = KEYWORD; token->value = "FOR";
  }
  else if(strcmp(keyword, "return") == 0){
    token->type = KEYWORD; token->value = "RETURN";
  }
  else if(strcmp(keyword, "package") == 0){
    token->type = KEYWORD; token->value = "PACKAGE";
  }
  else if(strcmp(keyword, "const") == 0){
    token->type = KEYWORD; token->value = "CONST";
  }
  else if(strcmp(keyword, "var") == 0){
    token->type = KEYWORD; token->value = "VAR";
  }
  else if(strcmp(keyword, "func") == 0){
    token->type = KEYWORD; token->value = "FUNC";
  }
  else {
    token->type = IDENTIFIER;
    token->value = keyword;
    return token;
  }

  free(keyword);
  return token;
}

static Token *generate_string_token(char *current, int *current_index){
  Token *token = (Token*)malloc(sizeof(Token));
  token->line_num = line_number;

  char *value = (char*)malloc(sizeof(char) * 256);
  int value_index = 0;

  (*current_index)++; // skip opening "
  while(current[*current_index] != '\"' && current[*current_index] != '\0'){
    if (current[*current_index] == '\n') line_number++;
    value[value_index++] = current[*current_index];
    (*current_index)++;
    if (value_index >= 255) break;
  }
  value[value_index] = '\0';
  token->type = STRING;
  token->value = value;
  return token;
}

static Token *generate_sep_or_op(char *current, int *current_index, TokenType type){
  Token *token = (Token*)malloc(sizeof(Token));
  token->value = (char*)malloc(sizeof(char) * 3);
  token->value[0] = current[*current_index];
  token->value[1] = '\0';
  token->line_num = line_number;
  token->type = type;
  return token;
}

Token *lexer(FILE *file){
  long length;
  char *current = 0;

  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (length < 0) { fclose(file); return NULL; }

  current = (char*)malloc((size_t)length + 1);
  size_t nread = fread(current, 1, (size_t)length, file);
  current[nread] = '\0';

  fclose(file);

  int current_index = 0;

  int number_of_tokens = 128;
  int tokens_size = 0;
  Token *tokens = (Token*)malloc(sizeof(Token) * number_of_tokens);
  int tokens_index = 0;

  while(current[current_index] != '\0'){
    /* Skip whitespace */
    if (isspace((unsigned char)current[current_index])){
      if (current[current_index] == '\n') line_number += 1;
      current_index++;
      continue;
    }

    /* Skip comments */
    if (current[current_index] == '/' && current[current_index+1] == '/'){
      skip_line_comment(current, &current_index);
      continue;
    }
    if (current[current_index] == '/' && current[current_index+1] == '*'){
      skip_block_comment(current, &current_index);
      continue;
    }

    /* Ensure capacity */
    if(tokens_size + 2 > number_of_tokens){
      number_of_tokens = number_of_tokens * 2;
      tokens = (Token*)realloc(tokens, sizeof(Token) * number_of_tokens);
    }

    /* Separators */
    if(current[current_index] == ';' || current[current_index] == ',' ||
       current[current_index] == '(' || current[current_index] == ')' ||
       current[current_index] == '{' || current[current_index] == '}'){
      Token *t = generate_sep_or_op(current, &current_index, SEPARATOR);
      tokens[tokens_index++] = *t; free(t);
      tokens_size++;
      current_index++;
      continue;
    }

    /* Comparators: ==, !=, <=, >=, <, > */
    if (current[current_index] == '=' && current[current_index+1] == '='){
      Token t = { COMP, xstrdup("=="), line_number };
      tokens[tokens_index++] = t; tokens_size++; current_index += 2; continue;
    }
    if (current[current_index] == '!' && current[current_index+1] == '='){
      Token t = { COMP, xstrdup("!="), line_number };
      tokens[tokens_index++] = t; tokens_size++; current_index += 2; continue;
    }
    if (current[current_index] == '<'){
      if (current[current_index+1] == '='){
        Token t = { COMP, xstrdup("<="), line_number };
        tokens[tokens_index++] = t; tokens_size++; current_index += 2;
      } else {
        Token t = { COMP, xstrdup("<"), line_number };
        tokens[tokens_index++] = t; tokens_size++; current_index += 1;
      }
      continue;
    }
    if (current[current_index] == '>'){
      if (current[current_index+1] == '='){
        Token t = { COMP, xstrdup(">="), line_number };
        tokens[tokens_index++] = t; tokens_size++; current_index += 2;
      } else {
        Token t = { COMP, xstrdup(">"), line_number };
        tokens[tokens_index++] = t; tokens_size++; current_index += 1;
      }
      continue;
    }

    /* Operators: = + - * / % */
    if(current[current_index] == '=' || current[current_index] == '+' ||
       current[current_index] == '-' || current[current_index] == '*' ||
       current[current_index] == '/' || current[current_index] == '%'){
      Token *t = generate_sep_or_op(current, &current_index, OPERATOR);
      tokens[tokens_index++] = *t; free(t);
      tokens_size++;
      current_index++;
      continue;
    }

    /* Strings */
    if(current[current_index] == '\"'){
      Token *t = generate_string_token(current, &current_index);
      tokens[tokens_index++] = *t; free(t);
      tokens_size++;
      if (current[current_index] == '\"') current_index++;
      continue;
    }

    /* Numbers */
    if(isdigit((unsigned char)current[current_index])){
      Token *t = generate_number(current, &current_index);
      tokens[tokens_index++] = *t; free(t);
      tokens_size++;
      continue;
    }

    /* Keywords / Identifiers */
    if(isalpha((unsigned char)current[current_index]) || current[current_index] == '_'){
      Token *t = generate_keyword_or_identifier(current, &current_index);
      tokens[tokens_index++] = *t; free(t);
      tokens_size++;
      continue;
    }

    /* Unknown char: skip */
    current_index++;
  }

  tokens[tokens_index].value = NULL;
  tokens[tokens_index].type = END_OF_TOKENS;
  tokens[tokens_index].line_num = line_number;

  free(current);
  return tokens;
}
