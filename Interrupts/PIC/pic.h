#ifndef _IDT_PIC_H
#define _IDT_PIC_H

// Initializes the Programmable Interrupt Controller (PIC) and sets the IRQ masks
void pic_init(const uint8_t uiMasterMask, const uint8_t uiSlaveMask);

#endif // _IDT_PIC_H