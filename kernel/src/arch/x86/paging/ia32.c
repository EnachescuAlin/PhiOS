#include "arch/x86/paging/ia32.h"
#include "memory/pmm.h"

extern struct KernelArea g_kernelArea;

static size_t helper_IA32_4KB_createPaging(struct Paging *a_paging)
{
    size_t error = ERROR_SUCCESS;
    size_t pdAddr = 0;
    struct IA32_PageDirectory_4KB *pd = NULL;

    do
    {
        error = PMM_alloc(&pdAddr, 8192, PMM_FOR_VIRTUAL_MEMORY);
        if (error != ERROR_SUCCESS)
        {
            break;
        }

        pd = (struct IA32_PageDirectory_4KB*) pdAddr;
        for (uint32 i = 0; i < PAGING_IA32_PDE_NUMBER; i++)
        {
            struct IA32_PageDirectory_4KB_Entry *pdEntry = pd->entries + i;
            size_t *entryAddr = (size_t*) pdEntry;
            *entryAddr = 0;
            pd->addresses[i] = NULL;
        }

        a_paging->pagingType                = PAGING_TYPE_IA32_4KB;
        a_paging->locked                    = 0;
        a_paging->pagingStruct              = (void*) pd;
        a_paging->allocFn                   = IA32_4KB_alloc;
        a_paging->freeFn                    = IA32_4KB_free;
        a_paging->freeMappedVirtualMemory   = 0;
        a_paging->freeVirtualMemory         = 0;
        a_paging->lastAllocatedPage         = 1;

    } while (false);

    return error;
}

static size_t helper_IA32_4KB_deletePaging(struct Paging *a_paging)
{
    size_t error = ERROR_SUCCESS;
    size_t pdAddr = 0;

    do
    {
        pdAddr = (size_t) a_paging->pagingStruct;
        error = PMM_free(pdAddr, 8192, PMM_FOR_VIRTUAL_MEMORY);
        if (error != ERROR_SUCCESS)
        {
            break;
        }

        a_paging->pagingType                = PAGING_TYPE_NONE;
        a_paging->locked                    = 0;
        a_paging->pagingStruct              = NULL;
        a_paging->allocFn                   = NULL;
        a_paging->freeFn                    = NULL;
        a_paging->freeMappedVirtualMemory   = 0;
        a_paging->freeVirtualMemory         = 0;
        a_paging->lastAllocatedPage         = 0;

    } while (false);

    return error;
}

static void helpet_IA32_4KB_allocArea(struct Paging *a_paging,
                                      struct IA32_4KB_Paging_AllocParam *a_request)
{
    struct IA32_PageDirectory_4KB *pd = a_paging->pagingStruct;
    size_t virtualAddress = a_request->virtualAddress & (~4095);

    size_t firstPageId = virtualAddress / 4096;
    size_t pageTableId = firstPageId / PAGING_IA32_PDE_NUMBER;
    size_t pageId = firstPageId % PAGING_IA32_PTE_NUMBER;

    size_t lastVirtualAddress = a_request->virtualAddress + a_request->length;
    if (lastVirtualAddress % 4096 != 0)
    {
        lastVirtualAddress &= (~4095);
        lastVirtualAddress += 4096;
    }

    size_t pagesNumber = (lastVirtualAddress - virtualAddress) / 4096;
    struct IA32_PageTable_4KB *pageTable = pd->addresses[pageTableId];
    size_t physicalAddress = a_request->physicalAddress;

#define PAGE pageTable->entries[pageId]

    while (pagesNumber != 0)
    {
        PAGE.present        = 1;
        PAGE.write          = a_request->write;
        PAGE.user           = a_request->user;
        PAGE.writeThrough   = a_request->writeThrough;
        PAGE.cacheDisabled  = a_request->cacheDisabled;
        PAGE.accessed       = 0;
        PAGE.dirty          = 0;
        PAGE.pat            = 0;
        PAGE.global         = 0;
        PAGE.ignored        = 0;
        PAGE.address        = (physicalAddress >> 12);

        pageId++;
        if (pageId == PAGING_IA32_PTE_NUMBER)
        {
            pageTableId++;
            pageId = 0;
            pageTable = pd->addresses[pageTableId];
        }

        pagesNumber--;
    }

#undef PAGE
}

