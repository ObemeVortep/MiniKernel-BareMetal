#include "Types/common.h"
#include "Ports/kports.h"
#include "Interrupts/PIT/pit.h"

/*
*   This file handles PIT (Programmable Interval Timer) configuration and frequency setup.
*/

#define PIT_CMD_PORT        0x43        // Command port for the PIT
#define PIT_DATA_PORT       0x40        // Data port for the PIT
#define PIT_FREQUENCY       1193182     // Default input clock frequency of the PIT (Hz)

volatile uint32_t uiPitTicks = 0;       // PIT tick counter — incremented in irq0_handler!

// Initializes the PIT to generate interrupts 1000 times per second
void pit_init() {
    // Calculate the divisor to set desired frequency (max value = 65535)
    uint16_t uiDivisor = PIT_FREQUENCY / 1000;
    uint8_t lo = uiDivisor & 0xFF;
    uint8_t hi = (uiDivisor >> 8) & 0xFF;

    // Send PIT command:
    // Channel 0, access mode = lobyte/hibyte, mode = 3 (square wave), binary mode
    outb(PIT_CMD_PORT, 0x36);

    // Send calculated divisor in two steps
    outb(PIT_DATA_PORT, lo);     // First: low byte
    outb(PIT_DATA_PORT, hi);     // Then: high byte
}