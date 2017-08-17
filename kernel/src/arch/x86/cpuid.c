#include "kernel/include/arch/x86/cpuid.h"
#include "util/kstdlib/include/kstring.h"

/*
 * It's implemented in cpuid.s
 */
uint32 CPUID_IsSuported();

/*
 * It's implemented in cpuid.s
 */
void CPUID_Call(
    uint32 eax_value,
    uint32 ebx_value,
    uint32 ecx_value,
    uint32 edx_value,
    uint32 *eax,
    uint32 *ebx,
    uint32 *ecx,
    uint32 *edx);

struct CPUID_eax_01h
{
    uint32 eax;
    uint32 ebx;
    uint32 ecx;
    uint32 edx;
};

struct CPUID_eax_00h
{
    uint32 eax;
    char vendor[13];
};

struct CPUInfo
{
    uint32 isInitialized;
    struct CPUID_eax_00h cpuid_eax_00h;
    struct CPUID_eax_01h cpuid_eax_01h;
};

#define SIZE_T_TO_CHAR(str, value)              \
    (str)[0] = (char)((value >>  0) & 0xFF);    \
    (str)[1] = (char)((value >>  8) & 0xFF);    \
    (str)[2] = (char)((value >> 16) & 0xFF);    \
    (str)[3] = (char)((value >> 24) & 0xFF);

static struct CPUInfo g_cpuInfo;

uint32 CPUID_Init()
{
    uint32 error = kmemset((void*) &g_cpuInfo, 0, sizeof(struct CPUInfo));
    if (error != ERROR_SUCCESS) {
        return error;
    }

    uint32 isSuported = CPUID_IsSuported();

    if (isSuported == 0) {
        return ERROR_UNSUPPORTED;
    }

    uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;

    CPUID_Call(0, 0, 0, 0, &eax, &ebx, &ecx, &edx);
    g_cpuInfo.cpuid_eax_00h.eax = eax;
    SIZE_T_TO_CHAR(g_cpuInfo.cpuid_eax_00h.vendor + 0, ebx);
    SIZE_T_TO_CHAR(g_cpuInfo.cpuid_eax_00h.vendor + 4, edx);
    SIZE_T_TO_CHAR(g_cpuInfo.cpuid_eax_00h.vendor + 8, ecx);

    CPUID_Call(1, 0, 0, 0, &eax, &ebx, &ecx, &edx);
    g_cpuInfo.cpuid_eax_01h.eax = eax;
    g_cpuInfo.cpuid_eax_01h.ebx = ebx;
    g_cpuInfo.cpuid_eax_01h.ecx = ecx;
    g_cpuInfo.cpuid_eax_01h.edx = edx;

    g_cpuInfo.isInitialized = 1;

    return ERROR_SUCCESS;
}

uint32 CPUID_GetVendorName(
    const char **a_vendorName)
{
    if (a_vendorName == NULL) {
        return ERROR_NULL_POINTER;
    }

    if (g_cpuInfo.isInitialized == 0) {
        return ERROR_UNINITIALIZED;
    }

    *a_vendorName = g_cpuInfo.cpuid_eax_00h.vendor;

    return ERROR_SUCCESS;
}

uint32 CPUID_HasFeature(
    uint32 a_featureId)
{
    if (g_cpuInfo.isInitialized == 0) {
        return ERROR_UNINITIALIZED;
    }

    if (a_featureId >= 0x00000100 && a_featureId <= 0x0000011F) {
        switch (a_featureId) {
            case 0x00000110:
            case 0x0000011F:
                return ERROR_INVALID_PARAMETER;
        }

        uint32 rangBit = a_featureId ^ 0x00000100;
        uint32 ret = g_cpuInfo.cpuid_eax_01h.ecx & (1 << rangBit);

        if (ret == 0) {
            return ERROR_UNSUPPORTED;
        }

        return ERROR_SUCCESS;
    }

    if (a_featureId >= 0x00000200 && a_featureId <= 0x0000021F) {
        switch (a_featureId) {
            case 0x0000020A:
            case 0x00000214:
            case 0x0000021E:
                return ERROR_INVALID_PARAMETER;
        }

        uint32 rangBit = a_featureId ^ 0x00000200;
        uint32 ret = g_cpuInfo.cpuid_eax_01h.edx & (1 << rangBit);

        if (ret == 0) {
            return ERROR_UNSUPPORTED;
        }

        return ERROR_SUCCESS;
    }

    return ERROR_INVALID_PARAMETER;
}
