#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

#define MAX_CURLY_STACK_LENGTH 64

typedef struct {
  Node *content[MAX_CURLY_STACK_LENGTH];
  int top;
} curly_stack;

static Node *init_node(const char *value, TokenType type){
  Node *n = (Node*)malloc(sizeof(Node));
  n->value = value ? strdup(value) : NULL;
  n->type = type;
  n->left = n->right = NULL;
  return n;
}

void print_tree(Node *node, int indent, char *identifier){
  if(!node) return;
  for(int i = 0; i < indent; ++i) printf(" ");
  printf("%s -> %s [%d]\n", identifier, node->value ? node->value : "(null)", node->type);
  print_tree(node->left, indent + 2, "L");
  print_tree(node->right, indent + 2, "R");
}

static Node *peek_curly(curly_stack *stack){
  if (stack->top < 0) return NULL;
  return stack->content[stack->top];
}
static void push_curly(curly_stack *stack, Node *element){
  if (stack->top+1 < MAX_CURLY_STACK_LENGTH){
    stack->top++;
    stack->content[stack->top] = element;
  }
}
static Node *pop_curly(curly_stack *stack){
  if (stack->top < 0) return NULL;
  Node *result = stack->content[stack->top];
  stack->top--;
  return result;
}

/* Build a tiny expr node for 'lhs <comp> rhs' */
static Node *make_binary(Node *lhs, const char *op, Node *rhs){
  Node *cmp = init_node(op, COMP);
  cmp->left = lhs;
  cmp->right = rhs;
  return cmp;
}

static int is_comparator(Token *t){
  return t && t->type == COMP &&
         (strcmp(t->value,"==")==0 || strcmp(t->value,"!=")==0 ||
          strcmp(t->value,"<=")==0 || strcmp(t->value,">=")==0 ||
          strcmp(t->value,"<")==0  || strcmp(t->value,">")==0);
}

/* Parse a lightweight condition between start (exclusive) and the next '{' */
static Node *parse_condition_between(Token *tokens, int start_index, int stop_index){
  /* try to find: (IDENT/INT) COMP (IDENT/INT) */
  Node *lhs = NULL, *rhs = NULL; const char *op = NULL;
  for (int i = start_index; i < stop_index; ++i){
    Token *t = &tokens[i];
    if (!lhs && (t->type == IDENTIFIER || t->type == INT)){
      lhs = init_node(t->value, t->type);
      continue;
    }
    if (!op && is_comparator(t)){
      op = t->value;
      continue;
    }
    if (lhs && op && !rhs && (t->type == IDENTIFIER || t->type == INT)){
      rhs = init_node(t->value, t->type);
      break;
    }
  }
  if (lhs && op && rhs) return make_binary(lhs, op, rhs);
  /* fallback: just return lhs to avoid crashing */
  if (lhs) return lhs;
  return NULL;
}

Node *parser(Token *tokens){
  Node *root = init_node("PROGRAM", BEGINNING);
  Node *current = root;

  curly_stack st; st.top = -1;

  for (int i = 0; tokens[i].type != END_OF_TOKENS; ++i){
    Token *tok = &tokens[i];

    if (tok->type == KEYWORD){
      if (strcmp(tok->value, "IF") == 0){
        Node *if_node = init_node("IF", KEYWORD);
        current->left = if_node;
        /* find the next '{' to know the condition bounds (Go has no required parens) */
        int j = i+1;
        while(tokens[j].type != END_OF_TOKENS){
          if (tokens[j].type == SEPARATOR && strcmp(tokens[j].value,"{")==0) break;
          j++;
        }
        Node *cond = parse_condition_between(tokens, i+1, j);
        if_node->left = cond;
        current = if_node;
        continue;
      }
      if (strcmp(tok->value, "FOR") == 0){
        Node *for_node = init_node("FOR", KEYWORD);
        current->left = for_node;
        /* while-style: e.g., for b != 0 { ... } ; semicolon-style will be ignored (we keep node) */
        int j = i+1;
        while(tokens[j].type != END_OF_TOKENS){
          if (tokens[j].type == SEPARATOR && strcmp(tokens[j].value,"{")==0) break;
          j++;
        }
        Node *cond = parse_condition_between(tokens, i+1, j);
        for_node->left = cond;
        current = for_node;
        continue;
      }
      if (strcmp(tok->value, "RETURN") == 0){
        Node *ret = init_node("RETURN", KEYWORD);
        current->left = ret;
        /* attach next literal/identifier as child if present */
        if (tokens[i+1].type == IDENTIFIER || tokens[i+1].type == INT){
          ret->left = init_node(tokens[i+1].value, tokens[i+1].type);
        }
        current = ret;
        continue;
      }
      if (strcmp(tok->value, "VAR") == 0 || strcmp(tok->value, "CONST") == 0){
        Node *decl = init_node(tok->value, KEYWORD);
        current->left = decl;
        current = decl;
        continue;
      }
      if (strcmp(tok->value, "FUNC") == 0){
        Node *fn = init_node("FUNC", KEYWORD);
        current->left = fn;
        current = fn;
        continue;
      }
      if (strcmp(tok->value, "PACKAGE") == 0){
        Node *pkg = init_node("PACKAGE", KEYWORD);
        current->left = pkg;
        current = pkg;
        continue;
      }
    }

    if (tok->type == SEPARATOR){
      if (strcmp(tok->value, "{") == 0){
        Node *lcurly = init_node("{", SEPARATOR);
        current->right = lcurly;
        push_curly(&st, lcurly);
        continue;
      }
      if (strcmp(tok->value, "}") == 0){
        Node *open = pop_curly(&st);
        Node *rcurly = init_node("}", SEPARATOR);
        if (open) open->right = rcurly;
        current = rcurly;
        continue;
      }
    }

    if (tok->type == IDENTIFIER){
      /* simple usage or call start */
      Node *id = init_node(tok->value, IDENTIFIER);
      current->right = id;
      current = id;
      continue;
    }
  }

  return root;
}
