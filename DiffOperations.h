#ifndef DIFF_OPERATIONS_H_
#define DIFF_OPERATIONS_H_

#include "Tree.h"

void DiffConst(Tree_t *tree, Node_t *node);
#define DIFF_CONST_()      \
    DiffConst(tree, node); \
    break

void DiffVariable(Tree_t *tree, Node_t *node);
#define DIFF_VARIABLE_()      \
    DiffVariable(tree, node); \
    break

Node_t* DiffUnaryOperationSin(Tree_t *tree, Node_t *node);
#define DIFF_SIN_()                                          \
    DiffExpression(tree ,DiffUnaryOperationSin(tree, node)); \
    break

Node_t* DiffUnaryOperationCos(Tree_t *tree, Node_t *node);
#define DIFF_COS_()                                          \
    DiffExpression(tree ,DiffUnaryOperationCos(tree, node)); \
    break

Node_t* DiffUnaryOperationLn (Tree_t *tree, Node_t *node);
#define DIFF_LN_()                                          \
    DiffExpression(tree ,DiffUnaryOperationLn(tree, node)); \
    break

#define DIFF_ADD_OR_SUB_()                  \
    DiffExpression(tree, node->leftChild ); \
    DiffExpression(tree, node->rightChild); \
    break

Node_t *DiffBinaryOperationMul(Tree_t *tree, Node_t *node);
#define DIFF_MUL_()                                     \
    DiffBinaryOperationMul(tree, node);                 \
    DiffExpression(tree, node->leftChild->leftChild  ); \
    DiffExpression(tree, node->rightChild->rightChild); \
    break

Node_t *DiffBinaryOperationDiv(Tree_t *tree, Node_t *node);
#define DIFF_DIV_()                                                \
    DiffBinaryOperationDiv(tree, node);                            \
    DiffExpression(tree, node->leftChild->leftChild->leftChild);   \
    DiffExpression(tree, node->leftChild->rightChild->rightChild); \
    break

Node_t *DiffBinaryOperationDegree(Tree_t *tree, Node_t *node);
#define DIFF_DEGREE_()                                  \
    DiffBinaryOperationDegree(tree, node);              \
    DiffExpression(tree, node->rightChild->rightChild); \
    break

#endif // DIFF_OPERATIONS_H_
