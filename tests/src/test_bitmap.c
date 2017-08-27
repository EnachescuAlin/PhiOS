#include <cut.h>

#include <stdlib.h>

#include "kernel/include/memory/bitmap_pma.h"
#include "kernel/include/memory/paa.h"

#define ALIGN(addr, align) addr = addr & (~(align - 1))
#define CHECK_ALIGN(addr, align) ((addr & (align - 1)) != 0)

CUT_DEFINE_TEST(test_bitmapCreate)
{
    struct BitmapPMA bpma = { 0 };
    uint32 err;

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);
    CUT_CHECK(bpma.startAddress == 0x0);
    CUT_CHECK(bpma.endAddress == 0x6400000);
    CUT_CHECK(bpma.frameSize == 0x1000);
    CUT_CHECK(bpma.freeFramesNumber == 25600);
    CUT_CHECK(bpma.positionLastAllocatedFrame == 0);
    CUT_CHECK(bpma.bitmapSize == 400);
    CUT_CHECK(bpma.bitmap != NULL);

    free(bpma.bitmap);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x200123, // ~2 mibs
        0x6400123 // ~100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);
    CUT_CHECK(bpma.startAddress == 0x201000);
    CUT_CHECK(bpma.endAddress == 0x6400000);
    CUT_CHECK(bpma.frameSize == 0x1000);
    CUT_CHECK(bpma.freeFramesNumber == 25087);
    CUT_CHECK(bpma.positionLastAllocatedFrame == 0);
    CUT_CHECK(bpma.bitmapSize == 392);
    CUT_CHECK(bpma.bitmap != NULL);

    free(bpma.bitmap);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x1000,
        0x0
    );
    CUT_CHECK(err == ERROR_INVALID_PARAMETER);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x1000,
        0x1000
    );
    CUT_CHECK(err == ERROR_INVALID_PARAMETER);

    err = BitmapPMA_createAllocator(
        NULL,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_NULL_POINTER);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1001,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_INVALID_PARAMETER);
}

CUT_DEFINE_TEST(test_bitmapAlloc)
{
    // first column is the size and the second column is the expected address
    static uint64 testCases[][2] = {
        0x00005000, 0x00000000,
        0x00000123, 0x00005000,
        0x00002012, 0x00006000,
        0x000F7000, 0x00009000,
        0x000FFFFF, 0x00100000,
        0x00200000, 0x00200000,
        0x003FE001, 0x00400000,
        0x00001000, 0x007FF000,
        0x05BFE123, 0x00800000,
        0x00001000, 0x063FF000
    };
    static uint32 testCasesLength = 10;

    struct BitmapPMA bpma = { 0 };
    uint32 err = 0;
    uint64 addr = 0;

    CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[0][0], &addr) == ERROR_UNINITIALIZED);

    /*----- when startAddress is at 0x0 -----*/

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    CUT_CHECK(BitmapPMA_alloc( NULL, 0x1000, &addr) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x1000,  NULL) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x0000, &addr) == ERROR_INVALID_PARAMETER);

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == testCases[i][1]);
    }

    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x1, &addr) == ERROR_NO_FREE_MEMORY);

    free(bpma.bitmap);

    /*----- when startAddress is greater than 0x0 -----*/

    err = BitmapPMA_createAllocator(
        &bpma, 
        0x1000, 
        0x400000, // 4 mibs
        0x6800000); // 104 mibs
    CUT_CHECK(err == ERROR_SUCCESS);

    CUT_CHECK(BitmapPMA_alloc( NULL, 0x1000, &addr) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x1000,  NULL) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x0000, &addr) == ERROR_INVALID_PARAMETER);

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == (testCases[i][1] + bpma.startAddress));
    }

    CUT_CHECK(BitmapPMA_alloc(&bpma, 0x1, &addr) == ERROR_NO_FREE_MEMORY);

    free(bpma.bitmap);
}

