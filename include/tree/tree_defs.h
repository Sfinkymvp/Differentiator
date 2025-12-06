#ifndef TREE_DEFS_H_
#define TREE_DEFS_H_


const int BUFFER_SIZE = 256;


typedef enum {
    NODE_OP = 0,
    NODE_VAR,
    NODE_NUM
} NodeType;


typedef enum {
    OP_ADD = 0,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_POW,
    OP_LOG,

    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_COT,

    OP_ASIN,
    OP_ACOS,
    OP_ATAN,
    OP_ACOT,

    OP_SINH,
    OP_COSH,
    OP_TANH,
    OP_COTH,

    OP_ASINH,
    OP_ACOSH,
    OP_ATANH,
    OP_ACOTH,

    OP_NONE
} OpType;


const size_t OP_MAX_COUNT = OP_NONE + 1;


typedef struct {
    OpType op;
    const char* name;
    const char* symbol;
} OpInfo;


#define OP_INFO_ITEM(ENUM, SYMBOL) {ENUM, #ENUM, SYMBOL}


const OpInfo OP_TABLE[] = {
    OP_INFO_ITEM(OP_ADD, "+"),
    OP_INFO_ITEM(OP_SUB, "-"),
    OP_INFO_ITEM(OP_MUL, "*"),
    OP_INFO_ITEM(OP_DIV, "/"),

    OP_INFO_ITEM(OP_POW, "^"),
    OP_INFO_ITEM(OP_LOG, "log"),

    OP_INFO_ITEM(OP_SIN, "sin"),
    OP_INFO_ITEM(OP_COS, "cos"),
    OP_INFO_ITEM(OP_TAN, "tan"),
    OP_INFO_ITEM(OP_COT, "cot"),

    OP_INFO_ITEM(OP_ASIN, "asin"),
    OP_INFO_ITEM(OP_ACOS, "acos"),
    OP_INFO_ITEM(OP_ATAN, "atan"),
    OP_INFO_ITEM(OP_ACOT, "acot"),

    OP_INFO_ITEM(OP_SINH, "sinh"),
    OP_INFO_ITEM(OP_COSH, "cosh"),
    OP_INFO_ITEM(OP_TANH, "tanh"),
    OP_INFO_ITEM(OP_COTH, "coth"),

    OP_INFO_ITEM(OP_ASINH, "asinh"),
    OP_INFO_ITEM(OP_ACOSH, "acosh"),
    OP_INFO_ITEM(OP_ATANH, "atanh"),
    OP_INFO_ITEM(OP_ACOTH, "acoth"),

    OP_INFO_ITEM(OP_NONE, "")
};
const size_t OP_TABLE_COUNT = sizeof(OP_TABLE) / sizeof(*OP_TABLE);


typedef struct {
    const char* name;
    const char* file;
    const char* function;
    int line;
} CreationInfo;


typedef union {
    OpType op;
    double num_val;
    size_t var_idx;
} NodeValue;



typedef struct TreeNode TreeNode;
struct TreeNode {
    NodeType type;
    NodeValue value;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
};


typedef struct {
    TreeNode* root;
    char* identifier;
    CreationInfo origin;
} BinaryTree;


#endif // TREE_DEFS_H_
