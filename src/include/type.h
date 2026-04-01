#ifndef I_TYPE
#define I_TYPE

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

#ifdef INCLUDE_ONCE

I_Type_Definition I_Type_Definitions[] = {
    I_Type_Definition(I_Type(I_KIND_INT, "int")),
};
int I_Type_DefinitionLen = 1; // Please update as you add more types

#endif

void I_type_add(I_Type_Definition definition);
char I_type_is(char *type, I_Type *typ); // For parsing: checks whether a given string corresponds to a type and which type it corresponds to

#endif
