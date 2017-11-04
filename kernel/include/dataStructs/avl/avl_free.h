#ifndef CLIB_TEMPLATE_DATA_STRUCTURES_AVL_FREE
#define CLIB_TEMPLATE_DATA_STRUCTURES_AVL_FREE

#ifdef AVL_USE_AVL_NODE_FREE

#define DECLARE_AVL_FUNC_FREE(type, name)                                       \
clib_error_code_t AVLFunc(name, free) (                                         \
    AVLStruct(name) *a_avl                                                      \
);

#define IMPLEMENT_AVL_FUNC_FREE(type, name)                                     \
clib_error_code_t AVLFunc(name, free) (                                         \
    AVLStruct(name) *a_avl)                                                     \
{                                                                               \
    INPUT_CHECK(a_avl == CLIB_NULLPTR, CLIB_ERROR_NULL_POINTER)                 \
                                                                                \
    if (a_avl->root != CLIB_NULLPTR) {                                          \
        AVLNodeFunc(name, free)(a_avl->root);                                   \
        a_avl->root = CLIB_NULLPTR;                                             \
    }                                                                           \
                                                                                \
    return CLIB_ERROR_SUCCESS;                                                  \
}

#else // AVL_USE_AVL_NODE_FREE is not defined

#define DECLARE_AVL_FUNC_FREE(type, name)
#define IMPLEMENT_AVL_FUNC_FREE(type, name)

#endif // end if AVL_USE_AVL_NODE_FREE

#endif // end if CLIB_TEMPLATE_DATA_STRUCTURES_AVL_FREE
