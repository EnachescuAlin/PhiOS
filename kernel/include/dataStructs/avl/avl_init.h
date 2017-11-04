#ifndef CLIB_TEMPLATE_DATA_STRUCTURES_AVL_INIT
#define CLIB_TEMPLATE_DATA_STRUCTURES_AVL_INIT

#define DECLARE_AVL_FUNC_INIT(type, name)                                       \
clib_error_code_t AVLFunc(name, init) (                                         \
    AVLStruct(name) *a_avl                                                      \
);

#define IMPLEMENT_AVL_FUNC_INIT(type, name)                                     \
clib_error_code_t AVLFunc(name, init) (                                         \
    AVLStruct(name) *a_avl)                                                     \
{                                                                               \
    INPUT_CHECK(a_avl == CLIB_NULLPTR, CLIB_ERROR_NULL_POINTER)                 \
                                                                                \
    a_avl->root = CLIB_NULLPTR;                                                 \
                                                                                \
    return CLIB_ERROR_SUCCESS;                                                  \
}

#endif // end if CLIB_TEMPLATE_DATA_STRUCTURES_AVL_INIT
