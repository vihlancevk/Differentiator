#include "DiffOperations.h"

const size_t STR_MAX_SIZE = 100;

void DiffConst(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "0");
    node->nodeType = CONST;
    node->value = 0.0;
}

void DiffVariable(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "1");
    node->nodeType = CONST;
    node->value = 1.0;
}

static void TreeCopy(Tree_t *tree, Node_t *node1, const Node_t *node2)
{
    assert(tree  != nullptr);
    assert(node1 != nullptr);
    assert(node2 != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    strcpy(node1->elem, node2->elem);
    node1->nodeType = node2->nodeType;
    node1->value = node2->value;

    if (node2->leftChild  != nullptr)
    {
        TreeInsert(tree, node1, nullptr, LEFT_CHILD, &treeError);
        TreeCopy(tree, node1->leftChild,  node2->leftChild );
    }
    if (node2->rightChild != nullptr)
    {
        TreeInsert(tree, node1, nullptr, RIGHT_CHILD, &treeError);
        TreeCopy(tree, node1->rightChild, node2->rightChild);
    }
}

static void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    free(node->elem);
    free(node);
}

static Node_t* BuildNewNodeForUnaryOperation(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *newNode = nullptr;

    if (node->parent != nullptr)
    {
        NodeChild child = (node->parent->leftChild == node) ? LEFT_CHILD : RIGHT_CHILD;
        newNode = TreeInsert(tree, node->parent, nullptr, child, &treeError);
    }
    else
    {
        newNode = (Node_t*)calloc(1, sizeof(Node_t));
        if (newNode == nullptr)
        {
            printf("Error in DiffNode in UnaryOperation - invalid pointer on newNode!\n");
            return nullptr;
        }

        newNode->elem = (elem_t*)calloc(STR_MAX_SIZE, sizeof(elem_t));
        if (newNode->elem == nullptr)
        {
            printf("Error in DiffNode in UnaryOperation - invalid pointer on newNode->elem!\n");
            return nullptr;
        }

        tree->root = newNode;
        tree->size = tree->size + 1;
    }

    return newNode;
}

Node_t* DiffUnaryOperationSin(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    Node_t *newNode = BuildNewNodeForUnaryOperation(tree, node);

    strcpy(newNode->elem, "*");
    newNode->leftChild = node;
    TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
    newNode->nodeType = MUL;
    newNode->value = -1.0;

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "cos");
    node->parent = newNode;
    node->nodeType = COS;
    node->value = -1.0;

    TreeCopy(tree, newNode->rightChild, node->leftChild);

    return newNode->rightChild;
}

Node_t* DiffUnaryOperationCos(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *newNode = BuildNewNodeForUnaryOperation(tree, node);

    strcpy(newNode->elem, "*");
    TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
    newNode->nodeType = MUL;
    newNode->value = -1.0;

    Node_t *newNode1 = TreeInsert(tree, newNode, "*", LEFT_CHILD, &treeError);
    newNode1->leftChild = node;
    TreeInsert(tree, newNode1, "-1", RIGHT_CHILD, &treeError);
    newNode1->rightChild->nodeType = CONST;
    newNode1->rightChild->value = 1.0;
    newNode1->nodeType = MUL;
    newNode1->value = -1.0;

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "sin");
    node->parent = newNode1;
    node->nodeType = SIN;
    node->value = -1.0;

    TreeCopy(tree, newNode->rightChild, node->leftChild);

    return newNode->rightChild;
}

Node_t* DiffUnaryOperationLn(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *newNode = BuildNewNodeForUnaryOperation(tree, node);

    strcpy(newNode->elem, "*");
    TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
    newNode->nodeType = MUL;
    newNode->value = -1.0;

    Node_t *newNode1 = TreeInsert(tree, newNode, "/", LEFT_CHILD, &treeError);
    newNode1->rightChild = node->leftChild;
    TreeInsert(tree, newNode1, "1", LEFT_CHILD, &treeError);
    newNode1->leftChild->nodeType = CONST;
    newNode1->leftChild->value = 1.0;
    newNode1->nodeType = DIV;
    newNode1->value = -1.0;

    node->leftChild->parent = newNode1;

    TreeCopy(tree, newNode->rightChild, node->leftChild);

    NodeDtor(node);

    return newNode->rightChild;
}

