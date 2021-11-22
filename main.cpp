#include "Tree.h"

const size_t STR_MAX_SIZE = 100;

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

Node_t* DiffNode(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    switch (node->nodeType)
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
        case SIN:
        {
            Node_t *newNode = TreeInsert(tree, node->parent, "*", (node->parent->leftChild == node) ? LEFT_CHILD : RIGHT_CHILD, &treeError);
            memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
            strcpy(node->elem, "cos");
            node->parent = newNode;
            node->nodeType = COS;
            node->value = -1.0;
            newNode = TreeInsert(tree, node->parent, nullptr, LEFT_CHILD, &treeError);
            newNode->leftChild = DiffNode(tree, node->rightChild);
            break;
        }
        case COS:
        {
            memset(node->elem, 0, sizeof(char) * STR_MAX_SIZE);
            strcpy(node->elem, "*");
            TreeInsert(tree, node, "-sin", LEFT_CHILD, &treeError);
            node->leftChild->parent = node;
            node->leftChild->nodeType = SIN;
            node->leftChild->value = -1.0;
            node->rightChild = DiffNode(tree, node->rightChild);
            break;
        }
        default:
        {
            break;
        }
    }

    return node;
}

void DiffExpression(Tree_t *tree)
{
    DiffNode(tree, tree->root);

    TreeDump(tree);

    assert(tree != nullptr);
}
