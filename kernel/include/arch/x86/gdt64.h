#ifndef _PhiOS_gdt64
#define _PhiOS_gdt64

#include "types.h"
#include "arch/x86/gdt32.h"

size_t GDT_init64();
extern void GDT_Load64(uint64 a_table);

#endif
