#include "Types/common.h"
#include "Ports/kports.h"
#include "Interrupts/PIC/pic.h"

/*
*   This file handles initialization of the Programmable Interrupt Controller (PIC).
*   ⚠️ Important:
*   - IRQ2 on the Master PIC is reserved (used to cascade the Slave PIC).
*   - In the IRQ mask, 0 = enabled, 1 = disabled.
*/

#define MASTER_COMMAND_PORT         0x20    // Command port for the Master PIC
#define SLAVE_COMMAND_PORT          0xA0    // Command port for the Slave PIC

#define MASTER_DATA_PORT            0x21    // Data port for the Master PIC
#define SLAVE_DATA_PORT             0xA1    // Data port for the Slave PIC

// Sets a new IRQ mask for the Master PIC
static FORCE_INLINE void set_master_mask(const uint8_t uiNewMask) {
    outb(MASTER_DATA_PORT, uiNewMask);
}

// Sets a new IRQ mask for the Slave PIC
static FORCE_INLINE void set_slave_mask(const uint8_t uiNewMask) {
    outb(SLAVE_DATA_PORT, uiNewMask);
}

// Initializes the Master and Slave PICs and applies IRQ masks
void pic_init(const uint8_t uiMasterMask, const uint8_t uiSlaveMask) {
    // Begin PIC initialization sequence (ICW1) — controller expects 3 additional bytes
    outb(MASTER_COMMAND_PORT, 0x11);    // ICW1: edge-triggered, cascaded, ICW4 required
    outb(SLAVE_COMMAND_PORT, 0x11);     // Same for Slave

    // ICW2: Set interrupt vector offset for each PIC
    outb(MASTER_DATA_PORT, 0x20);       // Master IRQs will start at IDT vector 0x20 (32)
    outb(SLAVE_DATA_PORT, 0x28);        // Slave IRQs will start at IDT vector 0x28 (40)

    // ICW3: Setup Master-Slave wiring
    outb(MASTER_DATA_PORT, 0x04);       // Master has Slave on IRQ2 (bit 2 set)
    outb(SLAVE_DATA_PORT, 0x02);        // Slave is connected to IRQ2 (value = 2)

    // ICW4: Set 8086/88 (MCS-80/85) mode
    outb(MASTER_DATA_PORT, 0x01);       // Enable 8086 mode
    outb(SLAVE_DATA_PORT, 0x01);        // Same for Slave

    // Initialization complete — apply IRQ masks
    set_master_mask(uiMasterMask);
    set_slave_mask(uiSlaveMask);
}