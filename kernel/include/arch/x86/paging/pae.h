#ifndef PhiOs_arch_x86_paging_pae
#define PhiOs_arch_x86_paging_pae

#include "types.h"
#include "errors.h"

#define PAGING_PAE_PDPT_ENTRIES_NUMBER              4
#define PAGING_PAE_PAGE_DIRECTORY_ENTRIES_NUMBER    512
#define PAGING_PAE_PAGE_TABLE_ENTRIES_NUMBER        512

/*
 * structs for page directory with page size 2MB
 */

// maps 2MB
struct PAE_PageDirectory2MB_Entry
{
    uint32 present        :  1;
    uint32 write          :  1;
    uint32 user           :  1;
    uint32 writeThrough   :  1;
    uint32 cacheDisabled  :  1;
    uint32 accessed       :  1;
    uint32 dirty          :  1;
    uint32 pageSize       :  1; // must be 1
    uint32 global         :  1;
    uint32 ignored        :  3;
    uint32 pat            :  1;
    uint32 alwaysZero1    :  8;
    uint32 address        : 31;
    uint32 alwaysZero2    : 11;
    uint32 executeDisable :  1;
} __attribute__((packed));

// maps 1GB
struct PAE_PageDirectory2MB
{
    struct PAE_PageDirectory2MB_Entry entries[PAGING_PAE_PAGE_DIRECTORY_ENTRIES_NUMBER];
} __attribute__((packed));

/*
 * structs for page directory with page size 4kb
 */

// maps 4kb
struct PAE_PageTable4KB_Entry
{
    uint32 present        :  1;
    uint32 write          :  1;
    uint32 user           :  1;
    uint32 writeThrough   :  1;
    uint32 cacheDisabled  :  1;
    uint32 accessed       :  1;
    uint32 dirty          :  1;
    uint32 pat            :  1; // reserved
    uint32 global         :  1;
    uint32 ignored        :  3;
    uint32 address        : 40;
    uint32 alwaysZero     : 11;
    uint32 executeDisable :  1;
} __attribute__((packed));

// maps 2mb
struct PAE_PageTable4KB
{
    struct PAE_PageTable4KB_Entry entries[PAGING_PAE_PAGE_TABLE_ENTRIES_NUMBER];
} __attribute__((packed));

// maps 2mb
struct PAE_PageDirectory4KB_Entry
{
    uint32 present        :  1;
    uint32 write          :  1;
    uint32 user           :  1;
    uint32 writeThrough   :  1;
    uint32 cacheDisabled  :  1;
    uint32 accessed       :  1;
    uint32 ignored1       :  1;
    uint32 pageSize       :  1; // must be 0
    uint32 ignored2       :  4;
    uint32 address        : 40;
    uint32 alwaysZero     : 11;
    uint32 executeDisable :  1;
} __attribute__((packed));

// maps 1GB with pages of 4KB
struct PAE_PageDirectory4KB
{
    struct PAE_PageDirectory4KB_Entry entries[PAGING_PAE_PAGE_DIRECTORY_ENTRIES_NUMBER];
    struct PAE_PageTable4KB *addresses[PAGING_PAE_PAGE_DIRECTORY_ENTRIES_NUMBER];
} __attribute__((packed));

/*
 * structs for PDPT.
 */

// maps 1GB. It's same structure for both page sizes (4KB and 2MB).
struct PAE_PageDirectoryPointerTable_Entry
{
    uint32 present       :  1;
    uint32 alwaysZero1   :  2;
    uint32 writeThrough  :  1;
    uint32 cacheDisabled :  1;
    uint32 alwaysZero2   :  4;
    uint32 ignored       :  3;
    uint32 address       : 40;
    uint32 alwaysZero3   : 10;
} __attribute__((packed));

// maps 4GB with pages of 4KB
struct PAE_PageDirectoryPointerTable4KB
{
    struct PAE_PageDirectoryPointerTable_Entry entries[PAGING_PAE_PDPT_ENTRIES_NUMBER];
    struct PAE_PageDirectory4KB *addresses[PAGING_PAE_PDPT_ENTRIES_NUMBER];
} __attribute__((packed));

// maps 4GB with pages of 2MB
struct PAE_PageDirectoryPointerTable2MB
{
    struct PAE_PageDirectoryPointerTable_Entry entries[PAGING_PAE_PDPT_ENTRIES_NUMBER];
    struct PAE_PageDirectory2MB *addresses[PAGING_PAE_PDPT_ENTRIES_NUMBER];
} __attribute__((packed));

#endif
