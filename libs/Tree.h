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
    LEFT_CHILD
};

enum TreeStatus
{
    TREE_NOT_CONSTRUCTED,
    TREE_CONSTRUCTED,
    TREE_DESTRUCTED
};

enum TreeErrorCode
{
    TREE_NO_ERROR,
    TREE_CONSTRUCTED_ERROR,
    TREE_INSERT_ERROR,
    TREE_DESTRUCTED_ERROR,
    TREE_FILL_ERROR,
    TREE_OBJECT_DEFINITION_MODE_ERROR
};

//! Number of NodeType elements is number this symbol in ASCII table

enum NodeType
{
    CONST      = 0  ,
    VARIABLE   = 120,
    SIN        = 4  ,
    COS        = 5  ,
    LN         = 6  ,
    ADD        = 43 ,
    SUB        = 45 ,
    MUL        = 42 ,
    DIV        = 47 ,
    POW        = 94
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

void NodeDtor(Node_t *node);

void SubtreeDtor(Node_t *node);

TreeErrorCode TreeDtor(Tree_t *tree);

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType,  const double value);

TreeErrorCode TreeBuild(Tree_t *tree, FILE *finput);

void TreeCopy(Tree_t *tree, Node_t *node1, const Node_t *node2);

TreeErrorCode TreeSaveInFile(Tree_t *tree, FILE* data, const char *str);

int SubtreeCompare(const Node_t *node1, const Node_t *node2);

#endif // TREE_H_
