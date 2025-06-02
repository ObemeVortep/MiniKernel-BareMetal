#include "Types/common.h"
#include "Interrupts/IDT/typedefs.h"
#include "Memory/kmemory.h"             // For memory utility functions
#include "Interrupts/PIC/pic.h"         // For initializing the PIC
#include "Interrupts/PIT/pit.h"         // For initializing the PIT
#include "Interrupts/IDT/idt.h"

// IRQ0 handler for PIT. Must be mapped to vector 0x20 in the IDT.
extern void irq0_handler();             // Implemented in Interrupts/IRQ/irq_handlers.asm

#define IDT_SIZE        256             // Total number of IDT entries (max 256)

/*
*   This code is responsible for initializing the processor's IDT (Interrupt Descriptor Table).
*/

static IDT_ENTRY idt[IDT_SIZE];         // Actual IDT array
static IDT_PTR idtp;                    // Pointer to the IDT to be loaded into the CPU

// Sets an individual gate in the IDT
static void idt_set_gate(uint8_t uiNum, uint32_t uiHandlerAddress, uint16_t uiSelector, uint8_t uiFlags) {
    // Populate the IDT entry with provided parameters
    idt[uiNum].uiBaseLow     = uiHandlerAddress & 0xFFFF;            // Lower 16 bits of handler address
    idt[uiNum].uiSelector    = uiSelector;                           // Code segment selector
    idt[uiNum].uiZero        = 0;                                    // Reserved (must be zero)
    idt[uiNum].uiFlags       = uiFlags;                              // Flags: P|DPL|Type (bit layout: P(7) | DPL(6:5) | 0 | Type(3:0))
    idt[uiNum].uiBaseHigh    = (uiHandlerAddress >> 16) & 0xFFFF;    // Upper 16 bits of handler address
}

// Initializes the IDT and sets up interrupt handling
void idt_init() {

/*
*   STEP 1: Configure and load the IDT
*/
    // Set up IDT pointer
    idtp.uiLimit = sizeof(idt) - 1;
    idtp.uiBase  = &idt;

    // Clear the entire IDT
    kmemset(&idt, 0x00, sizeof(idt));

    // Set IRQ0 handler (used by PIT) at vector 0x20
    idt_set_gate(0x20, (uint32_t)irq0_handler, 0x08, 0x8E);

    // Load the IDT into the processor using lidt
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r"(&idtp)
    );

/*
*   STEP 2: Initialize the PIC
*/
    // Remap the PIC to avoid conflicts with CPU exceptions
    // Only IRQ0 is handled, mapped to IDT vector 0x20
    pic_init(0xFE, 0xFF);

/*
*   STEP 3: Initialize the PIT
*/
    // Set up PIT to generate IRQ0 1000 times per second
    pit_init();

/*
*   STEP 4: Enable interrupts globally
*/
    // Set the IF (Interrupt Flag) in EFLAGS — enables hardware interrupt handling
    __asm__ volatile (
        "sti"
    );
}