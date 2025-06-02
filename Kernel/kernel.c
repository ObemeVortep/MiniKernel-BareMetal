#include "Kernel/kernel.h"

/*
*   Main kernel entry point.
*   ⚠️ Important: if the kernel size changes (e.g., new modules are added),
*   make sure to update the bootloader and protected mode entry code.
*
*   Both bootloader.asm and pm_entry.asm must be adjusted to reflect the correct kernel size,
*   since the bootloader loads a fixed number of sectors into memory and relocates the kernel to address 0x100000.
*/

void kernel_main() {
    /*
    *   Initialization phase — these functions must be called before anything else.
    */

    // Initialize memory management
    kmemory_init();

    // Set up the Interrupt Descriptor Table (IDT) and all related interrupt handling
    idt_init();

    // Initialize thread system and scheduler
    kthreads_init();

    /*
    *   Core kernel systems have been successfully initialized.
    *   At this point, we can:
    *   - Allocate memory
    *   - Output to screen via VGA
    *   - Create and manage threads
    *   - Handle hardware/software interrupts
    */

    // Infinite loop to prevent accidental execution of uninitialized code
    while(1);
}