#include "Tree.h"

int main()
{
    Tree_t tree = {};

    TreeCtor(&tree);
    TreeBuild(&tree);
    TreeSaveInFile(&tree);
    TreeDtor(&tree);

    return 0;
}
