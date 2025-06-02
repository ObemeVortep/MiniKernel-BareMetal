SECTIONS {
    /* Set the base address where the kernel will be loaded */
    . = 0x100000;

    .text :
    {
        *(.text*)              /* All executable code */
    }

    .rodata :
    {
        *(.rodata*)            /* Read-only data: constants, strings, etc. */
    }

    .data :
    {
        *(.data*)              /* Initialized global/static variables */
    }

    .bss :
    {
        *(.bss*)               /* Uninitialized global/static variables */
    }
}