CUT_DEFINE_TEST(test_bitmapFree)
{
    // first column is the size and the second column is the expected address
    static uint64 testCases[][2] = {
        0x00005000, 0x00000000,
        0x00000123, 0x00005000,
        0x00002012, 0x00006000,
        0x000F7000, 0x00009000,
        0x000FFFFF, 0x00100000,
        0x00200000, 0x00200000,
        0x003FE001, 0x00400000,
        0x00001000, 0x007FF000,
        0x05BFE123, 0x00800000,
        0x00001000, 0x063FF000
    };
    static uint32 testCasesLength = 10;

    struct BitmapPMA bpma = { 0 };
    uint32 err = 0;
    uint64 addr = 0;

    CUT_CHECK(BitmapPMA_free(&bpma, 0x1000, 0x0) == ERROR_UNINITIALIZED);

    /*----- when startAddress is at 0x0 -----*/

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    CUT_CHECK(BitmapPMA_free( NULL, 0x00001000, 0x00001000          ) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, bpma.startAddress   ) == ERROR_INVALID_STATE);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00002000, 0x063FF000          ) == ERROR_INVALID_STATE);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x06500000, 0x00001000          ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00000000, 0x00001000          ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, bpma.endAddress + 1 ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, 0x00000123          ) == ERROR_INVALID_PARAMETER);

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == testCases[i][1]);
    }

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_free(&bpma, testCases[i][0], testCases[i][1]) == ERROR_SUCCESS);
    }

    bpma.positionLastAllocatedFrame = 0;

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == testCases[i][1]);
    }

    free(bpma.bitmap);

    /*----- when startAddress is greater than 0x0 -----*/

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x400000, // 4 mibs
        0x6800000 // 104 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    CUT_CHECK(BitmapPMA_free( NULL, 0x00001000, 0x00001000              ) == ERROR_NULL_POINTER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, bpma.startAddress       ) == ERROR_INVALID_STATE);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00002000, 0x063FF000              ) == ERROR_INVALID_STATE);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x06500000, 0x00001000              ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00000000, 0x00001000              ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, bpma.endAddress + 1     ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, bpma.startAddress - 1   ) == ERROR_INVALID_PARAMETER);
    CUT_CHECK(BitmapPMA_free(&bpma, 0x00001000, 0x00000123              ) == ERROR_INVALID_PARAMETER);

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == (testCases[i][1] + bpma.startAddress));
    }

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_free(&bpma, testCases[i][0], testCases[i][1]  + bpma.startAddress) == ERROR_SUCCESS);
    }

    bpma.positionLastAllocatedFrame = 0;

    for (uint32 i = 0; i < testCasesLength; i++) {
        CUT_CHECK(BitmapPMA_alloc(&bpma, testCases[i][0], &addr) == ERROR_SUCCESS);
        CUT_CHECK(addr == (testCases[i][1] + bpma.startAddress));
    }

    free(bpma.bitmap);
}

CUT_DEFINE_TEST(test_bitmapReserve)
{
    struct BitmapPMA bpma;
    uint32 err;

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x400000, // 4 mibs
        0x6800000 // 104 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);
}

CUT_DEFINE_TEST(test_bitmapCheck)
{
    struct BitmapPMA bpma;
    uint32 err;

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x400000, // 4 mibs
        0x6800000 // 104 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);
}

CUT_DEFINE_TEST(test_bitmapAll)
{
    struct BitmapPMA bpma;
    uint32 err;

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x0,
        0x6400000 // 100 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);

    err = BitmapPMA_createAllocator(
        &bpma,
        0x1000,
        0x400000, // 4 mibs
        0x6800000 // 104 mibs
    );
    CUT_CHECK(err == ERROR_SUCCESS);

    free(bpma.bitmap);
}

CUT_DEFINE_MAIN
    CUT_CALL_TEST(test_bitmapCreate);
    CUT_CALL_TEST(test_bitmapAlloc);
    CUT_CALL_TEST(test_bitmapFree);
    CUT_CALL_TEST(test_bitmapReserve);
    CUT_CALL_TEST(test_bitmapCheck);
    CUT_CALL_TEST(test_bitmapAll);
CUT_END_MAIN
