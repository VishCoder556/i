#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ----- UTILITIES -------

// Used for struct definitions
#define LinkedList(type) type *type##_head; type *type##_tail; int type##_len;

#define InitLinkedList(parent, type) parent.type##_head = NULL; parent.type##_tail = NULL; parent.type##_len = 0;

#define AppendToLinkedListPrefix(prefix, type, ...) \
if (prefix##_head == NULL){ \
    prefix##_head = malloc(sizeof(type)); \
    prefix##_tail = prefix##_head; \
    prefix##_head->next = NULL; \
    *prefix##_tail = __VA_ARGS__; \
}else if (prefix##_tail != NULL){ \
    prefix##_tail->next = malloc(sizeof(type)); \
    prefix##_tail = prefix##_tail->next; \
    *prefix##_tail = __VA_ARGS__; \
}else { \
    assert(0 && "Unreachable code: the tail should only be a null pointer if the head is also a null pointer -- error in linked list"); \
} \
    prefix##_len++; \

#define AppendToLinkedList(parent, type, ...) AppendToLinkedListPrefix(parent.type, type, __VA_ARGS__)

#define SetLinkedList(ll1, ll2, type) \
    ll1.type##_head = ll2.type##_head; \
    ll1.type##_tail = ll2.type##_tail; \
    ll1.type##_len = ll2.type##_len;

#define GetLinkedListHead(ll, type) ll.type##_head
#define GetLinkedListTail(ll, type) ll.type##_tail
#define GetLinkedListLen(ll, type) ll.type##_len
#define GetLinkedListNextElem(ll) ll->next

#define PopTopLinkedList(parent, type) GetLinkedListHead(parent, type) = GetLinkedListNextElem(GetLinkedListHead(parent, type)); GetLinkedListLen(parent, type)--;


// Go through each element in a linked list and free them
#define FreeLinkedList(ll, type) \
    type *last_elem == ll.type##_head; \
    for (int i=0; i<ll.type##_len; i++){ \
        type *old = last_elem; \
        last_elem = last_elem->next; \
        free(old); \
    }; \
    free(last_elem);


// ----- TOKENIZER -------
typedef enum {
    I_TOKEN_ID,
    I_TOKEN_EQ,
    I_TOKEN_INT,
    I_TOKEN_LB,
    I_TOKEN_RB,
    I_TOKEN_LP,
    I_TOKEN_RP,
    I_TOKEN_STRING,
    I_TOKEN_COMMA,
    I_TOKEN_MAX, // Marker to know how many tokens we have
    // Keep adding on eventually
}I_TokenType;

typedef struct {
    I_TokenType type;
    char *value;
}I_Token;

typedef struct {
    char *input_file;
    char *buffer;
    int bufferlen;


    I_Token *tokens;
    int tokencap;
    int tokenlen;


    int cur;

    int col;
    int row;
}I_Tokenizer;


I_Tokenizer *I_tokenizer_init(char *input_file, char *buffer);

char I_tokenizer_token(I_Tokenizer *tokenizer);



// ----- TYPES -------

typedef enum {
    I_KIND_INT,
    I_KIND_PTR,
    I_KIND_UNKNOWN,
    I_KIND_MAX,
}I_Type_Kind;

typedef struct {
    I_Type_Kind kind;
    char *name;
}I_Type;

typedef struct {
    I_Type type;
}I_Type_Definition;

#define I_Type(kind, name) (I_Type){kind, name}

#define I_Type_Definition(...) (I_Type_Definition){__VA_ARGS__}

I_Type_Definition I_Type_Definitions[] = {
    I_Type_Definition(I_Type(I_KIND_INT, "int")),
};
int I_Type_DefinitionLen = 1; // Please update as you add more types

void I_type_add(I_Type_Definition definition);
char I_type_is(char *type, I_Type *typ); // For parsing: checks whether a given string corresponds to a type and which type it corresponds to




// ----- PARSER -------


typedef enum {
    I_AST_EXPR_STRING,
    I_AST_EXPR_INT,
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
    struct I_AST_Statement *next;
}I_AST_Statement;

typedef enum {
    I_AST_BODY_FUNCDEF,
    I_AST_BODY_MAX
}I_AST_BodyType;

