#ifndef PhiOS_arch_x86_paging_ia32
#define PhiOS_arch_x86_paging_ia32

#include "types.h"
#include "errors.h"
#include "paging.h"

#define PAGING_IA32_PTE_NUMBER   1024
#define PAGING_IA32_PDE_NUMBER   1024

/*
 * structs for page directory with page size 4KB
 */

// maps 4kb of virtual memory
struct IA32_PageTable_4KB_Entry
{
    union
    {
        struct
        {
            uint32 present        :  1;
            uint32 write          :  1;
            uint32 user           :  1;
            uint32 writeThrough   :  1;
            uint32 cacheDisabled  :  1;
            uint32 accessed       :  1;
            uint32 dirty          :  1;
            uint32 pat            :  1;
            uint32 global         :  1;
            uint32 ignored        :  3;
            uint32 address        : 20;
        };
        uint32 data;
    };
} __attribute__((packed));

// maps 4mb of virtual memory
struct IA32_PageTable_4KB
{
    struct IA32_PageTable_4KB_Entry entries[PAGING_IA32_PTE_NUMBER];
} __attribute__((packed));

// maps 4mb of virtual memory
struct IA32_PageDirectory_4KB_Entry
{
    union
    {
        struct
        {
            uint32 present       :  1;
            uint32 write         :  1;
            uint32 user          :  1;
            uint32 writeThrough  :  1;
            uint32 cacheDisabled :  1;
            uint32 accessed      :  1;
            uint32 ignored1      :  1;
            uint32 pageSize      :  1; // If CR4.PSE = 1, must be 0 otherwise, this entry
                                       // maps a 4-MByte page; otherwise, ignored
            uint32 ignored2      :  4;
            uint32 address       : 20;
        };
        uint32 data;
    };
} __attribute__((packed));

// maps 4gb of virtual memory
struct IA32_PageDirectory_4KB
{
    struct IA32_PageDirectory_4KB_Entry entries[PAGING_IA32_PDE_NUMBER];
} __attribute__((packed));

/*
 * public functions for ia32 paging with page size of 4KB
 */

uint32 IA32_4KB_initKernelPaging(
    struct Paging *a_paging
);

uint32 IA32_4KB_init(
    struct Paging *a_paging,
    struct Paging *a_currentPaging
);

uint32 IA32_4KB_alloc(
    struct Paging *a_paging,
    struct AllocFuncParam *a_request,
    size_t *a_address
);

uint32 IA32_4KB_free(
    struct Paging *a_paging,
    struct FreeFuncParam *a_request
);

uint32 IA32_4KB_switchDirectory(
    struct Paging *a_paging,
    struct IA32_PageDirectory_4KB *a_pageDirectory
);

uint32 IA32_4KB_enablePaging(
    struct Paging *a_paging
);

struct IA32_4KB_Paging_AllocParam
{
    uint32          flag;
    bool            user;
    bool            write;
    bool            cacheDisabled;
    bool            writeThrough;
    uint32          virtualAddress;
    uint32          length; // in bytes
    uint32          physicalAddress;
};

struct IA32_4KB_Paging_FreeParam
{
    uint32 flag;
    uint32 startAddress;
    uint32 length; // in bytes
};

/*
 * struct for page directory with page size 4MB
 */

// maps 4MB of virtual memory
struct IA32_PageDirectory_4MB_Entry
{
    union
    {
        struct
        {
            uint32 present       :  1;
            uint32 write         :  1;
            uint32 user          :  1;
            uint32 writeThrough  :  1;
            uint32 cacheDisabled :  1;
            uint32 accessed      :  1;
            uint32 dirty         :  1;
            uint32 pageSize      :  1; // Must be 1 otherwise it references a page table
            uint32 global        :  1;
            uint32 ignored       :  3;
            uint32 pat           :  1;
            uint32 unkown        :  9; // Unkown bits, must read Intel Docs
            uint32 highAddrBits  : 10; // Bits 31:22 of physical address
        };
        uint32 data;
    };
} __attribute__((packed));

struct IA32_PageDirectory_4MB
{
    struct IA32_PageDirectory_4MB_Entry entries[PAGING_IA32_PDE_NUMBER];
} __attribute__((packed));

#endif
