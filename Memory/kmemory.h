#ifndef _KMEMORY_H
#define _KMEMORY_H

// Mandatory initialization function for the KMEMORY module
void kmemory_init(void);                                      

// Allocates a buffer of the given size and returns a pointer to it  
PVOID kmalloc(const uint32_t uSizeToAllocate);                

// Frees the allocated MEMORY_BLOCK. Detailed logic is in kmemory.c
void kfree(PVOID pBuffer);                                      

// Fills pBuffer with uiValue, repeated uiSize times
void kmemset(PVOID pBuffer, uint8_t uiValue, uint32_t uiSize);  

#endif // _KMEMORY_H