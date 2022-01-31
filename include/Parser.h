#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"

enum ParserErrorCode
{
    PARSER_NO_ERROR,
    PARSER_SYNTAX_ERROR
};

struct Parser
{
    char* str;
    size_t curOffset;
    ParserErrorCode parserError;
};

struct UnaryOperation
{
    const char* str;
    size_t strSize;
    double (*operation) (double);
    NodeType nodeType;
};

Node_t* ParserExpression(Parser *parser);

#endif // PARSER_H_
