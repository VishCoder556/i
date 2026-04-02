#ifndef I_TOKENIZER
#define I_TOKENIZER

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

#endif
