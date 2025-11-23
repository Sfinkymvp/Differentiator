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


typedef struct {
    OpType op;
    const char* name;
    const char* symbol;
} OpInfo;


extern const OpInfo OP_TABLE[];


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