Node_t *DiffBinaryOperationMul(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *nodeLeftChild  = node->leftChild ;
    Node_t *nodeRightChild = node->rightChild;

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "+");
    node->nodeType = ADD;
    node->value = -1.0;

    Node_t *newNode  = TreeInsert(tree, node, "*", LEFT_CHILD , &treeError);
    Node_t *newNode1 = TreeInsert(tree, node, "*", RIGHT_CHILD, &treeError);

    newNode->nodeType = MUL;
    newNode->value = -1.0;
    newNode1->nodeType = MUL;
    newNode1->value = -1.0;

    TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode->rightChild, nodeRightChild);

    TreeInsert(tree, newNode1, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode1->leftChild, nodeLeftChild);

    newNode->leftChild   = nodeLeftChild ;
    newNode1->rightChild = nodeRightChild;

    nodeLeftChild->parent  = newNode ;
    nodeRightChild->parent = newNode1;

    return node;
}

Node_t *DiffBinaryOperationDiv(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *nodeLeftChild  = node->leftChild ;
    Node_t *nodeRightChild = node->rightChild;

    Node_t *newNode = TreeInsert(tree, node, "-", LEFT_CHILD, &treeError);
    newNode->nodeType = SUB;
    newNode->value = -1.0;

    Node_t *newNode1 = TreeInsert(tree, newNode, "*", LEFT_CHILD, &treeError);
    newNode1->nodeType = MUL;
    newNode1->value = -1.0;
    newNode1->leftChild = nodeLeftChild;
    nodeLeftChild->parent = newNode1;
    TreeInsert(tree, newNode1, nullptr, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode1->rightChild, nodeRightChild);

    Node_t *newNode2 = TreeInsert(tree, newNode, "*", RIGHT_CHILD, &treeError);
    newNode2->nodeType = MUL;
    newNode2->value = -1.0;
    newNode2->rightChild = nodeRightChild;
    nodeRightChild->parent = newNode2;
    TreeInsert(tree, newNode2, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode2->leftChild, nodeLeftChild);

    Node_t *newNode3 = TreeInsert(tree, node, "*", RIGHT_CHILD, &treeError);
    newNode3->nodeType = MUL;
    newNode3->value = -1.0;
    TreeInsert(tree, newNode3, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode3->leftChild, nodeRightChild);
    TreeInsert(tree, newNode3, nullptr, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode3->rightChild, nodeRightChild);

    return node;
}

Node_t *DiffBinaryOperationDegree(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *nodeLeftChild  = node->leftChild ;
    Node_t *nodeRightChild = node->rightChild;

    memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
    strcpy(node->elem, "*");
    node->nodeType = MUL;
    node->value = -1.0;

    TreeInsert(tree, node, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, node->leftChild, nodeRightChild);

    Node_t *newNode = TreeInsert(tree, node, "*", RIGHT_CHILD, &treeError);
    newNode->nodeType = MUL;
    newNode->value = -1.0;

    Node_t *newNode1 = TreeInsert(tree, newNode, "^", LEFT_CHILD, &treeError);
    newNode1->nodeType = DEGREE;
    newNode1->value = -1.0;

    TreeInsert(tree, newNode1, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode1->leftChild, nodeLeftChild);

    Node_t *newNode2 = TreeInsert(tree, newNode1, "-", RIGHT_CHILD, &treeError);
    newNode2->nodeType = SUB;
    newNode2->value = -1.0;

    TreeInsert(tree, newNode2, nullptr, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode2->leftChild, nodeRightChild);

    TreeInsert(tree, newNode2, "1", RIGHT_CHILD, &treeError);
    newNode2->rightChild->nodeType = CONST;
    newNode2->rightChild->value = 1.0;

    TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
    newNode->rightChild = nodeLeftChild;
    nodeLeftChild->parent = newNode;

    return node;
}
