#ifndef DIFF_OPERATIONS_H_
#define DIFF_OPERATIONS_H_

#include "Tree.h"

void DiffConst(Tree_t *tree, Node_t *node);
#define DIFF_CONST_()                                                     \
    DiffConst(tree, node);                                                \
    /*TreeDump(tree);*/                                                   \
    /*system("mimeopen -d /home/kostya/Differentiator/graphviz.png\n");*/ \
    break

void DiffVariable(Tree_t *tree, Node_t *node);
#define DIFF_VARIABLE_()                                                  \
    DiffVariable(tree, node);                                             \
    /*TreeDump(tree);*/                                                   \
    /*system("mimeopen -d /home/kostya/Differentiator/graphviz.png\n");*/ \
    break

Node_t* DiffUnaryOperationSin(Tree_t *tree, Node_t *node);
#define DIFF_SIN_()                                    \
    DiffNode(tree ,DiffUnaryOperationSin(tree, node)); \
    break

Node_t* DiffUnaryOperationCos(Tree_t *tree, Node_t *node);
#define DIFF_COS_()                                    \
    DiffNode(tree ,DiffUnaryOperationCos(tree, node)); \
    break

Node_t* DiffUnaryOperationLn (Tree_t *tree, Node_t *node);
#define DIFF_LN_()                                    \
    DiffNode(tree ,DiffUnaryOperationLn(tree, node)); \
    break

#define DIFF_ADD_OR_SUB_()            \
    DiffNode(tree, node->leftChild ); \
    DiffNode(tree, node->rightChild); \
    break

Node_t *DiffBinaryOperationMul(Tree_t *tree, Node_t *node);
#define DIFF_MUL_()                                                       \
    DiffBinaryOperationMul(tree, node);                                   \
    /*TreeDump(tree);*/                                                   \
    /*system("mimeopen -d /home/kostya/Differentiator/graphviz.png\n");*/ \
    DiffNode(tree, node->leftChild->leftChild  );                         \
    DiffNode(tree, node->rightChild->rightChild);                         \
    break

Node_t *DiffBinaryOperationDiv(Tree_t *tree, Node_t *node);
#define DIFF_DIV_()                                                       \
    DiffBinaryOperationDiv(tree, node);                                   \
    /*TreeDump(tree);*/                                                   \
    /*system("mimeopen -d /home/kostya/Differentiator/graphviz.png\n");*/ \
    DiffNode(tree, node->leftChild->leftChild->leftChild);                \
    DiffNode(tree, node->leftChild->rightChild->rightChild);              \
    break

Node_t *DiffBinaryOperationDegree(Tree_t *tree, Node_t *node);
#define DIFF_DEGREE_()                                                    \
    DiffBinaryOperationDegree(tree, node);                                \
    /*TreeDump(tree);*/                                                   \
    /*system("mimeopen -d /home/kostya/Differentiator/graphviz.png\n");*/ \
    DiffNode(tree, node->rightChild->rightChild);                         \
    break

#endif // DIFF_OPERATIONS_H_
