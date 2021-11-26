#include "Tree.h"
#include "DiffOperations.h"
#include "SimplifyingExpression.h"

const char *OUTPUT_FILE_NAME = "data.tex";

void DiffExpression(Tree_t *tree, Node_t *node);
void Differentiator(Tree_t *tree);

int main()
{
    Tree_t tree = {};

    TreeCtor(&tree);
    TreeBuild(&tree);
    Differentiator(&tree);
    TreeDtor(&tree);

    return 0;
}

void DiffExpression(Tree_t *tree, Node_t *node)
{
    assert(tree != nullptr);
    assert(node != nullptr);

    switch ((int)node->nodeType)
    {
        case CONST:    { DIFF_CONST_()     ; }
        case VARIABLE: { DIFF_VARIABLE_()  ; }
        case SIN:      { DIFF_SIN_()       ; }
        case COS:      { DIFF_COS_()       ; }
        case LN :      { DIFF_LN_()        ; }
        case ADD:
        case SUB:      { DIFF_ADD_OR_SUB_(); }
        case MUL:      { DIFF_MUL_()       ; }
        case DIV:      { DIFF_DIV_()       ; }
        case DEGREE:   { DIFF_DEGREE_()    ; }
        default:
        {
            printf("Error - invalid nodeType!\n");
            break;
        }
    }

    return;
}

void Differentiator(Tree_t *tree)
{
    assert(tree != nullptr);

    FILE *foutput = fopen(OUTPUT_FILE_NAME, "a");

    TreeSaveInFile(tree, foutput, "1. Выражение для дифференцирования:\n\n");

    SimplifyExpression(tree);

    TreeSaveInFile(tree, foutput, "2. Первоначальная обработка выражения:\n\n");

    DiffExpression(tree, tree->root);

    TreeSaveInFile(tree, foutput, "3. Выражение после дифференцирования:\n\n");

    SimplifyExpression(tree);

    TreeSaveInFile(tree, foutput, "4. Упрощенное выражение после дифференцирования:\n\n");

    fprintf(foutput, "\\end{document}");
    fclose(foutput);

    TreeDump(tree);
}
