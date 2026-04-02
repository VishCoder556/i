#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define INCLUDE_ONCE
#include "include/parser.h"
#include "type.c"

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
