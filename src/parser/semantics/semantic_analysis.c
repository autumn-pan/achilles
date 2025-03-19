#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "/workspaces/achilles/src/ast/ast.c"

bool validate_binary_operation(ASTNode * node)
{

    NodeType type = node->type;

    // Define the types of the parent node's children
    NodeType c_type_0 = node->children[0]->type;
    NodeType c_type_1 = node->children[1]->type;

    //First, check if it is a string concatenation
    if (type == ADD)
    {
        if (c_type_1 == STR_LITERAL || c_type_0 == STR_LITERAL)
        {
            node->type = CONCATENATE;
            return true;
        }
        return false;
    }

    // Validate numerical operators
    if (type == ADD || type == SUB || type == MULT || type == DIV || type == RIGHT_SHIFT || type == LEFT_SHIFT || type == EQUAL || type == LESS_EQUAL || type == GREATER_EQUAL || type == GREATER || type == LESS || type == MOD)
    {
        if((c_type_0 == INT_LITERAL || c_type_0 == FLOAT_LITERAL || c_type_1 == IDENTIFIER) && (c_type_1 == INT_LITERAL || c_type_1 == FLOAT_LITERAL || c_type_1 == IDENTIFIER))
            return true;
        return false;
    }

    // Validate assignment operators
    if (type == ASSIGN || type == MINUS_EQUAL || type == PLUS_EQUAL)
    {
        if(c_type_0 == IDENTIFIER)
        {
            if(c_type_1 == INT_LITERAL || c_type_1 == FLOAT_LITERAL || c_type_1 == STR_LITERAL || c_type_1 == CHAR_LITERAL || c_type_1 == IDENTIFIER)
                return true;
        }
        return false;
    }

    // Validate boolean operators
    if (type == OR || type == AND || type == NOT)
    {
        if ((c_type_0 == BOOL_LITERAL || c_type_0 == IDENTIFIER) && (c_type_1 == BOOL_LITERAL || c_type_1 == IDENTIFIER))
            return true;
        return false;
    }

}

bool validate_variable_call(ASTNode * node)
{
    NodeType type = node->type;

    if
}


