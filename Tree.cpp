#include "Tree.h"
#include "FileOperations.h"

#define DEBUG

const size_t STR_MAX_SIZE = 100;
const char *TREE_GRAPH_VIZ   = "graphviz.gv";
const char *INPUT_FILE_NAME  = "data.txt";
static const char *OUTPUT_FILE_NAME = "data.tex";

struct NodeView
{
    char shape[20];
    char color[20];
    char str[5];
};

static void NodeViewBuild(const Node_t *node, NodeView *nodeView)
{
    assert(node     != nullptr);
    assert(nodeView != nullptr);

    #define SET_NODE_VIEW_(thisShape, thisColor, thisStr) \
        strcpy(nodeView->shape , #thisShape);             \
        strcpy(nodeView->color , #thisColor );            \
        strcpy(nodeView->str   , #thisStr   );            \
        break

    switch ((int)node->nodeType)
    {
        case ADD:        { SET_NODE_VIEW_(octagon, yellow, +)  ; }
        case SUB:        { SET_NODE_VIEW_(octagon, yellow, -)  ; }
        case MUL:        { SET_NODE_VIEW_(octagon, yellow, *)  ; }
        case DIV:        { SET_NODE_VIEW_(octagon, yellow, /)  ; }
        case DEGREE:     { SET_NODE_VIEW_(octagon, yellow, ^)  ; }
        case SIN:        { SET_NODE_VIEW_(circle, yellow, sin) ; }
        case COS:        { SET_NODE_VIEW_(circle, yellow, cos) ; }
        case LN:         { SET_NODE_VIEW_(circle, yellow, ln)  ; }
        case VARIABLE_X: { SET_NODE_VIEW_(rectangle, red, x)   ; }
        case VARIABLE_Y: { SET_NODE_VIEW_(rectangle, red, y)   ; }
        case VARIABLE_Z: { SET_NODE_VIEW_(rectangle, red, z)   ; }
        case CONST:
        {
            sprintf(nodeView->str, "%g", node->value);
            strcpy(nodeView->shape, "rectangle");
            strcpy(nodeView->color, "green");
            break;
        }
        default:
        {
            printf("Error in NodeViewBuild!\n");
            break;
        }
    }

    #undef SET_NODE_VIEW_

}

static void TreeVisitPrintNodeInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    NodeView nodeView = {};

    NodeViewBuild(node, &nodeView);

    char str[STR_MAX_SIZE] = {};
    sprintf(str, "\t%lu[shape=record, shape=%s, style=\"filled\", fillcolor=%s, label=\"%s\"];\n", node, nodeView.shape, nodeView.color, nodeView.str);
    fprintf(foutput, "%s", str);

    if (node->leftChild  != nullptr) TreeVisitPrintNodeInFile(node->leftChild, foutput);
    if (node->rightChild != nullptr) TreeVisitPrintNodeInFile(node->rightChild, foutput);
}

static void TreeVisitPrintArrowInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    if (node->parent != nullptr) fprintf(foutput, "\t%lu -> %lu[color=red, fontsize=12]\n", node, node->parent);

    if (node->leftChild  != nullptr) fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", node, node->leftChild);

    if (node->rightChild != nullptr) fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", node, node->rightChild);

    if (node->leftChild  != nullptr) TreeVisitPrintArrowInFile(node->leftChild, foutput);
    if (node->rightChild != nullptr) TreeVisitPrintArrowInFile(node->rightChild, foutput);
}

void TreeDump(Tree_t *tree)
{
    FILE *graphViz = fopen(TREE_GRAPH_VIZ, "w");

    fprintf(graphViz, "digraph Tree{\n\n");
    fprintf(graphViz, "\trankdir=UD;\n\n");
    fprintf(graphViz, "\tnode[fontsize=14];\n\n");

    TreeVisitPrintNodeInFile(tree->root, graphViz);

    fprintf(graphViz, "\n");

    TreeVisitPrintArrowInFile(tree->root, graphViz);

    fprintf(graphViz, "\n");

    fprintf(graphViz, "}");

    fclose(graphViz);

    system("dot -Tpng graphviz.gv -o graphviz.png");
}

TreeErrorCode TreeCtor(Tree_t *tree)
{
    assert(tree != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    if (tree->status != TREE_NOT_CONSTRUCTED)
    {
        treeError = TREE_CONSTRUCTED_ERROR;
    }

    tree->root = (Node_t*)calloc(1, sizeof(Node_t));
    if (tree->root == nullptr)
    {
        treeError = TREE_CONSTRUCTED_ERROR;
    }

    tree->size = 0;
    tree->status = TREE_CONSTRUCTED;

    return treeError;
}

static void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    Node_t *leftChild  = node->leftChild;
    Node_t *rightChild = node->rightChild;

    free(node);

    if (leftChild  != nullptr) NodeDtor(leftChild);
    if (rightChild != nullptr) NodeDtor(rightChild);
}

TreeErrorCode TreeDtor(Tree_t *tree)
{
    assert(tree != nullptr);

    if (tree->status == TREE_DESTRUCTED)
    {
        return TREE_DESTRUCTED_ERROR;
    }

    NodeDtor(tree->root);
    tree->size = 0;
    tree->size = TREE_DESTRUCTED;

    return TREE_NO_ERROR;
}

Node_t* TreeInsert(Tree_t *tree, Node_t *node, const NodeChild child, TreeErrorCode *treeError)
{
    assert(tree      != nullptr);
    assert(node      != nullptr);
    assert(treeError != nullptr);

    *treeError = TREE_NO_ERROR;
    Node_t *newNode = nullptr;
    #define TREE_INSERT_(branch)                                            \
        do                                                                  \
        {                                                                   \
        node->branch = (Node_t*)calloc(1, sizeof(Node_t));                  \
        if (node->branch == nullptr)                                        \
        {                                                                   \
            *treeError = TREE_INSERT_ERROR;                                 \
        }                                                                   \
        node->branch->parent = node;                                        \
        newNode = node->branch;                                             \
        } while(0)

    if (child == LEFT_CHILD)
    {
        TREE_INSERT_(leftChild);
    }
    else
    {
        TREE_INSERT_(rightChild);
    }

    #undef TREE_INSERT_

    if (tree->size == 0)
    {
        Node_t *newBeginNode = tree->root->leftChild;
        free(tree->root);
        tree->root = newBeginNode;
        tree->root->parent = nullptr;
    }
    tree->size   = tree->size + 1;

    return newNode;
}

static char* TreeReadData()
{
    FILE *finput = fopen(INPUT_FILE_NAME, "r");

    int numberBytesFile = GetFileSize(finput);
    char *str = (char*)calloc(numberBytesFile, sizeof(char));
    str = (char*)ReadFile(finput, str, numberBytesFile);

    fclose(finput);

    return str;
}

static char* StrBufferFindEndStr(char *str)
{
    assert(str != nullptr);

    return strchr(str, ')');
}

static NodeType DefineNodeType(const char *str)
{
    assert(str != nullptr);

    #define STRCOMPARE_(thisStr) \
        strcmp(str, thisStr) == 0

    if (STRCOMPARE_("x"))
    {
        return VARIABLE_X;
    }
    else if (STRCOMPARE_("y"))
    {
        return VARIABLE_Y;
    }
    else if (STRCOMPARE_("z"))
    {
        return VARIABLE_Z;
    }
    else if (STRCOMPARE_("sin"))
    {
        return SIN;
    }
    else if (STRCOMPARE_("cos"))
    {
        return COS;
    }
    else if (STRCOMPARE_("ln"))
    {
        return LN;
    }
    else if (STRCOMPARE_("+"))
    {
        return ADD;
    }
    else if (STRCOMPARE_("-"))
    {
        return SUB;
    }
    else if (STRCOMPARE_("*"))
    {
        return MUL;
    }
    else if (STRCOMPARE_("/"))
    {
        return DIV;
    }
    else if (STRCOMPARE_("^"))
    {
        return DEGREE;
    }
    else
    {
        return CONST;
    }

    #undef STRCOMPARE_
}

void SetNodeType(Node_t *node, const NodeType nodeType, const double value)
{
    assert(node != nullptr);

    node->nodeType = nodeType;
    node->value    = value;
}

static char* NodeBuild(Tree_t *tree, Node_t *node, char *str, TreeErrorCode *treeError, const NodeChild child)
{
    assert(tree      != nullptr);
    assert(node      != nullptr);
    assert(str       != nullptr);
    assert(treeError != nullptr);

    char mathOperation[3] = {};

    if (*str == '(' && *(str + 1) == '(')
    {
        Node_t *newNode = TreeInsert(tree, node, child, treeError);

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);

        strncpy(mathOperation, str, 1);
        newNode->nodeType = DefineNodeType(mathOperation);
        newNode->value = -1.0;

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, RIGHT_CHILD);
    }
    else
    {
        str = str + 1;
        Node_t *newNode = TreeInsert(tree, node, child, treeError);
        if (strncmp(str, "sin", 3) == 0 || strncmp(str, "cos", 3) == 0)
        {
            strncpy(mathOperation, str, 3);
            str = str + 3;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else if (strncmp(str, "ln", 2) == 0)
        {
            strncpy(mathOperation, str, 2);
            str = str + 2;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else
        {
            char *endStr = StrBufferFindEndStr(str);
            *endStr = '\0';
            strcpy(mathOperation, str);
            str = endStr + 1;
        }

        newNode->nodeType = DefineNodeType(mathOperation);
        if (newNode->nodeType == CONST)
        {
            newNode->value = atof(mathOperation);
        }
        else
        {
            newNode->value = -1.0;
        }

        while (*str == ')')
        {
            str = str + 1;
        }
    }

    return str;
}

TreeErrorCode TreeBuild(Tree_t *tree)
{
    assert(tree != nullptr);

    char *str = TreeReadData();
    if (str == nullptr)
    {
        return TREE_FILL_ERROR;
    }

    char *strCopy = str;
    TreeErrorCode treeError = TREE_NO_ERROR;

    NodeBuild(tree, tree->root, strCopy, &treeError, LEFT_CHILD);

    free(str);
    return treeError;
}

static void NodeSaveInFile(Node_t *node, FILE *foutput, NodeChild child)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    if (node->nodeType == CONST)
    {
        fprintf(foutput, "%g", node->value);
    }
    else if (node->nodeType == VARIABLE_X || node->nodeType == VARIABLE_Y || node->nodeType == VARIABLE_Z)
    {
        fprintf(foutput, "%c", node->nodeType);
    }
    else if (node->nodeType == SIN)
    {
        fprintf(foutput, "\\sin");
        fprintf(foutput, "{");
        NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
        fprintf(foutput, "}");
    }
    else if (node->nodeType == COS)
    {
        fprintf(foutput, "\\cos");
        fprintf(foutput, "{");
        NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
        fprintf(foutput, "}");
    }
    else if (node->nodeType == LN)
    {
        fprintf(foutput, "\\ln");
        fprintf(foutput, "{");
        NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
        fprintf(foutput, "}");
    }
    else
    {
        if (node->nodeType == MUL)
        {
            fprintf(foutput, "{");
            NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
            fprintf(foutput, "}");

            fprintf(foutput, "\\cdot");

            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput, RIGHT_CHILD);
            fprintf(foutput, "}");
        }
        else if (node->nodeType == DIV)
        {
            fprintf(foutput, "\\frac");

            fprintf(foutput, "{");
            NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
            fprintf(foutput, "}");

            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput, RIGHT_CHILD);
            fprintf(foutput, "}");
        }
        else if (node->nodeType == DEGREE)
        {
            NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
            fprintf(foutput, "%c", node->nodeType);
            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput, RIGHT_CHILD);
            fprintf(foutput, "}");
        }
        else
        {
            NodeSaveInFile(node->leftChild, foutput, LEFT_CHILD);
            fprintf(foutput, "%c", node->nodeType);
            NodeSaveInFile(node->rightChild, foutput, RIGHT_CHILD);
        }
    }
}

TreeErrorCode TreeSaveInFile(Tree_t *tree, FILE* data)
{
    assert(tree != nullptr);
    assert(data != nullptr);

    NodeSaveInFile(tree->root, data, LEFT_CHILD);

    return TREE_NO_ERROR;
}
