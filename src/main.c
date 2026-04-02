#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "include/i.h"

char *I_expect_arg(int *argused, int argc, char ***argv){
// Pass by reference to make sure we change the main function
    char *str = **argv;
    (*(argv))++;
    (*argused)++;
    if (*argused > argc){
        assert(0 && "An argument-requiring option didn't get its argument");
    }
    return str;
}

void _print(struct I_Runtime *runtime){
    for (int i=0; i<I_runtime_get_args_count(runtime); i++){
        switch (I_runtime_get_arg_type(runtime)){
            case I_RUNTIME_STRING: printf("%s ", I_runtime_pop_string(runtime)); break;
            case I_RUNTIME_INT: printf("%d ", I_runtime_pop_int(runtime)); break;
            default: assert(0 && "Unknown type found");
        };
    }
    printf("\n");
}

int main(int argc, char **argv){
    char *input_file = "";
    char *output_file = "";
    int i = 0;
    while (*argv) {
        char *arg = I_expect_arg(&i, argc, &argv);
        input_file = arg;
    }
    I_Runtime *runtime = I_runtime_from_file(input_file);
    I_runtime_add_function(runtime, "print", _print);


    I_Runtime_Function *func = I_runtime_find_function(runtime, "main");
    I_runtime_execute_function(runtime, func);

    return 0;
}
