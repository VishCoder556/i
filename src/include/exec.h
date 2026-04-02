#ifndef I_EXEC
#define I_EXEC

#include "parser.h"

// Execution header file for I programming language


/*
 * I_function_create((I_Runtime *runtime) => {
 *      printf("%s", I_runtime_get_arg(runtime, I_RUNTIME_STRING));
 * });
 *
 *
 *
*/

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
#endif
