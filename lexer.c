#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

size_t line_number = 0;

void print_token(Token token){
  printf("TOKEN VALUE: ");
  printf("'");
  for(int i = 0; token.value && token.value[i] != '\0'; i++){
    printf("%c", token.value[i]);
  }
  printf("'");
  printf("\nline number: %zu", token.line_num);

  switch(token.type){
    case INT:
      printf(" TOKEN TYPE: INT\n");
      break;
    case KEYWORD:
      printf(" TOKEN TYPE: KEYWORD\n");
      break;
    case SEPARATOR:
      printf(" TOKEN TYPE: SEPARATOR\n");
      break;
    case OPERATOR:
      printf(" TOKEN TYPE: OPERATOR\n");
      break;
    case IDENTIFIER:
      printf(" TOKEN TYPE: IDENTIFIER\n");
      break;
    case STRING:
      printf(" TOKEN TYPE: STRING\n");
      break;
    case COMP:
      printf(" TOKEN TYPE: COMPARATOR\n");
      break;
    case END_OF_TOKENS:
      printf(" END OF TOKENS\n");
      break;
    case BEGINNING:
      printf("BEGINNING\n");
      break;
  }
}

Token *generate_number(char *current, int *current_index){
  Token *token = malloc(sizeof(Token));
  token->line_num = line_number;
  token->type = INT;

  char *value = malloc(sizeof(char) * 64);
  int value_index = 0;

  while(isdigit((unsigned char)current[*current_index])){
    value[value_index] = current[*current_index];
    value_index++;
    *current_index += 1;
    if (value_index >= 63) break;
  }
  value[value_index] = '\0';
  token->value = value;
  return token;
}

Token *generate_keyword_or_identifier(char *current, int *current_index){
  Token *token = malloc(sizeof(Token));
  token->line_num = line_number;

  char *keyword = malloc(sizeof(char) * 64);
  int keyword_index = 0;

  while(isalpha((unsigned char)current[*current_index]) && current[*current_index] != '\0'){
    keyword[keyword_index] = current[*current_index];
    keyword_index++;
    *current_index += 1;
    if (keyword_index >= 63) break;
  }
  keyword[keyword_index] = '\0';

  if(strcmp(keyword, "break") == 0){
    token->type = KEYWORD; 
    token->value = "BREAK";
  } 
  else if(strcmp(keyword, "default") == 0){
    token->type = KEYWORD; 
    token->value = "DEFAULT";
  } 
  else if(strcmp(keyword, "func") == 0){
    token->type = KEYWORD; 
    token->value = "FUNC";
  } 
  else if(strcmp(keyword, "interface") == 0){
    token->type = KEYWORD; 
    token->value = "INTERFACE";
  } 
  else if(strcmp(keyword, "select") == 0){
    token->type = KEYWORD; 
    token->value = "SELECT";
  } 
  else if(strcmp(keyword, "case") == 0){
    token->type = KEYWORD; 
    token->value = "CASE";
  } 
  else if(strcmp(keyword, "defer") == 0){
    token->type = KEYWORD; 
    token->value = "DEFER";
  } 
  else if(strcmp(keyword, "go") == 0){
    token->type = KEYWORD; 
    token->value = "GO";
  } 
  else if(strcmp(keyword, "map") == 0){
    token->type = KEYWORD; 
    token->value = "MAP";
  } 
  else if(strcmp(keyword, "struct") == 0){
    token->type = KEYWORD; 
    token->value = "STRUCT";
  } 
  else if(strcmp(keyword, "chan") == 0){
    token->type = KEYWORD; 
    token->value = "CHAN";
  } 
  else if(strcmp(keyword, "else") == 0){
    token->type = KEYWORD; 
    token->value = "ELSE";
  } 
  else if(strcmp(keyword, "goto") == 0){
    token->type = KEYWORD; 
    token->value = "GOTO";
  } 
  else if(strcmp(keyword, "package") == 0){
    token->type = KEYWORD; 
    token->value = "PACKAGE";
  } 
  else if(strcmp(keyword, "switch") == 0){
    token->type = KEYWORD; 
    token->value = "SWITCH";
  } 
  else if(strcmp(keyword, "const") == 0){
    token->type = KEYWORD; 
    token->value = "CONST";
  } 
  else if(strcmp(keyword, "fallthrough") == 0){
    token->type = KEYWORD; 
    token->value = "FALLTHROUGH";
  } 
  else if(strcmp(keyword, "if") == 0){
    token->type = KEYWORD; 
    token->value = "IF";
  } 
  else if(strcmp(keyword, "range") == 0){
    token->type = KEYWORD; 
    token->value = "RANGE";
  } 
  else if(strcmp(keyword, "type") == 0){
    token->type = KEYWORD; 
    token->value = "TYPE";
  } 
  else if(strcmp(keyword, "continue") == 0){
    token->type = KEYWORD; 
    token->value = "CONTINUE";
  } 
  else if(strcmp(keyword, "for") == 0){
    token->type = KEYWORD; 
    token->value = "FOR";
  } 
  else if(strcmp(keyword, "import") == 0){
    token->type = KEYWORD; 
    token->value = "IMPORT";
  } 
  else if(strcmp(keyword, "return") == 0){
    token->type = KEYWORD; 
    token->value = "RETURN";
  } 
  else if(strcmp(keyword, "var") == 0){
    token->type = KEYWORD; 
    token->value = "VAR";
  } 
  else {
    token->type = IDENTIFIER;
    token->value = keyword;
    return token;
  }

  free(keyword);
  return token;
}


