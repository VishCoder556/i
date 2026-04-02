#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "include/tokenizer.h"
#include "include/parser.h"
#include "include/exec.h"

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
        if (strcmp(arg, "-o") == 0){
            output_file = I_expect_arg(&i, argc, &argv);
        }else {
            input_file = arg;
        }
    }
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
    I_runtime_add_function(runtime, "print", _print);


    I_Runtime_Function *func = I_runtime_find_function(runtime, "main");
    I_runtime_execute_function(runtime, func);

    free(parser);

    fclose(file);
    return 0;
}
