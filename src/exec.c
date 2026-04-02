#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "include/exec.h"

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
        // for (int i=0; i<len; i++){
        //     PopTopLinkedList((*runtime), I_Runtime_Arg);
        // }
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
    I_Runtime_Arg *arg = GetLinkedListHead((*runtime), I_Runtime_Arg);
    if (arg->type == I_RUNTIME_STRING){
        PopTopLinkedList((*runtime), I_Runtime_Arg);
        return (char*)arg->ptr;
    }else {
        assert(0 && "Trying to pop string from non-string argument");
    }
}
int I_runtime_pop_int(I_Runtime *runtime){
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
