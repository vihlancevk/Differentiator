#include "Tree.h"

const size_t STR_MAX_SIZE = 100;

void DiffNode(Tree_t *tree, Node_t *node);
void DiffExpression(Tree_t *tree);

int main()
{
    Tree_t tree = {};

    TreeCtor(&tree);
    TreeBuild(&tree);
    DiffExpression(&tree);
    TreeDtor(&tree);

    return 0;
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

    return;
}

static void DiffUnaryOperationSin(Tree_t *tree, Node_t *node)
{
    assert(tree          != nullptr);
    assert(node          != nullptr);

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
                printf("Error in DiffNode in SIN - invalid pointer on newNode!\n");
                return;
            }

            newNode->elem = (elem_t*)calloc(STR_MAX_SIZE, sizeof(elem_t));
            if (newNode->elem == nullptr)
            {
                printf("Error in DiffNode in SIN - invalid pointer on newNode->elem!\n");
                return;
            }

            tree->root = newNode;
            tree->size = tree->size + 1;
            newNode->num = tree->size;
        }

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
        DiffNode(tree, newNode->rightChild);

        return;
}

static void DiffUnaryOperationCos(Tree_t *tree, Node_t *node)
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
                printf("Error in DiffNode in COS - invalid pointer on newNode!\n");
                return;
            }

            newNode->elem = (elem_t*)calloc(STR_MAX_SIZE, sizeof(elem_t));
            if (newNode->elem == nullptr)
            {
                printf("Error in DiffNode in COS - invalid pointer on newNode->elem!\n");
                return;
            }

            tree->root = newNode;
            tree->size = tree->size + 1;
            newNode->num = tree->size;
        }

        strcpy(newNode->elem, "*");
        TreeInsert(tree, newNode, nullptr, RIGHT_CHILD, &treeError);
        newNode->nodeType = MUL;
        newNode->value = -1.0;

        Node_t *newNode1 = TreeInsert(tree, newNode, "*", LEFT_CHILD, &treeError);
        newNode1->leftChild = node;
        TreeInsert(tree, newNode1, "-1", RIGHT_CHILD, &treeError);
        newNode1->rightChild->nodeType = CONST;
        newNode1->rightChild->value = -1.0;
        newNode1->nodeType = MUL;
        newNode1->value = -1.0;

        memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
        strcpy(node->elem, "sin");
        node->parent = newNode1;
        node->nodeType = SIN;
        node->value = -1.0;

        TreeCopy(tree, newNode->rightChild, node->leftChild);
        DiffNode(tree, newNode->rightChild);

        return;
}

void DiffNode(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    switch ((int)node->nodeType)
    {
        case CONST :
        {
            memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
            strcpy(node->elem, "0");
            node->nodeType = CONST;
            node->value = 0.0;
            break;
        }
        case VARIABLE:
        {
            memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
            strcpy(node->elem, "1");
            node->nodeType = CONST;
            node->value = 1.0;
            break;
        }
        case SIN: { DiffUnaryOperationSin(tree, node); break; }
        case COS: { DiffUnaryOperationCos(tree, node); break; }
        case LN:
        {

            break;
        }
        case ADD:
        {
            DiffNode(tree, node->leftChild);
            DiffNode(tree, node->rightChild);
            break;
        }
        case SUB:
        {
            DiffNode(tree, node->leftChild);
            DiffNode(tree, node->rightChild);
            break;
        }
        default:
        {
            printf("Error - invalid nodeType!\n");
            break;
        }
    }

    return;
}

void DiffExpression(Tree_t *tree)
{
    DiffNode(tree, tree->root);

    TreeDump(tree);

    assert(tree != nullptr);
}
