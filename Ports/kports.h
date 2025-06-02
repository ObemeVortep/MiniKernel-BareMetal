#ifndef _KPORTS_H
#define _KPORTS_H

// Sends a single byte `uiValue` to the specified I/O `uiPort`
void outb(const uint16_t uiPort, const uint8_t uiValue);

// Receives a single byte from the specified I/O `uiPort`
uint8_t inb(const uint16_t uiPort);

#endif // _KPORTS_H