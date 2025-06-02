#ifndef _MEMORY_TYPEDEFS_H
#define _MEMORY_TYPEDEFS_H

// Macro to align a given number x to the next multiple of 4
#define ALIGN4(x) (((x) + 3) & ~3)

/*
*   STRUCTURE OF AN ALLOCATED BUFFER:
*   | MEMORY_BLOCK (9 bytes) | ALIGN (3 bytes) | pBuffer (size bytes) | ALIGN (optional) |
*                                         or
*   | ALIGN4(sizeof(MEMORY_BLOCK)) | ALIGN4(size) |
*
*   That is, in memory, you must account not only for the buffer itself (pBuffer),
*   but also for its associated MEMORY_BLOCK and any necessary alignment!
*
*   To get a pointer to the MEMORY_BLOCK for a given pBuffer,
*   subtract ALIGN4(sizeof(MEMORY_BLOCK)) bytes from the pBuffer pointer.
*/
typedef struct _MEMORY_BLOCK {
    uint32_t                uiSize;     // Size of the current block
    uint8_t                 isFree;     // Whether this block is free
    struct MEMORY_BLOCK*    pNext;      // Pointer to the next memory block
} MEMORY_BLOCK;

#endif // _MEMORY_TYPEDEFS_H