#ifndef I_PARSER
#define I_PARSER
#include "tokenizer.h"
#include "linkedlist.h"
#include "type.h"

typedef enum {
    I_AST_EXPR_STRING,
    I_AST_EXPR_MAX
}I_AST_ExprType;

typedef struct {
// For ints, strings, variables
    char *value;
}I_AST_Expr_Arg;

typedef struct {
    I_AST_ExprType type;
    union {
        I_AST_Expr_Arg arg;
    }data;
    struct I_AST_Expr *next;
}I_AST_Expr;

typedef enum {
    I_AST_STATEMENT_FUNCALL,
    I_AST_STATEMENT_MAX
}I_AST_StatementType;

typedef struct {
    char *name; // Function call name
    LinkedList(I_AST_Expr);
}I_AST_Statement_Funcall;

typedef struct {
    I_AST_StatementType type;
    union {
        I_AST_Statement_Funcall funcall;
    }data;
}I_AST_Statement;

typedef enum {
    I_AST_BODY_FUNCDEF,
    I_AST_BODY_MAX
}I_AST_BodyType;

typedef struct {
    char *name; // TODO: Implement more types
    I_Type return_type;
}I_AST_Body_Funcdef;

typedef struct {
    I_AST_BodyType type;
    union {
        I_AST_Body_Funcdef funcdef;
    }data;
    struct I_AST_Body *next;
}I_AST_Body;

typedef struct {
    I_AST_Body *asthead; // First AST
    I_AST_Body *asttail; // Last AST
    int astlen; // Number of ASTs
}I_ASTs;
// Linked list implementation of ASTs

typedef struct {
    I_Token *tokens;
    int tokenlen;

    int cur;


    I_ASTs asts;
}I_Parser;




// TODO: implement types and type parsing (int, bool, etc.)


I_Parser *I_parser_init(I_Tokenizer *tokenizer);
char I_parser_parse_body(I_Parser *parser);

#endif
