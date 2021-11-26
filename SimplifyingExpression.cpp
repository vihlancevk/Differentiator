#include <stdlib.h>
#include "SimplifyingExpression.h"

static void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    free(node);
}

static void PartTreeDtor(Node_t *node)
{
    assert(node != nullptr);

    Node_t *leftChild  = node->leftChild;
    Node_t *rightChild = node->rightChild;

    free(node);

    if (leftChild  != nullptr) NodeDtor(leftChild);
    if (rightChild != nullptr) NodeDtor(rightChild);
}

int status = 0;

#define OPERATION_WITH_NODE_(operation)                                                 \
    SetNodeType(node, CONST, node->leftChild->value operation node->rightChild->value); \
    NodeDtor(node->leftChild );                                                         \
    NodeDtor(node->rightChild);                                                         \
    node->leftChild  = nullptr;                                                         \
    node->rightChild = nullptr;                                                         \
    status = status + 1;                                                                \
    break

static void SimplifyConst(Node_t *node)
{
    assert(node != nullptr);

    switch ((int)node->nodeType)
        {
            case ADD: { OPERATION_WITH_NODE_(+) ; }
            case SUB: { OPERATION_WITH_NODE_(-) ; }
            case MUL: { OPERATION_WITH_NODE_(*) ; }
            case DIV: { OPERATION_WITH_NODE_(/) ; }
            default:  {break                    ; }
        }
}

#undef OPERATION_WITH_NODE_

#define SIMPLIFY_OPERATIONS_(child)                                                     \
    do                                                                                  \
    {                                                                                   \
        node->child->parent = node->parent;                                             \
        if (node->parent != nullptr)                                                    \
        {                                                                               \
            if (node->parent->leftChild == node) node->parent->leftChild = node->child; \
            else node->parent->rightChild = node->child;                                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
           tree->root = node->child;                                                    \
        }                                                                               \
        NodeDtor(node);                                                                 \
        status = status + 1;                                                            \
    } while(0)

static void SimplifyAddAndSub(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    if (node->leftChild->value == 0.0)
    {
        NodeDtor(node->leftChild);
        SIMPLIFY_OPERATIONS_(rightChild);
    }
    else if (node->rightChild->value == 0.0)
    {
        NodeDtor(node->rightChild);
        SIMPLIFY_OPERATIONS_(leftChild);
    }
}

static void SimplifyMul(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    if (node->leftChild->value == 0.0 || node->rightChild->value == 0.0)
    {
        if (node->parent->leftChild == node)
        {
            node->parent->leftChild = nullptr;
            TreeInsert(tree, node->parent, LEFT_CHILD, &treeError);
            SetNodeType(node->parent->leftChild, CONST, 0.0);
        }
        else
        {
            node->parent->rightChild = nullptr;
            TreeInsert(tree, node->parent, RIGHT_CHILD, &treeError);
            SetNodeType(node->parent->rightChild, CONST, 0.0);
        }
        PartTreeDtor(node);
        status = status + 1;
    }

    if (node->leftChild->value == 1.0)
    {
        NodeDtor(node->leftChild);
        SIMPLIFY_OPERATIONS_(rightChild);
    }
    else if (node->rightChild->value == 1.0)
    {
        NodeDtor(node->rightChild);
        SIMPLIFY_OPERATIONS_(leftChild);
    }
}

static void SimplifyDiv(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    if (node->leftChild->value == 0.0)
    {
        if (node->parent->leftChild == node)
        {
            node->parent->leftChild = nullptr;
            TreeInsert(tree, node->parent, LEFT_CHILD, &treeError);
        }
        else
        {
            node->parent->rightChild = nullptr;
            TreeInsert(tree, node->parent, RIGHT_CHILD, &treeError);
        }
        SetNodeType(node->parent->rightChild, CONST, 0.0);
        PartTreeDtor(node);
        status = status + 1;
    }
    else if (node->rightChild->value == 1.0)
    {
        NodeDtor(node->rightChild);
        SIMPLIFY_OPERATIONS_(leftChild);
    }
}

static void SimplifyDegree(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    if (node->rightChild->value == 0.0)
    {
        SetNodeType(node, CONST, 1.0);
        PartTreeDtor(node->leftChild);
        node->leftChild = nullptr;
        NodeDtor(node->rightChild);
        node->rightChild = nullptr;
        status = status + 1;
    }
    else if (node->rightChild->value == 1.0)
    {
        if (node->parent == nullptr)
        {
            node->leftChild->parent = nullptr;
            tree->root = node->leftChild;
            node->leftChild = nullptr;
            PartTreeDtor(node);
        }
        else
        {
            node->leftChild->parent =  node->parent;
            if (node->parent->leftChild == node) { node->parent->leftChild  = node->leftChild; }
            else                                 { node->parent->rightChild = node->leftChild; }
            node->leftChild = nullptr;
            PartTreeDtor(node);
        }
    }
}

#undef SIMPLIFY_OPERATIONS_

static void CheckNode(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    if (node->leftChild  == nullptr) return;
    if (node->rightChild == nullptr) return;

    if (node->leftChild->nodeType == CONST && node->rightChild->nodeType == CONST)
    {
        SimplifyConst(node);
        return;
    }

    switch ((int)node->nodeType)
    {
        case ADD:
        case SUB   : { ; SimplifyAddAndSub(tree, node); break ; }
        case MUL   : { ; SimplifyMul(tree, node)      ; break ; }
        case DIV   : { ; SimplifyDiv(tree, node)      ; break ; }
        case DEGREE: { ; SimplifyDegree(tree, node)   ; break ; }
        default    : { break ; }
    }

    if (node->leftChild  != nullptr) CheckNode(tree, node->leftChild );
    if (node->rightChild != nullptr) CheckNode(tree, node->rightChild);
}

void SimplifyExpression(Tree_t *tree)
{
    assert(tree != nullptr);

    do
    {
        status = 0;
        CheckNode(tree, tree->root);
    } while(status != 0);
}
