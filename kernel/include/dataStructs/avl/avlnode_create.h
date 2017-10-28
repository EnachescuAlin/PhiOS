#ifndef PhiOS_DATA_STRUCTURES_AVL_NODE_CREATE
#define PhiOS_DATA_STRUCTURES_AVL_NODE_CREATE

#ifdef AVL_USE_AVL_NODE_CREATE

// void* avlAlloc(size_t a_size)
#ifndef AVL_ALLOC_FUNC
#error "alloc function is mandatory"
#endif

// uint32 copyType(type *a_dest, const type *a_src)
#ifndef AVL_COPY_TYPE_FUNC
#error "copy function is mandatory"
#endif

// void avlFree(void *a_ptr)
#ifndef AVL_FREE_FUNC
#error "free function is mandatory"
#endif

#define DECLARE_AVL_NODE_FUNC_CREATE(type, name)                                \
uint32 AVLNodeFunc(name, create) (                                              \
    AVLNodeStruct(name) **a_node,                                               \
    const type *a_data                                                          \
);

#define IMPLEMENT_AVL_NODE_FUNC_CREATE(type, name)                              \
uint32 AVLNodeFunc(name, create) (                                              \
    AVLNodeStruct(name) **a_node,                                               \
    const type *a_data)                                                         \
{                                                                               \
    if (a_node == NULL) {                                                       \
        return ERROR_NULL_POINTER;                                              \
    }                                                                           \
                                                                                \
    if (a_data == NULL) {                                                       \
        return ERROR_NULL_POINTER;                                              \
    }                                                                           \
                                                                                \
    *a_node = (AVLNodeStruct(name)*) AVL_ALLOC_FUNC((sizeof(*a_node)));         \
    if (a_node == NULL) {                                                       \
        return ERROR_NO_FREE_MEMORY;                                            \
    }                                                                           \
                                                                                \
    (*a_node)->left = NULL;                                                     \
    (*a_node)->right = NULL;                                                    \
    (*a_node)->height = 1;                                                      \
                                                                                \
    uint32 err = AVL_COPY_TYPE_FUNC((&(*a_node)->data), a_data);                \
    if (err != ERROR_SUCCESS) {                                                 \
        AVL_FREE_FUNC((*a_node));                                               \
        return err;                                                             \
    }                                                                           \
                                                                                \
    return ERROR_SUCCESS;                                                       \
}

#else // AVL_USE_AVL_NODE_CREATE is not defined

#define DECLARE_AVL_NODE_FUNC_CREATE(type, name)
#define IMPLEMENT_AVL_NODE_FUNC_CREATE(type, name)

#endif // end if AVL_USE_AVL_NODE_CREATE

#endif // end if PhiOS_DATA_STRUCTURES_AVL_NODE_CREATE