Token *generate_string_token(char *current, int *current_index){
  Token *token = malloc(sizeof(Token));
  token->line_num = line_number;

  char *value = malloc(sizeof(char) * 256);
  int value_index = 0;

  *current_index += 1; // skip opening "
  while(current[*current_index] != '"' && current[*current_index] != '\0'){
    value[value_index] = current[*current_index];
    value_index++;
    *current_index += 1;
    if (value_index >= 255) break;
  }
  if (current[*current_index] == '"') {
    // leave closing quote to be taken by a increment
  }
  value[value_index] = '\0';
  token->type = STRING;
  token->value = value;
  return token;
}

Token *generate_separator_or_operator(char *current, int *current_index, TokenType type){
  Token *token = malloc(sizeof(Token));
  token->value = malloc(sizeof(char) * 2);
  token->value[0] = current[*current_index];
  token->value[1] = '\0';
  token->line_num = line_number;
  token->type = type;
  return token;
}

size_t tokens_index;

Token *lexer(FILE *file){
  long length;
  char *current = 0;

  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (length < 0) { fclose(file); return NULL; }

  current = malloc((size_t)length + 1);
  size_t nread = fread(current, 1, (size_t)length, file);
  current[nread] = '\0';

  fclose(file);

  int current_index = 0;

  int number_of_tokens = 12;
  int tokens_size = 0;
  Token *tokens = malloc(sizeof(Token) * number_of_tokens);
  tokens_index = 0;

  while(current[current_index] != '\0'){
    Token *token = malloc(sizeof(Token));
    tokens_size++;
    if(tokens_size > number_of_tokens){
      number_of_tokens = (int)(number_of_tokens * 1.5);
      if (number_of_tokens < tokens_size) number_of_tokens = tokens_size + 1;
      tokens = realloc(tokens, sizeof(Token) * number_of_tokens);
    }

    if(current[current_index] == ';'){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == ','){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '('){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == ')'){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '{'){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '}'){
      token = generate_separator_or_operator(current, &current_index, SEPARATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '='){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '+'){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '-'){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '*'){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '/'){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '%'){
      token = generate_separator_or_operator(current, &current_index, OPERATOR);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(current[current_index] == '"'){
      token = generate_string_token(current, &current_index);
      tokens[tokens_index] = *token;
      tokens_index++;
    } else if(isdigit((unsigned char)current[current_index])){
      token = generate_number(current, &current_index); 
      tokens[tokens_index] = *token;
      tokens_index++;
      current_index--;
    } else if(isalpha((unsigned char)current[current_index])){
      token = generate_keyword_or_identifier(current, &current_index);
      tokens[tokens_index] = *token;
      tokens_index++;
      current_index--;
    } else if(current[current_index] == '\n'){
      line_number += 1;
    } 
    free(token);
    current_index++;
  }

  tokens[tokens_index].value = NULL;
  tokens[tokens_index].type = END_OF_TOKENS;
  tokens[tokens_index].line_num = line_number;
  return tokens;
}
