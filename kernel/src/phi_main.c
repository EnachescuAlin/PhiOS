#include "drivers/keyboard/include/keyboard.h"
#include "util/kstdlib/include/kstdio.h"
#include "kernel/include/arch/x86/gdt32.h"
#include "kernel/include/arch/x86/idt32.h"
#include "kernel/include/arch/x86/tss32.h"
#include "kernel/include/arch/x86/pit.h"
#include "kernel/include/arch/x86/pic.h"

extern size_t g_kernelStack[2048];

// TODO: remove this when tasks are available
size_t g_userStack[2048]; // temporary user mode

void user_main()
{
    kprintf("Hello, world!\n");
    kprintf("> ");
    while (1)
    {
        char c = keyboard_readKey();
        if (c == '\n')
        {
            kprintf("\n> ");
        }

        if (c == 'q')
        {
            break;
        }
        else
        {
            kprintf("%c", c);
        }
    }
}

int x, y;

void print(void* addr)
{
    kprintf("%p\n", addr);
}

void f1()
{
    print(&y);
}

void f2()
{
    x = 0;
    print(&x);
}

void kernel_main()
{
    //f1();
    //f2();

    kprintf("paging enabled\n");

    // Inits GDT for 32-bit
    GDT32_init();

    // Sets kernel stack in TSS struct
    TSS32_setKernelStack((uint32) &g_kernelStack[2047]);

    // Inits IDT for 32-bit
    IDT32_init();

    // Inits PIC
    PIC_init();
    PIC_maskUnusedIRQs();

    // Inits timer
    PIT_init((uint16) -1);
    kprintf("[SYSTEM] Initialized timer at %d frequency.\n", OSCILLATOR_FREQUENCY);

    // Inits keyboard
    keyboard_init();
    kprintf("[SYSTEM] Initialized keyboard.\n");

    return;
}