size_t IA32_4KB_init(struct Paging *a_paging, struct Paging *a_currentPaging)
{
    if (a_paging == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    size_t error = ERROR_SUCCESS;

    error = helper_IA32_4KB_createPaging(a_paging);
    if (error != ERROR_SUCCESS)
    {
        return error;
    }

    struct IA32_4KB_Paging_AllocParam allocParam;
    struct AllocFuncParam request;
    size_t returnedAddress  = 0;
    size_t requestedAddress = 3221225472; // 3GB
    size_t difference       = 0;

    do
    {
        // init request
        request.pagingType = PAGING_TYPE_IA32_4KB;
        request.param      = &allocParam;

        // init allocParam for the kernel code area
        allocParam.flag             = PAGING_FLAG_ALLOC_SHARED_MEMORY   |
                                      PAGING_FLAG_ALLOC_AT_ADDRESS      |
                                      PAGING_FLAG_ALLOC_MAPS_KERNEL;
        allocParam.currentPaging    = a_currentPaging;
        allocParam.user             = false;
        allocParam.write            = false;
        allocParam.cacheDisabled    = false;
        allocParam.writeThrough     = true;
        allocParam.virtualAddress   = requestedAddress;
        allocParam.length           = g_kernelArea.codeEndAddr -
                                      g_kernelArea.codeStartAddr;
        allocParam.physicalAddress  = g_kernelArea.codeStartAddr;

        error = IA32_4KB_alloc(a_paging, &request, &returnedAddress);
        if (error != ERROR_SUCCESS)
        {
            break;
        }

        requestedAddress = allocParam.length;
        if ((difference = allocParam.length % 4096) != 0)
        {
            requestedAddress += (4096 - difference);
        }

        allocParam.virtualAddress   = requestedAddress;
        allocParam.length           = g_kernelArea.rodataEndAddr -
                                      g_kernelArea.rodataStartAddr;
        allocParam.physicalAddress  = g_kernelArea.rodataStartAddr;

        error = IA32_4KB_alloc(a_paging, &request, &returnedAddress);
        if (error != ERROR_SUCCESS)
        {
            break;
        }

        requestedAddress = allocParam.length;
        if ((difference = allocParam.length % 4096) != 0)
        {
            requestedAddress += (4096 - difference);
        }

        allocParam.write            = true;
        allocParam.virtualAddress   = requestedAddress;
        allocParam.length           = g_kernelArea.rwdataEndAddr -
                                      g_kernelArea.rwdataStartAddr;
        allocParam.physicalAddress  = g_kernelArea.rwdataStartAddr;

        error = IA32_4KB_alloc(a_paging, &request, &returnedAddress);
        if (error != ERROR_SUCCESS)
        {
            break;
        }
    } while (false);

    if (error != ERROR_SUCCESS)
    {
        helper_IA32_4KB_deletePaging(a_paging);
    }

    return error;
}

size_t IA32_4KB_alloc(struct Paging *a_paging,
                      struct AllocFuncParam *a_request,
                      size_t *a_address)
{
    if (a_paging == NULL || a_request == NULL || a_address == NULL ||
        a_paging->pagingStruct == NULL || a_request->param == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    if (a_request->pagingType != PAGING_TYPE_IA32_4KB)
    {
        return ERROR_INVALID_PARAMETER;
    }

    size_t error = ERROR_SUCCESS;

    *a_address = NULL;

    struct IA32_PageDirectory_4KB     *pd;
    struct IA32_4KB_Paging_AllocParam *request;

    pd      = (struct IA32_PageDirectory_4KB*)     a_paging->pagingStruct;
    request = (struct IA32_4KB_Paging_AllocParam*) a_request->param;

    do
    {
    } while (false);

    return error;
}

size_t IA32_4KB_free(struct Paging *a_paging,
                     struct FreeFuncParam *a_request)
{
    if (a_paging == NULL || a_request == NULL ||
        a_paging->pagingStruct == NULL || a_request->param == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    if (a_request->pagingType != PAGING_TYPE_IA32_4KB)
    {
        return ERROR_INVALID_PARAMETER;
    }

    size_t error = ERROR_SUCCESS;

    struct IA32_PageDirectory_4KB     *pd;
    struct IA32_4KB_Paging_FreeParam  *request;

    pd      = (struct IA32_PageDirectory_4KB*)     a_paging->pagingStruct;
    request = (struct IA32_4KB_Paging_FreeParam*)  a_request->param;

    do
    {
    } while (false);

    return error;
}

/*
struct Paging g_kernelPaging;
struct IA32_Paging_4KB g_kernelPagingStruct;
struct IA32_PageDirectory_4KB g_kernelPageDirectory;

void helper_IA32_4KB_allocPage(struct IA32_PageDirectory_4KB *currentPageDirectory,
                               size_t a_address,
                               uint32 a_flags,
                               bool   a_write,
                               bool   a_user,
                               bool   a_writeThrough,
                               bool   a_cacheDisabled)
{
    size_t pageNum = (a_address / (4 * KiB)) % PAGING_IA32_PTE_NUMBER;
    size_t tableNum = (a_address / (4 * KiB)) / PAGING_IA32_PDE_NUMBER;
    kprintf("MAP PAGE %x [%d][%d]\n", a_address, tableNum, pageNum);

    if (currentPageDirectory->addresses[tableNum] == NULL)
    {
        //kprintf("ALLOC NEW TABLE\n");
        size_t addrToAlloc;
        PAA_alloc(4 * KiB, &addrToAlloc, 4 * KiB);
        // A heap should be used to alloc new page tables...
        //PMM_alloc(&addrToAlloc, 1, PMM_FOR_VIRTUAL_MEMORY);

        currentPageDirectory->addresses[tableNum] = (struct IA32_PageTable_4KB*) addrToAlloc;
        currentPageDirectory->entries[tableNum].address = (uint32) addrToAlloc;
        currentPageDirectory->entries[tableNum].write = a_write;
        currentPageDirectory->entries[tableNum].user = a_user;
        currentPageDirectory->entries[tableNum].writeThrough = a_writeThrough;
        currentPageDirectory->entries[tableNum].cacheDisabled = a_cacheDisabled;
    }

    currentPageDirectory->addresses[tableNum]->entries[pageNum].address = (uint32) a_address;
    currentPageDirectory->addresses[tableNum]->entries[pageNum].present = true;
    currentPageDirectory->addresses[tableNum]->entries[pageNum].write = a_write;
    currentPageDirectory->addresses[tableNum]->entries[pageNum].user = a_user;
    currentPageDirectory->addresses[tableNum]->entries[pageNum].writeThrough = a_writeThrough;
    currentPageDirectory->addresses[tableNum]->entries[pageNum].cacheDisabled = a_cacheDisabled;
}

void helper_IA32_4KB_freePage(struct Paging *a_paging,
                              size_t a_address)
{
    size_t pageNum = (a_address / (4 * KiB)) % PAGING_IA32_PTE_NUMBER;
    size_t tableNum = (a_address / (4 * KiB)) / PAGING_IA32_PDE_NUMBER;

    struct IA32_Paging_4KB *currentPagingStruct = (struct IA32_Paging_4KB*) a_paging->pagingStruct;
    struct IA32_PageDirectory_4KB *currentPageDirectory = currentPagingStruct->currentPageDirectory;

    if (currentPageDirectory->addresses[tableNum] == NULL)
    {
        return ;
    }

    currentPageDirectory->addresses[tableNum]->entries[pageNum].present = false;
    asm volatile ("invlpg (%0)" : : "a" ((uint32) a_address));
}

size_t IA32_4KB_initKernelStruct(struct Paging *a_paging,
                                 size_t a_codeStartAddr,
                                 size_t a_codeEndAddr,
                                 size_t a_rodataStartAddr,
                                 size_t a_rodataEndAddr,
                                 size_t a_rwdataStartAddr,
                                 size_t a_rwdataEndAddr)
{
    if (a_paging == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    if (a_codeEndAddr   <= a_codeStartAddr    ||
        a_rodataEndAddr <  a_rodataStartAddr  ||
        a_rwdataEndAddr <  a_rwdataStartAddr)
    {
        return ERROR_INVALID_PARAMETER;
    }

    if ((a_codeStartAddr   & 4095) != 0 ||
        (a_rodataStartAddr & 4095) != 0 ||
        (a_rwdataStartAddr & 4095) != 0)
    {
        return ERROR_UNALIGNED_ADDRESS;
    }

    a_paging->pagingStruct = (void*) &g_kernelPagingStruct;
    g_kernelPagingStruct.currentPageDirectory = &g_kernelPageDirectory;

    kmemset(g_kernelPageDirectory.entries, 0, sizeof(struct IA32_PageDirectory_4KB_Entry) * PAGING_IA32_PDE_NUMBER);
    for (size_t i = 0; i < PAGING_IA32_PDE_NUMBER; i++)
    {
        g_kernelPageDirectory.addresses[i] = NULL;
    }

    size_t pages = (a_kernelEndAddr - a_kernelStartAddr) / (4 * KiB);
    kprintf("Start: %x End: %x Pages: %d\n", a_kernelStartAddr, a_kernelEndAddr, pages);
    IA32_4KB_allocAtAddress(a_paging, a_kernelStartAddr, pages, 0, true, true, false, false);

    return ERROR_SUCCESS;
}

size_t IA32_4KB_alloc(struct Paging *a_paging,
                      size_t         a_pagesNumber,
                      uint32         a_flags,
                      bool           a_write,
                      bool           a_user,
                      bool           a_writeThrough,
                      bool           a_cacheDisabled,
                      size_t        *a_address)
{
    if (a_paging == NULL || a_address == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    struct IA32_Paging_4KB *currentPagingStruct = (struct IA32_Paging_4KB*) a_paging->pagingStruct;
    struct IA32_PageDirectory_4KB *currentPageDirectory = currentPagingStruct->currentPageDirectory;

    size_t address;
    PMM_alloc(&address, a_pagesNumber, PMM_FOR_VIRTUAL_MEMORY);

    for (size_t i = 0; i < a_pagesNumber; i++, address += 4 * KiB)
    {
        helper_IA32_4KB_allocPage(currentPageDirectory, address, a_flags,
                                a_write, a_user, a_writeThrough, a_cacheDisabled);
    }

    return ERROR_SUCCESS;
}

size_t IA32_4KB_allocAtAddress(struct Paging *a_paging,
                               size_t         a_address,
                               size_t         a_pagesNumber,
                               uint32         a_flags,
                               bool           a_write,
                               bool           a_user,
                               bool           a_writeThrough,
                               bool           a_cacheDisabled)
{
    if (a_paging == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    struct IA32_Paging_4KB *currentPagingStruct = (struct IA32_Paging_4KB*) a_paging->pagingStruct;
    struct IA32_PageDirectory_4KB *currentPageDirectory = currentPagingStruct->currentPageDirectory;

    for (size_t i = 0, address = a_address; i < a_pagesNumber; i++, address += 4 * KiB)
    {
        helper_IA32_4KB_allocPage(currentPageDirectory, address, a_flags,
                                a_write, a_user, a_writeThrough, a_cacheDisabled);
    }

    return ERROR_SUCCESS;
}

size_t IA32_4KB_free(struct Paging *a_paging,
                     size_t         a_address,
                     size_t         a_pagesNumber,
                     uint32         a_flags)
{
    if (a_paging == NULL || a_address == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    PMM_free(a_address, a_pagesNumber, PMM_FOR_VIRTUAL_MEMORY);
    for (size_t i = 0, address = a_address; i < a_pagesNumber; i++, address += 4 * KiB)
    {
        helper_IA32_4KB_freePage(a_paging, address);
    }

    return ERROR_SUCCESS;
}
*/
