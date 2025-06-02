#ifndef _INTERRUPTS_IDT_TYPEDEFS_H
#define _INTERRUPTS_IDT_TYPEDEFS_H

// The IDT consists of 256 entries, each represented by an IDT_ENTRY structure
typedef struct __attribute__((packed)) _IDT_ENTRY {
    uint16_t uiBaseLow;      // Lower 16 bits of the ISR (interrupt service routine) address
    uint16_t uiSelector;     // GDT segment selector (e.g. 0x08 for kernel code segment)
    uint8_t  uiZero;         // Reserved, must be 0
    uint8_t  uiFlags;        // Type and attributes (e.g. 0x8E = 32-bit interrupt gate, Ring 0, present)
    uint16_t uiBaseHigh;     // Upper 16 bits of the ISR address
} IDT_ENTRY;

// Pointer structure used by 'lidt' instruction to load the IDT
typedef struct __attribute__((packed)) _IDT_PTR {
    uint16_t uiLimit;        // Size of the IDT in bytes - 1 (processor adds +1 internally)
    uint32_t uiBase;         // Base address of the IDT
} IDT_PTR;

#endif // _INTERRUPTS_IDT_TYPEDEFS_H