typedef struct {
    char *name; // TODO: Implement more types
    I_Type return_type;
    LinkedList(I_AST_Statement);
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


I_Parser *I_parser_init(I_Tokenizer *tokenizer);
char I_parser_parse_body(I_Parser *parser);

// ----- RUNTIME -------

typedef enum {
    I_RUNTIME_STRING,
    I_RUNTIME_INT,
    I_RUNTIME_MAX
}I_Runtime_ArgType;

typedef struct {
    I_Runtime_ArgType type;
    void *ptr;
    // Yes, void* as a generic isn't good, but it works for now
    struct I_Runtime_Arg *next;
}I_Runtime_Arg;


typedef struct {
    LinkedList(I_AST_Statement);
}I_Runtime_Function_Native;

struct I_Runtime;
typedef struct {
    void (*callback)(struct I_Runtime *runtime);
}I_Runtime_Function_C;

typedef enum {
    I_RUNTIME_FUNCTION_C,
    I_RUNTIME_FUNCTION_NATIVE
}I_Runtime_FunctionType;
typedef struct {
    I_Runtime_FunctionType type;
    char *name;
    union {
        I_Runtime_Function_Native native;
        I_Runtime_Function_C c;
    }data;
    struct I_Runtime_Function *next;
}I_Runtime_Function;

// Runtime struct for I, contains important metadata
typedef struct {
    LinkedList(I_AST_Body); // These are the old ASTs that will be converted soon
    I_AST_Body *cur;
    int idx;

    LinkedList(I_Runtime_Arg); // I features an argument stack for functions
    LinkedList(I_Runtime_Function); // List of functions defined
}I_Runtime;


I_Runtime *I_runtime_init(I_Parser *parser);

// I_runtime_setup gets the runtime ready for execution
char I_runtime_setup(I_Runtime *runtime);

I_Runtime_Function *I_runtime_find_function(I_Runtime *runtime, char *name);


void I_runtime_execute_function(I_Runtime *runtime, I_Runtime_Function *func);


void I_runtime_add_function(I_Runtime *runtime, char *name, void (*callback)(struct I_Runtime *runtime));

char *I_runtime_pop_string(I_Runtime *runtime);

int I_runtime_pop_int(I_Runtime *runtime);

I_Runtime_ArgType I_runtime_get_arg_type(I_Runtime *runtime);

int I_runtime_get_args_count(I_Runtime *runtime);

I_Runtime *I_runtime_from_file(char *input_file);

I_Runtime *I_runtime_from_code(char *input_file, char *buffer);




// ---------- IMPLEMENTATION ----------



// ----- TOKENIZER -------

I_Tokenizer *I_tokenizer_init(char *input_file, char *buffer){
    I_Tokenizer *tokenizer = malloc(sizeof(I_Tokenizer));
    tokenizer->input_file = input_file;
    tokenizer->buffer = buffer;
    tokenizer->bufferlen = strlen(buffer);
    tokenizer->cur = 0;
    tokenizer->col = 0;
    tokenizer->row = 0;
    tokenizer->tokencap = 10; // For now
    tokenizer->tokenlen = 0;
    tokenizer->tokens = malloc(sizeof(I_Token) * tokenizer->tokencap);
    return tokenizer;
}

void I_tokenizer_append(I_Tokenizer *tokenizer, I_TokenType type, char *value){
    if (tokenizer->tokenlen >= tokenizer->tokencap){
        tokenizer->tokencap += 10;
        tokenizer->tokens = realloc(tokenizer->tokens, sizeof(I_Token) * tokenizer->tokencap);
    }
    tokenizer->tokens[tokenizer->tokenlen++] = (I_Token){type, value};
}

char I_tokenizer_peek(I_Tokenizer *tokenizer){
    if (tokenizer->cur <= tokenizer->bufferlen){
        char c = tokenizer->buffer[tokenizer->cur];
        tokenizer->cur++;
        tokenizer->col++;
        return c;
    }
    return '\0';
}

char *I_char_to_string(char c){
    char *string = malloc(2);
    string[0] = c;
    string[1] = '\0';
    return string;
}

#define I_tokenizer_advance(tokenizer)  I_char_to_string(I_tokenizer_peek(tokenizer))
// Get the current character as a string and advance

char I_tokenizer_token(I_Tokenizer *tokenizer){
    char c = tokenizer->buffer[tokenizer->cur];

    assert(I_TOKEN_MAX == 9 && "Exhaustive handling of tokens -- please implement token here");
    // Assertion style copied from Tsoding Daily in his programming language Porth (P.S. you should check it out);

    switch (c){
        case ',': I_tokenizer_append(tokenizer, I_TOKEN_COMMA, I_tokenizer_advance(tokenizer)); break;
        case '=': I_tokenizer_append(tokenizer, I_TOKEN_EQ, I_tokenizer_advance(tokenizer)); break;
        case '{': I_tokenizer_append(tokenizer, I_TOKEN_LB, I_tokenizer_advance(tokenizer)); break;
        case '}': I_tokenizer_append(tokenizer, I_TOKEN_RB, I_tokenizer_advance(tokenizer)); break;
        case '(': I_tokenizer_append(tokenizer, I_TOKEN_LP, I_tokenizer_advance(tokenizer)); break;
        case ')': I_tokenizer_append(tokenizer, I_TOKEN_RP, I_tokenizer_advance(tokenizer)); break;
        case '\n': tokenizer->row++; tokenizer->col = 0; tokenizer->cur++; break;
        case ' ': I_tokenizer_peek(tokenizer); break;
        case '\"':
            I_tokenizer_peek(tokenizer);
            int valuecap = 100;
            char *value = malloc(valuecap);
            int len = 0;
            c = tokenizer->buffer[tokenizer->cur];
            while (c != '\"'){
                if (len >= valuecap){
                    valuecap += 5;
                    value = realloc(value, valuecap);
                }
                value[len++] = c;
                I_tokenizer_peek(tokenizer);
                c = tokenizer->buffer[tokenizer->cur];
            }
            if (I_tokenizer_peek(tokenizer) != '\"'){
                assert(0 && "Unreachable code");
            }
            I_tokenizer_append(tokenizer, I_TOKEN_STRING, value);
            break;
        case '\t': break;
        case '\0': return -1;
        default:
            if (isalpha(c)){
                int valuecap = 100;
                char *value = malloc(valuecap);
                int len = 0;
                while (isalpha(c)){
                    if (len >= valuecap){
                        valuecap += 5;
                        value = realloc(value, valuecap);
                    }
                    value[len++] = c;
                    I_tokenizer_peek(tokenizer);
                    c = tokenizer->buffer[tokenizer->cur];
                }
                I_tokenizer_append(tokenizer, I_TOKEN_ID, value);
                break;
            }else if (isnumber(c)){
                int valuecap = 100;
                char *value = malloc(valuecap);
                int len = 0;
                while (isnumber(c)){
                    if (len >= valuecap){
                        valuecap += 5;
                        value = realloc(value, valuecap);
                    }
                    value[len++] = c;
                    I_tokenizer_peek(tokenizer);
                    tokenizer->col++;
                    c = tokenizer->buffer[tokenizer->cur];
                }
                I_tokenizer_append(tokenizer, I_TOKEN_INT, value);
                break;
            }
    }
skip_increment:
    return 0;
}







// ----- TYPES -------

void I_type_add(I_Type_Definition definition){
    I_Type_Definitions[I_Type_DefinitionLen++] = definition;
};
char I_type_is(char *type, I_Type *typ){
    for (int i=0; i<I_Type_DefinitionLen; i++){
        char *type1 = I_Type_Definitions[i].type.name;
        if (type != NULL && type1 != NULL){
            if (strcmp(type, type1) == 0){
                *typ = I_Type_Definitions[i].type;
                return 1;
            };
        }
    }
    return 0;
};




// ----- PARSER -------

I_Parser *I_parser_init(I_Tokenizer *tokenizer){
    I_Parser *parser = malloc(sizeof(I_Parser));
    parser->tokens = tokenizer->tokens;
    parser->tokenlen = tokenizer->tokenlen;

    parser->asts.asthead = NULL;
    parser->asts.asttail = NULL;
    parser->asts.astlen = 0;

    parser->cur = 0;
    return parser;
}

void I_parser_peek(I_Parser *parser){
    if (parser->cur > parser->tokenlen){
        assert(0 && "Error: Peeking past standard bounds of tokens");
    }
    parser->cur++;
}

char I_parser_check(I_Parser *parser){
    if (parser->cur > parser->tokenlen){
        return 1;
    }
    parser->cur++;
    return 0;
}

void I_parser_expect(I_Parser *parser, int token){
    if (parser->tokens[parser->cur].type == token){
        I_parser_peek(parser);
    }else {
        assert(0 && "Error: Expecting went wrong");
        // Todo: stop making these asserts and add error messages
    };
}

I_AST_Body *I_parser_next_ast_body(I_Parser *parser){
    if (parser->asts.asthead == NULL){
        // First AST
        parser->asts.asthead = malloc(sizeof(I_AST_Body));
        parser->asts.asttail = parser->asts.asthead;
        parser->asts.asthead->next = NULL;
        return parser->asts.asttail;
    }else if (parser->asts.asttail != NULL){
        parser->asts.asttail->next = malloc(sizeof(I_AST_Body));
        parser->asts.asttail = parser->asts.asttail->next;
        return parser->asts.asttail;
    }
    parser->asts.astlen++;
    assert(0 && "Unreachable code: the tail should only be a null pointer if the head is also a null pointer");
    return NULL;
}

I_AST_Expr I_parser_parse_expr(I_Parser *parser){
    I_AST_Expr expr = (I_AST_Expr){0};
    I_Token tok = parser->tokens[parser->cur];
    assert(I_AST_EXPR_MAX == 2 && "Exhaustive handling of AST exprs -- please implement how tokens can be parsed to the AST");
    expr.type = I_AST_EXPR_MAX;
    if (tok.type == I_TOKEN_STRING){
        expr.type = I_AST_EXPR_STRING;
        expr.data.arg.value = tok.value;
    }else if(tok.type == I_TOKEN_INT){
        expr.type = I_AST_EXPR_INT;
        expr.data.arg.value = tok.value;
    }
    I_parser_peek(parser);
    return expr;
}
I_AST_Statement I_parser_parse_statement(I_Parser *parser){
    I_AST_Statement stmnt = (I_AST_Statement){0};
    I_Token tok = parser->tokens[parser->cur];
    assert(I_AST_STATEMENT_MAX == 1 && "Exhaustive handling of AST statements -- please implement how tokens can be parsed to the AST");
    stmnt.type = I_AST_STATEMENT_MAX;
    if (tok.type == I_TOKEN_ID){
        InitLinkedList(stmnt.data.funcall, I_AST_Expr);
        stmnt.type = I_AST_STATEMENT_FUNCALL;
        stmnt.data.funcall.name = tok.value;
        I_parser_peek(parser);
        I_parser_expect(parser, I_TOKEN_LP); // Expect funcall
        while (parser->tokens[parser->cur].type != I_TOKEN_RP){
            I_AST_Expr expr = I_parser_parse_expr(parser);
            if (expr.type == I_AST_EXPR_MAX){
                assert(0 && "Something weird happened here || an error in the expr parsing that is apparent in the function statement argument parsing");
            }
            AppendToLinkedList(stmnt.data.funcall, I_AST_Expr, expr);
            if (parser->tokens[parser->cur].type != I_TOKEN_RP){
                I_parser_expect(parser, I_TOKEN_COMMA);
            }
        };
        I_parser_expect(parser, I_TOKEN_RP);
    }
    return stmnt;
}


char I_parser_parse_body(I_Parser *parser){
    if (parser->cur > parser->tokenlen){
        return 0;
    }
    I_Token token = parser->tokens[parser->cur];
    assert(I_TOKEN_MAX == 9 && "Exhaustive handling of tokens -- please implement how the token should be parsed here");
    assert(I_AST_BODY_MAX == 1 && "Exhaustive handling of ASTs -- please implement how tokens can be parsed to the AST");
    if (token.type == I_TOKEN_ID){
        I_Type type;
        if (I_type_is(token.value, &type) == 1){
            // Expect the AST to be a function definition
            I_AST_Body *next_elem = I_parser_next_ast_body(parser);
            next_elem->type = I_AST_BODY_FUNCDEF;
            next_elem->data.funcdef.return_type = type;
            I_parser_peek(parser);
            token = parser->tokens[parser->cur];
            I_parser_expect(parser, I_TOKEN_ID);
            next_elem->data.funcdef.name = token.value; // Function name
            I_parser_expect(parser, I_TOKEN_LP);
            token = parser->tokens[parser->cur];
            // TODO: Add function arguments
            I_parser_expect(parser, I_TOKEN_RP);
            I_parser_expect(parser, I_TOKEN_LB);


            InitLinkedList(next_elem->data.funcdef, I_AST_Statement);
            while (parser->tokens[parser->cur].type != I_TOKEN_RB){
                I_AST_Statement stmnt = I_parser_parse_statement(parser);
                if (stmnt.type == I_AST_STATEMENT_MAX){
                    assert(0 && "Something weird happened here || an error in the statement parsing that is apparent in the function definition body parsing");
                }
                AppendToLinkedList(next_elem->data.funcdef, I_AST_Statement, stmnt);
                // Evaluate loop
            }
            I_parser_expect(parser, I_TOKEN_RB);
        }
    }
    if (I_parser_check(parser) != 0){
        // Past standard bounds, return
        return 0;
    }
    return 1;
}




// ----- RUNTIME -------

I_Runtime *I_runtime_init(I_Parser *parser){
    I_Runtime *runtime = malloc(sizeof(I_Runtime));

    runtime->I_AST_Body_head = parser->asts.asthead;
    runtime->I_AST_Body_tail = parser->asts.asttail;
    runtime->I_AST_Body_len = parser->asts.astlen;

    InitLinkedList((*runtime), I_Runtime_Arg);
    InitLinkedList((*runtime), I_Runtime_Function);

    runtime->cur = GetLinkedListHead((*runtime), I_AST_Body);
    runtime->idx = 0;

    return runtime;
}

char I_runtime_advance(I_Runtime *runtime){
    if (runtime->idx >= GetLinkedListLen((*runtime), I_AST_Body)){
        return 1;
    }
    runtime->cur = GetLinkedListNextElem(runtime->cur);
    runtime->idx++;
    return 0;
}


char I_runtime_setup(I_Runtime *runtime){
    I_AST_Body *cur = runtime->cur;
    if (cur->type == I_AST_BODY_FUNCDEF){
// Runtime Setup is pretty simple: it just gets all of the structures ready
        I_Runtime_Function func = (I_Runtime_Function){0};
        func.name = cur->data.funcdef.name;
        func.type = I_RUNTIME_FUNCTION_NATIVE;
        SetLinkedList(func.data.native, cur->data.funcdef, I_AST_Statement);
        AppendToLinkedList((*runtime), I_Runtime_Function, func);
    };
    if (I_runtime_advance(runtime) == 1){
        return 0;
    };
    return 1;
    // I_AST_Body body = ;
}

I_Runtime_Function *I_runtime_find_function(I_Runtime *runtime, char *name){
    I_Runtime_Function *func = GetLinkedListHead((*runtime), I_Runtime_Function);
    for (int i=0; i<GetLinkedListLen((*runtime), I_Runtime_Function); i++){
        if (strcmp(func->name, name) == 0){
            return func;
        };
        func = GetLinkedListNextElem(func);
    };
    assert(0 && "Fatal Error: Could not find function");
}

I_Runtime_Arg I_runtime_run_expr(I_Runtime *runtime, I_AST_Expr *expr){
    I_Runtime_Arg arg = (I_Runtime_Arg){0};
    assert(I_RUNTIME_MAX == 2 && "Exhaustive handling of runtime argument in execution");
    arg.type = I_RUNTIME_MAX;
    if (expr->type == I_AST_EXPR_STRING){
        arg.type = I_RUNTIME_STRING;
        arg.ptr = expr->data.arg.value;
    }else if (expr->type == I_AST_EXPR_INT){
        arg.type = I_RUNTIME_INT;
        arg.ptr = (void*)atoi(expr->data.arg.value);
    };
    return arg;
}

void I_runtime_execute_function(I_Runtime *runtime, I_Runtime_Function *func);
void I_runtime_run_statement(I_Runtime *runtime, I_AST_Statement *stmnt){
    if (stmnt->type == I_AST_STATEMENT_FUNCALL){
        I_AST_Expr *expr = GetLinkedListHead(stmnt->data.funcall, I_AST_Expr);
        int len = GetLinkedListLen(stmnt->data.funcall, I_AST_Expr);
        for (int i=0; i<len; i++){
            I_Runtime_Arg arg = I_runtime_run_expr(runtime, expr);
            AppendToLinkedList((*runtime), I_Runtime_Arg, arg);
            expr = GetLinkedListNextElem(expr);
        };
        I_Runtime_Function *func = I_runtime_find_function(runtime, stmnt->data.funcall.name);
        I_runtime_execute_function(runtime, func);
    };
}

void I_runtime_add_function(I_Runtime *runtime, char *name, void (*callback)(struct I_Runtime *runtime)){
    I_Runtime_Function func;
    func.name = name;
    func.type = I_RUNTIME_FUNCTION_C;
    func.data.c.callback = callback;
    AppendToLinkedList((*runtime), I_Runtime_Function, func);
}

void I_runtime_execute_function(I_Runtime *runtime, I_Runtime_Function *func){
    if (func->type == I_RUNTIME_FUNCTION_NATIVE){
        I_AST_Statement *stmnt = GetLinkedListHead(func->data.native, I_AST_Statement);
        for (int i=0; i<GetLinkedListLen(func->data.native, I_AST_Statement); i++){
            I_runtime_run_statement(runtime, stmnt);
            stmnt = GetLinkedListNextElem(stmnt);
        }
    }else if (func->type == I_RUNTIME_FUNCTION_C){
        func->data.c.callback(runtime);
    }
}

char *I_runtime_pop_string(I_Runtime *runtime){
    if (GetLinkedListLen((*runtime), I_Runtime_Arg) <= 0){
        assert(0 && "Attempting to pop from empty list");
    }
    I_Runtime_Arg *arg = GetLinkedListHead((*runtime), I_Runtime_Arg);
    if (arg->type == I_RUNTIME_STRING){
        PopTopLinkedList((*runtime), I_Runtime_Arg);
        return (char*)arg->ptr;
    }else {
        assert(0 && "Trying to pop string from non-string argument");
    }
}
int I_runtime_pop_int(I_Runtime *runtime){
    if (GetLinkedListLen((*runtime), I_Runtime_Arg) <= 0){
        assert(0 && "Attempting to pop from empty list");
    }
    I_Runtime_Arg *arg = GetLinkedListHead((*runtime), I_Runtime_Arg);
    if (arg->type == I_RUNTIME_INT){
        PopTopLinkedList((*runtime), I_Runtime_Arg);
        return (int)arg->ptr;
    }else {
        assert(0 && "Trying to pop int from non-int argument");
    }
}

I_Runtime_ArgType I_runtime_get_arg_type(I_Runtime *runtime){
    I_Runtime_Arg *arg = GetLinkedListHead((*runtime), I_Runtime_Arg);
    return arg->type;
}

int I_runtime_get_args_count(I_Runtime *runtime){
    return GetLinkedListLen((*runtime), I_Runtime_Arg);
}

I_Runtime *I_runtime_from_code(char *input_file, char *buffer){
    I_Tokenizer *tokenizer = I_tokenizer_init(input_file, buffer);
    while (I_tokenizer_token(tokenizer) == 0){
    };
    free(tokenizer->buffer);

    I_Parser *parser = I_parser_init(tokenizer);

    while (I_parser_parse_body(parser) != 0){

    }
    free(tokenizer);
    free(parser->tokens);

    I_Runtime *runtime = I_runtime_init(parser);
    while (I_runtime_setup(runtime) != 0){

    };
    return runtime;
}

I_Runtime *I_runtime_from_file(char *input_file){
    FILE *file = fopen(input_file, "r");
    if (file == NULL){
        fprintf(stderr, "ERROR: Input File '%s' does not exist\n", input_file);
        exit(-1);
    };

    fseek(file, 0, SEEK_END);
    long count = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(count + 1);
    fread(buffer, 1, count, file);
    buffer[count] = '\0';
    fclose(file);

    return I_runtime_from_code(input_file, buffer);
}
