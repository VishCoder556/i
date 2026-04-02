#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "include/tokenizer.h"

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
