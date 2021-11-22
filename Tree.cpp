#include "Tree.h"
#include "FileOperations.h"

#define DEBUG

const size_t STR_MAX_SIZE = 100;
const char *TREE_GRAPH_VIZ   = "/home/kostya/Differentiator/graphviz.gv";
const char *INPUT_FILE_NAME  = "/home/kostya/Differentiator/data.txt";
const char *OUTPUT_FILE_NAME = "/home/kostya/Differentiator/data.txt";

struct NodeType
{
    char shape[20];
    char color[20];
};

static void NodeTypeBuild(const Node_t *node, NodeType *nodeType)
{
    assert(node     != nullptr);
    assert(nodeType != nullptr);

    switch ((int)node->elemType)
    {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case DEGREE:
        {
            strcpy(nodeType->shape, "octagon");
            strcpy(nodeType->color, "yellow");
            break;
        }
        case SIN:
        case COS:
        case LN:
        {
            strcpy(nodeType->shape, "circle");
            strcpy(nodeType->color, "yellow");
            break;
        }
        case VARIABLE:
        {
            strcpy(nodeType->shape, "rectangle");
            strcpy(nodeType->color, "red");
            break;
        }
        default:
        {
            strcpy(nodeType->shape, "rectangle");
            strcpy(nodeType->color, "green");
            break;
        }
    }
}

static void TreeVisitPrintNodeInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    NodeType nodeType = {};

    NodeTypeBuild(node, &nodeType);

    char str[STR_MAX_SIZE] = {};
    sprintf(str, "\t%lu[shape=record, shape=%s, style=\"filled\", fillcolor=%s, label=\"%s\"];\n", node->num, nodeType.shape, nodeType.color, node->elem);
    fprintf(foutput, "%s", str);

    if (node->left  != nullptr) TreeVisitPrintNodeInFile(node->left, foutput);
    if (node->right != nullptr) TreeVisitPrintNodeInFile(node->right, foutput);
}

static void TreeVisitPrintArrowInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    if (node->left  != nullptr) fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", node->num, node->left->num);

    if (node->right != nullptr) fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", node->num, node->right->num);

    if (node->left  != nullptr) TreeVisitPrintArrowInFile(node->left, foutput);
    if (node->right != nullptr) TreeVisitPrintArrowInFile(node->right, foutput);
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

    system("dot -Tpng /home/kostya/Differentiator/graphviz.gv -o /home/kostya/Differentiator/graphviz.png");
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
    tree->root->elem = (char*)calloc(STR_MAX_SIZE, sizeof(char));
    if (tree->root->elem == nullptr)
    {
        treeError = TREE_CONSTRUCTED_ERROR;
    }
    strcpy(tree->root->elem, "Begin");
    tree->root->num = 1;

    tree->size = 0;
    tree->status = TREE_CONSTRUCTED;

    return treeError;
}

static void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    Node_t *left  = node->left;
    Node_t *right = node->right;
    free(node->elem);
    free(node);

    if (left  != nullptr) NodeDtor(left);
    if (right != nullptr) NodeDtor(right);
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

Node_t* TreeInsert(Tree_t *tree, Node_t *node, char *str, const NodeChild child, TreeErrorCode *treeError)
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
        node->branch->elem = (elem_t*)calloc(STR_MAX_SIZE, sizeof(elem_t)); \
        if (node->branch->elem == nullptr)                                  \
        {                                                                   \
            *treeError = TREE_INSERT_ERROR;                                 \
        }                                                                   \
        if (str != nullptr)                                                 \
        {                                                                   \
            strcpy(node->branch->elem, str);                                \
        }                                                                   \
        newNode = node->branch;                                             \
        } while(0)

    if (child == LEFT_CHILD)
    {
        TREE_INSERT_(left);
    }
    else
    {
        TREE_INSERT_(right);
    }

    #undef TREE_INSERT_

    if (tree->size == 0)
    {
        Node_t *newBeginNode = tree->root->left;
        free(tree->root->elem);
        free(tree->root);
        tree->root = newBeginNode;
    }
    tree->size   = tree->size + 1;
    newNode->num = tree->size + 1;

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

static NodeElemType DefineNodeElemType(const Node_t *node)
{
    assert(node != nullptr);

    #define STRCOMPARE_(str) \
        strcmp(node->elem, str) == 0

    if (STRCOMPARE_("x") ||  STRCOMPARE_("y") || STRCOMPARE_("z"))
    {
        return VARIABLE;
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

static char* NodeBuild(Tree_t *tree, Node_t *node, char *str, TreeErrorCode *treeError, const NodeChild child)
{
    assert(tree      != nullptr);
    assert(node      != nullptr);
    assert(str       != nullptr);
    assert(treeError != nullptr);

    char mathOperation[3] = {};

    if (*str == '(' && *(str + 1) == '(')
    {
        Node_t *newNode = TreeInsert(tree, node, nullptr, child, treeError);

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);

        strncpy(newNode->elem, str, 1);
        newNode->elemType = DefineNodeElemType(newNode);

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, RIGHT_CHILD);
    }
    else
    {
        str = str + 1;
        Node_t *newNode = TreeInsert(tree, node, nullptr, child, treeError);
        if (strncmp(str, "sin", 3) == 0 || strncmp(str, "cos", 3) == 0)
        {
            strncpy(mathOperation, str, 3);
            strcpy(newNode->elem, mathOperation);
            str = str + 3;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else if (strncmp(str, "ln", 2) == 0)
        {
            strncpy(mathOperation, str, 2);
            strcpy(newNode->elem, mathOperation);
            str = str + 2;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else
        {
            char *endStr = StrBufferFindEndStr(str);
            *endStr = '\0';
            strcpy(newNode->elem, str);
            str = endStr + 1;
        }

        newNode->elemType = DefineNodeElemType(newNode);

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

    TreeDump(tree);

    free(str);
    return treeError;
}

static void NodeSaveInFile(Node_t *node, FILE *foutput, NodeChild child)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    fprintf(foutput, "(");

    if (node->elemType == CONST || node->elemType == VARIABLE)
    {
        fprintf(foutput, "%s", node->elem);
    }
    else if (node->elemType == SIN || node->elemType == COS || node->elemType == LN)
    {
        fprintf(foutput, "%s", node->elem);
        NodeSaveInFile(node->left, foutput, LEFT_CHILD);
    }
    else
    {
        NodeSaveInFile(node->left, foutput, LEFT_CHILD);
        fprintf(foutput, "%s", node->elem);
        NodeSaveInFile(node->right, foutput, RIGHT_CHILD);
    }

    fprintf(foutput, ")");
}

TreeErrorCode TreeSaveInFile(Tree_t *tree)
{
    assert(tree != nullptr);

    FILE *data = fopen(OUTPUT_FILE_NAME, "w");

    NodeSaveInFile(tree->root, data, LEFT_CHILD);

    TreeDump(tree);

    fclose(data);
    return TREE_NO_ERROR;
}
