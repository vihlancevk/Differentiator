#ifndef TREE_H_
#define TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cstddef>

typedef char elem_t;

enum NodeChild
{
    RIGHT_CHILD,
    LEFT_CHILD,
};

enum TreeStatus
{
    TREE_NOT_CONSTRUCTED,
    TREE_CONSTRUCTED,
    TREE_DESTRUCTED,
};

enum TreeErrorCode
{
    TREE_NO_ERROR,
    TREE_CONSTRUCTED_ERROR,
    TREE_INSERT_ERROR,
    TREE_DESTRUCTED_ERROR,
    TREE_FILL_ERROR,
    TREE_OBJECT_DEFINITION_MODE_ERROR,
};

//! Number of operation ADD (and SUB, MUL, DIV, DEGREE) is number this symbol in ASCII table

enum NodeOperationType
{
    CONST = 0,
    VARIABLE = 1,
    SIN = 2,
    COS = 3,
    LN = 4,
    ADD = 43,
    SUB = 45,
    MUL = 42,
    DIV = 47,
    DEGREE = 94,
};

struct Node_t
{
    elem_t *elem;
    Node_t *parent;
    Node_t *leftChild;
    Node_t *rightChild;
    NodeOperationType nodeType;
    double value;
    size_t num;
};

struct Tree_t
{
    Node_t *root;
    size_t size;
    TreeStatus status;
};

void TreeDump(Tree_t *tree);

TreeErrorCode TreeCtor(Tree_t *tree);

Node_t* TreeInsert(Tree_t *tree, Node_t *node, char *str, const NodeChild child, TreeErrorCode *treeError);

TreeErrorCode TreeDtor(Tree_t *tree);

TreeErrorCode TreeBuild(Tree_t *tree);

TreeErrorCode TreeSaveInFile(Tree_t *tree);

#endif // TREE_H_
