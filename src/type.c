

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
