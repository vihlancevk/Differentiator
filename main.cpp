#include "Tree.h"
#include "DiffOperations.h"
#include "SimplifyingExpression.h"

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

void DiffNode(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    switch ((int)node->nodeType)
    {
        case CONST:      { DIFF_CONST_()     ; }
        case VARIABLE_X: { DIFF_VARIABLE_()  ; }
        case VARIABLE_Y: { DIFF_VARIABLE_()  ; }
        case VARIABLE_Z: { DIFF_VARIABLE_()  ; }
        case SIN:        { DIFF_SIN_()       ; }
        case COS:        { DIFF_COS_()       ; }
        case LN :        { DIFF_LN_()        ; }
        case ADD:
        case SUB:        { DIFF_ADD_OR_SUB_(); }
        case MUL:        { DIFF_MUL_()       ; }
        case DIV:        { DIFF_DIV_()       ; }
        case DEGREE:     { DIFF_DEGREE_()    ; }
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
    SimplifyExpression(tree);

    DiffNode(tree, tree->root);

    SimplifyExpression(tree);

    TreeDump(tree);

    assert(tree != nullptr);
}
