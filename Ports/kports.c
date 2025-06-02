#include "Types/common.h"
#include "Ports/kports.h"

/*
*   This .c file is responsible for managing and interacting with the system's I/O ports
*/

// Sends a single byte `uiValue` to the specified `uiPort`
void outb(const uint16_t uiPort, const uint8_t uiValue) {
    __asm__ volatile (
        "outb %1, %0"
        : 
        : "Nd"(uiPort), "a"(uiValue)
    );
}

// Receives a single byte from the specified `uiPort`
uint8_t inb(const uint16_t uiPort) {
    uint8_t uiRes;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(uiRes)
        : "Nd"(uiPort)
    );
    return uiRes;
}