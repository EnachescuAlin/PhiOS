#ifndef PhiOS_PhysicalMemoryManager
#define PhiOS_PhysicalMemoryManager

#include "include/types.h"
#include "include/errors.h"

#define PMM_FOR_VIRTUAL_MEMORY  1
#define PMM_FOR_DMA             2

typedef uint32 (*PMA_ALLOC_PFN)(
    void *a_pma,
    uint64 a_size,
    uint64 *a_physicalAddress
);

typedef uint32 (*PMA_FREE_PFN)(
    void *a_pma,
    uint64 a_size,
    uint64 a_physicalAddress
);

typedef uint32 (*PMA_RESERVE_PFN)(
    void *a_pma,
    uint64 a_size,
    uint64 physicalAddress
);

typedef uint32 (*PMA_CHECK_PFN)(
    void *a_pma,
    uint64 a_startAddr,
    uint64 a_endAddr,
    uint8 *a_state
);

struct PMA
{
    uint8           type;
    uint8           locked;
    void           *PMAStruct;
    PMA_ALLOC_PFN   allocFn;
    PMA_FREE_PFN    freeFn;
    PMA_RESERVE_PFN reserveFn;
    PMA_CHECK_PFN   checkFn;
};

uint32 PMM_init(
    uint8 a_allocatorsNumber
);

uint32 PMM_addAllocator(
    void *a_allocator,
    uint8 a_flag,
    PMA_ALLOC_PFN a_allocFn,
    PMA_FREE_PFN a_freeFn,
    PMA_RESERVE_PFN a_reserveFn,
    PMA_CHECK_PFN a_checkFn
);

uint32 PMM_alloc(
    uint64 *a_address,
    uint64 a_size,
    uint8 a_flag
);

uint32 PMM_free(
    uint64 a_address,
    uint64 a_size,
    uint8 a_flag
);

uint32 PMM_reserve(
    uint64 a_address,
    uint64 a_size,
    uint8 a_flag
);

uint32 PMM_check(
    uint64 a_startAddr,
    uint64 a_endAddr,
    uint8 *a_state,
    uint8 a_flag
);

uint32 PMM_adjustPointers(
    uint32 a_offset
);

#endif
