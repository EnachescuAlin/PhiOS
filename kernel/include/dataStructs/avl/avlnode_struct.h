#ifndef CLIB_TEMPLATE_DATA_STRUCTURES_AVL_NODE_STRUCT
#define CLIB_TEMPLATE_DATA_STRUCTURES_AVL_NODE_STRUCT

#define AVLNodeStruct(name)                                                     \
    C_TEMPLATE_DATA_STRUCTURES_CONCAT(                                          \
        name,                                                                   \
        AVLNode                                                                 \
    )

#define AVLNodeStruct_t(name)                                                   \
    C_TEMPLATE_DATA_STRUCTURES_CONCAT(                                          \
        AVLNodeStruct(name),                                                    \
        _t                                                                      \
    )

#define AVLNodeFunc(name, func)                                                 \
    C_TEMPLATE_DATA_STRUCTURES_CONCAT(                                          \
        AVLNodeStruct(name),                                                    \
        C_TEMPLATE_DATA_STRUCTURES_CONCAT(                                      \
            _,                                                                  \
            func                                                                \
        )                                                                       \
    )

#define DECLARE_AVL_NODE_STRUCT(type, name)                                     \
typedef struct AVLNodeStruct_t(name)                                            \
{                                                                               \
    struct AVLNodeStruct_t(name) *left;                                         \
    struct AVLNodeStruct_t(name) *right;                                        \
    unsigned int height;                                                        \
    type data;                                                                  \
} AVLNodeStruct(name);

#endif // end if CLIB_TEMPLATE_DATA_STRUCTURES_AVL_NODE_STRUCT
