#include "DiffOperations.h"

const double NODE_NO_VALUE = -1.0;

void DiffConst(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    SetNodeType(node, CONST, 0.0);
}

void DiffVariable(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    SetNodeType(node, CONST, 1.0);
}

static void TreeCopy(Tree_t *tree, Node_t *node1, const Node_t *node2)
{
    assert(tree  != nullptr);
    assert(node1 != nullptr);
    assert(node2 != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    SetNodeType(node1, node2->nodeType, node2->value);

    if (node2->leftChild  != nullptr)
    {
        TreeInsert(tree, node1, LEFT_CHILD, &treeError);
        TreeCopy(tree, node1->leftChild,  node2->leftChild );
    }
    if (node2->rightChild != nullptr)
    {
        TreeInsert(tree, node1, RIGHT_CHILD, &treeError);
        TreeCopy(tree, node1->rightChild, node2->rightChild);
    }
}

static void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    free(node);
}

static NodeChild GetNodeBranch(Node_t *node)
{
    assert(node != nullptr);

    return (node->parent->leftChild == node) ? LEFT_CHILD : RIGHT_CHILD;
}

static Node_t* BuildNewNodeForUnaryOperation(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *newNode = nullptr;

    if (node->parent != nullptr)
    {
        NodeChild child = GetNodeBranch(node);
        newNode = TreeInsert(tree, node->parent, child, &treeError);
    }
    else
    {
        newNode = (Node_t*)calloc(1, sizeof(Node_t));
        if (newNode == nullptr)
        {
            printf("Error in DiffNode in UnaryOperation - invalid pointer on newNode!\n");
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
    SetNodeType(newNode, MUL, NODE_NO_VALUE);

    TreeInsert(tree, newNode, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode->rightChild, node->leftChild);

    newNode->leftChild = node;
    node->parent = newNode;
    SetNodeType(node, COS, NODE_NO_VALUE);

    return newNode->rightChild;
}

Node_t* DiffUnaryOperationCos(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    Node_t *newNode = BuildNewNodeForUnaryOperation(tree, node);
    SetNodeType(newNode, MUL, NODE_NO_VALUE);

    TreeInsert(tree, newNode, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode->rightChild, node->leftChild);

    Node_t *newNode1 = TreeInsert(tree, newNode, LEFT_CHILD, &treeError);
    SetNodeType(newNode1, MUL, NODE_NO_VALUE );

    newNode1->leftChild = node;
    node->parent = newNode1;
    SetNodeType(node, SIN, NODE_NO_VALUE);

    TreeInsert(tree, newNode1, RIGHT_CHILD, &treeError);
    SetNodeType(newNode1->rightChild, CONST, -1.0);

    return newNode->rightChild;
}

Node_t* DiffUnaryOperationLn(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    Node_t *newNode = BuildNewNodeForUnaryOperation(tree, node);
    SetNodeType(newNode, MUL, NODE_NO_VALUE);

    TreeInsert(tree, newNode, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode->rightChild, node->leftChild);

    Node_t *newNode1 = TreeInsert(tree, newNode, LEFT_CHILD, &treeError);
    SetNodeType(newNode1, DIV, NODE_NO_VALUE);

    TreeInsert(tree, newNode1, LEFT_CHILD, &treeError);
    SetNodeType(newNode1->leftChild, CONST, 1.0);

    newNode1->rightChild = node->leftChild;
    node->leftChild->parent = newNode1;

    NodeDtor(node);

    return newNode->rightChild;
}

Node_t *DiffBinaryOperationMul(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *leftChild  = node->leftChild ;
    Node_t *rightChild = node->rightChild;

    #define BUILD_NODE_BRANCH_(branch)                                                                              \
        do                                                                                                          \
        {                                                                                                           \
        Node_t *branch##Node = nullptr;                                                                             \
        if (strcmp("leftChild", #branch) == 0) { branch##Node = TreeInsert(tree, node, LEFT_CHILD, &treeError)  ; } \
        else                                   { branch##Node = TreeInsert(tree, node, RIGHT_CHILD, &treeError) ; } \
        SetNodeType(branch##Node, MUL, NODE_NO_VALUE);                                                              \
        branch##Node->branch = branch;                                                                              \
        branch->parent = branch##Node;                                                                              \
        TreeInsert(tree, branch##Node, (strcmp("leftChild", #branch) == 0) ? RIGHT_CHILD : LEFT_CHILD, &treeError); \
        if (strcmp("leftChild", #branch) == 0) { TreeCopy(tree, branch##Node->rightChild, rightChild) ; }           \
        else                                   { TreeCopy(tree, branch##Node->leftChild, leftChild)   ; }           \
        } while(0)


    SetNodeType(node, ADD, NODE_NO_VALUE);

    BUILD_NODE_BRANCH_(leftChild);

    BUILD_NODE_BRANCH_(rightChild);

    return node;
}

Node_t *DiffBinaryOperationDiv(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;
    Node_t *nodeLeftChild  = node->leftChild ;
    Node_t *nodeRightChild = node->rightChild;

    Node_t *newNode = TreeInsert(tree, node, LEFT_CHILD, &treeError);
    SetNodeType(newNode, SUB, NODE_NO_VALUE);

    Node_t *newNode1 = TreeInsert(tree, newNode, LEFT_CHILD, &treeError);
    SetNodeType(newNode1, MUL, NODE_NO_VALUE);
    newNode1->leftChild = nodeLeftChild;
    nodeLeftChild->parent = newNode1;
    TreeInsert(tree, newNode1, RIGHT_CHILD, &treeError);
    TreeCopy(tree, newNode1->rightChild, nodeRightChild);

    Node_t *newNode2 = TreeInsert(tree, newNode, RIGHT_CHILD, &treeError);
    SetNodeType(newNode2, MUL, NODE_NO_VALUE);
    newNode2->rightChild = nodeRightChild;
    nodeRightChild->parent = newNode2;
    TreeInsert(tree, newNode2, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode2->leftChild, nodeLeftChild);

    Node_t *newNode3 = TreeInsert(tree, node, RIGHT_CHILD, &treeError);
    SetNodeType(newNode3, MUL, NODE_NO_VALUE);
    TreeInsert(tree, newNode3, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode3->leftChild, nodeRightChild);
    TreeInsert(tree, newNode3, RIGHT_CHILD, &treeError);
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

    SetNodeType(node, MUL, NODE_NO_VALUE);

    TreeInsert(tree, node, LEFT_CHILD, &treeError);
    TreeCopy(tree, node->leftChild, nodeRightChild);

    Node_t *newNode = TreeInsert(tree, node, RIGHT_CHILD, &treeError);
    SetNodeType(newNode, MUL, NODE_NO_VALUE);

    Node_t *newNode1 = TreeInsert(tree, newNode, LEFT_CHILD, &treeError);
    SetNodeType(newNode1, DEGREE, NODE_NO_VALUE);

    TreeInsert(tree, newNode1, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode1->leftChild, nodeLeftChild);

    Node_t *newNode2 = TreeInsert(tree, newNode1, RIGHT_CHILD, &treeError);
    SetNodeType(newNode2, SUB, NODE_NO_VALUE);

    TreeInsert(tree, newNode2, LEFT_CHILD, &treeError);
    TreeCopy(tree, newNode2->leftChild, nodeRightChild);

    TreeInsert(tree, newNode2, RIGHT_CHILD, &treeError);
    SetNodeType(newNode2->rightChild, CONST, 1.0);

    TreeInsert(tree, newNode, RIGHT_CHILD, &treeError);
    newNode->rightChild = nodeLeftChild;
    nodeLeftChild->parent = newNode;

    return node;
}
