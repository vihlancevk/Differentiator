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

//! Number of NodeType elements is number this symbol in ASCII table

enum NodeType
{
    CONST      = 0  ,
    VARIABLE_X = 120,
    VARIABLE_Y = 121,
    VARIABLE_Z = 122,
    SIN        = 4  ,
    COS        = 5  ,
    LN         = 6  ,
    ADD        = 43 ,
    SUB        = 45 ,
    MUL        = 42 ,
    DIV        = 47 ,
    DEGREE     = 94 ,
};

struct Node_t
{
    Node_t *parent;
    Node_t *leftChild;
    Node_t *rightChild;
    NodeType nodeType;
    double value;
};

struct Tree_t
{
    Node_t *root;
    size_t size;
    TreeStatus status;
};

void TreeDump(Tree_t *tree);

TreeErrorCode TreeCtor(Tree_t *tree);

Node_t* TreeInsert(Tree_t *tree, Node_t *node, const NodeChild child, TreeErrorCode *treeError);

TreeErrorCode TreeDtor(Tree_t *tree);

void SetNodeType(Node_t *node, const NodeType nodeType,  const double value);

TreeErrorCode TreeBuild(Tree_t *tree);

TreeErrorCode TreeSaveInFile(Tree_t *tree);

#endif // TREE_H_
