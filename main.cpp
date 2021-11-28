#include "Tree.h"
#include "DiffOperations.h"

const char *INPUT_FILE_NAME  = "data.txt";
const char *OUTPUT_FILE_NAME = "data.tex";

int main()
{
    Tree_t tree = {};

    TreeCtor(&tree);
    FILE *finput = fopen(INPUT_FILE_NAME, "r");
    TreeBuild(&tree, finput);

    FILE *foutput = fopen(OUTPUT_FILE_NAME, "a");
    Differentiate(&tree, foutput);

    fclose(finput);
    fclose(foutput);
    TreeDtor(&tree);

    return 0;
}
