#include "types.h"
#include "arch/x86/gdt.h"
#include "errors.h"

#include "kstdio.h"

GDT g_GDTStruct[GDT_ENTRIES];
GDT_Entry g_GDTEntries[GDT_ENTRIES];
GDT_Table g_GDTTable;

size_t GDT_init32()
{
    GDT_setStruct(&g_GDTStruct[0], 0, 0, 0);
    GDT_setStruct(&g_GDTStruct[1], 0, 0x000FFFFF, GDT_CODE_PL0);
    GDT_setStruct(&g_GDTStruct[2], 0, 0x000FFFFF, GDT_CODE_PL0);
    GDT_setStruct(&g_GDTStruct[3], 0, 0x000FFFFF, GDT_CODE_PL3);
    GDT_setStruct(&g_GDTStruct[4], 0, 0x000FFFFF, GDT_CODE_PL3);
    GDT_createEntries(g_GDTStruct);

    g_GDTTable.limit = sizeof(GDT_Entry) * GDT_ENTRIES - 1;
    g_GDTTable.base = (uint32) &g_GDTEntries;

    GDT_Load((uint32) &g_GDTTable);

    return ERROR_SUCCESS;
}

size_t GDT_init64()
{
    return ERROR_SUCCESS;
}

size_t GDT_setStruct(GDT *a_gdt, uint32 a_base,
                    uint32 a_limit, uint16 a_type)
{
    if (a_gdt == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    a_gdt->base = a_base;
    a_gdt->limit = a_limit;
    a_gdt->type = a_type;

    return ERROR_SUCCESS;
}

size_t GDT_getStruct(uint32 a_num, GDT **a_gdt)
{
    if (a_num >= GDT_ENTRIES)
    {
        return ERROR_UNSUPPORTED;
    }

    if (a_gdt == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    *a_gdt = &g_GDTStruct[a_num];
    return ERROR_SUCCESS;
}

size_t GDT_createEntries(GDT *a_gdtArray)
{
    if (a_gdtArray == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    for (int i = 0; i < GDT_ENTRIES; i++)
    {
        GDT_Entry descriptor;
        uint32 base = a_gdtArray[i].base;
        uint32 limit = a_gdtArray[i].limit;
        uint16 type = a_gdtArray[i].type;
        kprintf("GDT %d: %u %u %u\n", i, base, limit, type);

        descriptor  = limit        & 0x000F0000;
        descriptor |= (type << 8)  & 0x00F0FF00;
        descriptor |= (base >> 16) & 0x000000FF;
        descriptor |= base         & 0xFF000000;

        descriptor <<= 32;

        descriptor |= base  << 16;
        descriptor |= limit  & 0x0000FFFF;

        g_GDTEntries[i] = descriptor;
        kprintf("Entry: %u == %u?\n", descriptor, g_GDTEntries[i]);

    }

    return ERROR_SUCCESS;
}
