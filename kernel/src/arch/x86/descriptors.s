.section text
.global GDT_Load
.type GDT_Load, @function

GDT_Load:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    jmp $0x08, $flush
flush:
    ret
