#ifndef TREE_DEFS_H_
#define TREE_DEFS_H_


const int BUFFER_SIZE = 256;


typedef struct {
    const char* name;
    const char* file;
    const char* function;
    int line;
} CreationInfo;


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
    OP_NONE
} OpType;


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
    const char* identifier;
    CreationInfo origin;
} BinaryTree;


#endif // TREE_DEFS_H_
