#include <assert.h>
#include <math.h>
#include <string.h>
#include "../include/Parser.h"

#define IS_PARSER_ERROR()                           \
    do                                              \
    {                                               \
        if (parser->parserError != PARSER_NO_ERROR) \
        {                                           \
            return nullptr;                         \
        }                                           \
    } while(0)

static Node_t* GetG    (Parser *parser);
static Node_t* GetE    (Parser *parser);
static Node_t* GetT    (Parser *parser);
static Node_t* GetPower(Parser *parser);
static Node_t* GetUnary(Parser *parser);
static Node_t* GetP    (Parser *parser);
static Node_t* GetId   (Parser *parser);
static Node_t* GetN    (Parser *parser);

const int NUMBER_UNARY_OPERATIONS = 3;
const double NO_VALUE = -1.0;

UnaryOperation unaryOperation[NUMBER_UNARY_OPERATIONS] = {{ "sin", 3, sin, SIN},
                                                          { "cos", 3, cos, COS},
                                                          { "ln" , 2, log, LN}};

static void Require(Parser *parser, const char symbol)
{
    assert(parser != nullptr);

    if (*(parser->str + parser->curOffset) == symbol) parser->curOffset++;
    else parser->parserError = PARSER_SYNTAX_ERROR;
}

static Node_t* NodeCtor(Node_t *thisNode)
{
    assert(thisNode != nullptr);

    Node_t *node = (Node_t*)calloc(1, sizeof(Node_t));
    node->parent = thisNode->parent;
    node->leftChild  = thisNode->leftChild;
    node->rightChild = thisNode->rightChild;
    node->nodeType = thisNode->nodeType;
    node->value = thisNode->value;

    if (thisNode->leftChild  != nullptr) { thisNode->leftChild->parent  = node; }
    if (thisNode->rightChild != nullptr) { thisNode->rightChild->parent = node; }

    return node;
}

static Node_t* GetG(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetE(parser);
    IS_PARSER_ERROR();

    Require(parser, '$');
    IS_PARSER_ERROR();

    return val;
}

static Node_t* GetE(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetT(parser);
    IS_PARSER_ERROR();

    while (*(parser->str + parser->curOffset) == '+' || *(parser->str + parser->curOffset) == '-')
    {
        char op = *(parser->str + parser->curOffset);
        parser->curOffset++;
        Node_t *val2 = GetT(parser);
        IS_PARSER_ERROR();
        if (op == '+')
        {
            Node_t node = {nullptr, val, val2, ADD, NO_VALUE};
            val = NodeCtor(&node);
        }
        else
        {
            Node_t node = {nullptr, val, val2, SUB, NO_VALUE};
            val = NodeCtor(&node);
        }
    }

    return val;
}

static Node_t* GetT(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetPower(parser);
    IS_PARSER_ERROR();

    while (*(parser->str + parser->curOffset) == '*' || *(parser->str + parser->curOffset) == '/')
    {
        char op = *(parser->str + parser->curOffset);
        parser->curOffset++;
        Node_t *val2 = GetPower(parser);
        IS_PARSER_ERROR();
        if (op == '*')
        {
            Node_t node = {nullptr, val, val2, MUL, NO_VALUE};
            val = NodeCtor(&node);
        }
        else
        {
            Node_t node = {nullptr, val, val2, DIV, NO_VALUE};
            val = NodeCtor(&node);
        }
    }

    return val;
}

static Node_t* GetPower(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = GetP(parser);
    while (*(parser->str + parser->curOffset) == '^')
    {
        parser->curOffset++;
        Node_t *val2 = GetP(parser);
        IS_PARSER_ERROR();
        Node_t node = {nullptr, val, val2, POW, NO_VALUE};

        val = NodeCtor(&node);
    }

    return val;
}

static Node_t* GetP(Parser *parser)
{
    assert(parser != nullptr);

    Node_t *val = nullptr;

    if (*(parser->str + parser->curOffset) == '(')
    {
        parser->curOffset++;
        val = GetE(parser);
        IS_PARSER_ERROR();
        Require(parser, ')');
    }
    else
    {
        size_t curOffset = parser->curOffset;
        val = GetUnary(parser);
        if (curOffset == parser->curOffset) { val = GetId(parser); }
        if (curOffset == parser->curOffset) { val = GetN(parser) ; }
        IS_PARSER_ERROR();
    }

    return val;
}

static Node_t* GetUnary(Parser *parser)
{
    assert(parser != nullptr);

    for (int i = 0; i < NUMBER_UNARY_OPERATIONS; i++)
    {
        if (strncmp(unaryOperation[i].str, (parser->str + parser->curOffset), unaryOperation[i].strSize) == 0)
        {
            char op[unaryOperation[i].strSize] = {};
            strncpy(op, parser->str + parser->curOffset, unaryOperation[i].strSize);
            parser->curOffset += unaryOperation[i].strSize;
            Node_t *val = GetP(parser);
            Node_t node = {nullptr, val, nullptr, unaryOperation[i].nodeType, NO_VALUE};

            return NodeCtor(&node);
        }
    }

    return nullptr;
}

static Node_t* GetId(Parser *parser)
{
    assert(parser != nullptr);

    if (*(parser->str + parser->curOffset) == 'x')
    {
        parser->curOffset++;

        Node_t node = {nullptr, nullptr, nullptr, VARIABLE, NO_VALUE};
        return NodeCtor(&node);
    }

    return nullptr;
}

static Node_t* GetN(Parser *parser)
{
    assert(parser != nullptr);

    double val = 0;
    size_t curOffset = parser->curOffset;
    const char *oldS = parser->str;
    while (*(parser->str + parser->curOffset) >= '0' && '9' >= *(parser->str + parser->curOffset))
    {
        val = val * 10 + (*(parser->str + parser->curOffset) - '0');
        parser->curOffset++;
    }
    if (oldS == (parser->str + parser->curOffset)) parser->parserError = PARSER_SYNTAX_ERROR;

    if (curOffset == parser->curOffset)
    {
        return nullptr;
    }
    else
    {
        Node_t node = {nullptr, nullptr, nullptr, CONST, val};
        return NodeCtor(&node);
    }
}

#undef IS_PARSER_ERROR

Node_t* ParserExpression(Parser *parser)
{
    assert(parser != nullptr);

    Node_t* val = GetG(parser);

    return val;
}
