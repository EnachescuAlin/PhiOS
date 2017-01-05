#ifndef PhiOS_arch_x86_paging
#define PhiOS_arch_x86_paging

#include "types.h"
#include "errors.h"

#define PAGING_TYPE_IA32_4KB    1
#define PAGING_TYPE_IA32_4MB    2

#define PAGING_TYPE_PAE_2MB     3
#define PAGING_TYPE_PAE_4KB     4

#define PAGING_TYPE_IA32E_4KB   5
#define PAGING_TYPE_IA32E_2MB   6
#define PAGING_TYPE_IA32E_1GB   7

struct Paging
{
    uint8 pagingType;
    void *pagingStruct;
};

#endif
