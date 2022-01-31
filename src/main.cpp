#include "../include/Tree.h"
#include "../include/DiffOperations.h"

const char *INPUT_FILE_NAME  = "../res/data.txt";
const char *OUTPUT_FILE_NAME = "../res/data.tex";

void CreateTeXFile();

int main()
{
    CreateTeXFile();

    Tree_t tree = {};

    TreeCtor(&tree);
    FILE *finput = fopen(INPUT_FILE_NAME, "r");
    TreeBuild(&tree, finput);
    TreeDump(&tree);

    FILE *foutput = fopen(OUTPUT_FILE_NAME, "a");
    Differentiate(&tree, foutput);

    fclose(finput);
    fclose(foutput);
    TreeDtor(&tree);

    system("pdflatex --output-directory=../res/ ../res/data.tex");

    return 0;
}

void CreateTeXFile()
{
    FILE *foutput = fopen(OUTPUT_FILE_NAME, "w");

    fprintf(foutput, "\\documentclass[a4paper, 12pt]{minimal}\n\n\n");
    fprintf(foutput, "%%  Русификация\n");
    fprintf(foutput, "\\usepackage[utf8]{inputenc}\n");
    fprintf(foutput, "\\usepackage[english, russian]{babel}\n\n\n");
    fprintf(foutput, "%% Греческий\n");
    fprintf(foutput, "\\usepackage{textgreek}\n\n");
    fprintf(foutput, "%% Математическая символика\n");
    fprintf(foutput, "\\usepackage{mathtools}\n\n");
    fprintf(foutput, "\\usepackage{vmargin}\n");
    fprintf(foutput, "\\setpapersize{A4}\n\n");
    fprintf(foutput, "%% Поля страницы\n");
    fprintf(foutput, "\\setmarginsrb{1cm}{1cm}{1cm}{1cm}{0pt}{0mm}{0pt}{0mm}\n\n");
    fprintf(foutput, "\\sloppy\n\n");
    fprintf(foutput, "%% Нумерация страница вкл.\n");
    fprintf(foutput, "\\pagestyle{plain}\n\n");
    fprintf(foutput, "\\begin{document}\n");
    fprintf(foutput, "\t{\n");
    fprintf(foutput, "\t\t\\noindent\n");

    fclose(foutput);
}
