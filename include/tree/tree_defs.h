#ifndef TREE_DEFS_H_
#define TREE_DEFS_H_


const int BUFFER_SIZE = 256;


extern const char* DUMP_DIRECTORY;
extern const char* DEFAULT_INPUT_FILE;
extern const char* DEFAULT_OUTPUT_FILE;


typedef enum {
    TREE_OK = 0,
    TREE_ROOT_HAS_PARENT,
    TREE_MISSING_PARENT,
    TREE_PARENT_CHILD_MISMATCH,
    TREE_INVALID_BRANCH_STRUCTURE,
    TREE_OUT_OF_MEMORY,
    TREE_INVALID_COUNT,
} TreeStatus;

typedef enum {
    FILE_OK = 0,
    FILE_OPEN_ERROR,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    FILE_CLOSE_ERROR
} FileStatus;


typedef struct {
    TreeStatus status;
    const char* message;
    const char* file;
    const char* function;
    int line;
} DumpInfo;


typedef struct {
    const char* name;
    const char* file;
    const char* function;
    int line;
} CreationInfo;


typedef struct {
    FILE* dump_file;
    char directory[BUFFER_SIZE];
    int image_counter;
} DumpState;


typedef struct {
    CreationInfo creation;
    DumpState dump;
} DebugInfo;


typedef struct {
    const char* input_file;
    const char* output_file;
} CmdArgs;


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
    OP_POW
} OpType;


typedef union {
    OpType op;
    double num_val;
    int var_idx;
} Data;


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
    char* source_code;
    VarTable* variables;
    CmdArgs args; 
    DebugInfo debug;
} BinaryTree;


#endif // TREE_DEFS_H_
