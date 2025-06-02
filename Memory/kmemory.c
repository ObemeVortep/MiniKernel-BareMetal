#include "Types/common.h"
#include "Memory/typedefs.h"
#include "Memory/kmemory.h"

/*
*   Approximately 1MB of heap memory will be used for internal kernel needs.
*   MEMORY IS MANAGED USING MEMORY_BLOCK STRUCTURES. THIS IS SUFFICIENT FOR CURRENT USAGE.
*/

static const uint32_t HEAP_START   = 0x102000;  // Assumes the kernel is loaded between 0x100000 and 0x102000
static const uint32_t HEAP_END     = 0x200000;  // We occupy all remaining space up to 0x200000

// Pointer to the first free byte in the heap
static PVOID pCurrentFreeHeap   = 0;            // Initialized in kmemory_init

// Linked list of allocated memory blocks
static MEMORY_BLOCK* pAllocatedBlocks = 0;      // Initialized in kmemory_init        

// Checks whether uiSizeToAllocate bytes can be allocated in heap. Returns 1 if yes, 0 if no.
static FORCE_INLINE uint8_t is_heap_enough(const uint32_t uiSizeToAllocate) {
    return ((HEAP_END - (uint32_t)pCurrentFreeHeap) >= ALIGN4(uiSizeToAllocate));
}

// Increments the current heap pointer by a 4-byte-aligned size
static FORCE_INLINE void increment_current_free_heap(const uint32_t uiSizeToIncrement) {
    pCurrentFreeHeap += ALIGN4(uiSizeToIncrement);
}

// Decrements the current heap pointer by a 4-byte-aligned size
static FORCE_INLINE void decrement_current_free_heap(const uint32_t uiSizeToDecrement) {
    pCurrentFreeHeap -= ALIGN4(uiSizeToDecrement);
}

// Inline function that returns a buffer pointer from a MEMORY_BLOCK*
static FORCE_INLINE PVOID get_bufferptr_from_memoryblockptr(MEMORY_BLOCK* pMemoryBlock) {
    return (PVOID)((uint8_t*)pMemoryBlock + ALIGN4(sizeof(MEMORY_BLOCK)));
}

// Inline function that returns a MEMORY_BLOCK* from a buffer pointer
static FORCE_INLINE MEMORY_BLOCK* get_memoryblockptr_from_bufferptr(PVOID pBuffer) {
    return (MEMORY_BLOCK*)((uint8_t*)pBuffer - ALIGN4(sizeof(MEMORY_BLOCK)));
}

// Mandatory initialization function for the KMEMORY module
void kmemory_init(void) {
    // Set the heap pointer to HEAP_START
    pCurrentFreeHeap = HEAP_START;

    // Allocate the first MEMORY_BLOCK
    pAllocatedBlocks = pCurrentFreeHeap;                // Allocate at pCurrentFreeHeap
    increment_current_free_heap(sizeof(MEMORY_BLOCK));  // Move the heap pointer forward by block size

    // Initialize the first MEMORY_BLOCK
    pAllocatedBlocks->uiSize  = 4;       // 4 bytes for the initial block    
    pAllocatedBlocks->isFree = 1;       // Marked as free    
    pAllocatedBlocks->pNext  = nullptr; // No next MEMORY_BLOCK
}

// Allocates a buffer of size uiSizeToAllocate and returns a pointer to it
PVOID kmalloc(const uint32_t uiSizeToAllocate) {
    // Validate the size and check if we have enough memory
    if (uiSizeToAllocate == 0) {
        return nullptr;
    }

    MEMORY_BLOCK* pBlocksCopy = pAllocatedBlocks;
    PVOID pBuffer = nullptr;

    // Iterate through all memory blocks until we find a suitable free one or reach the end
    while(1) {
        if (pBlocksCopy->isFree) {
            // If block is free, check if there's a next MEMORY_BLOCK
            if (pBlocksCopy->pNext == nullptr) {
                // No next block — allowed to resize this one
                if (ALIGN4(uiSizeToAllocate) > ALIGN4(pBlocksCopy->uiSize)) {
                    // Requested size is larger — check if enough heap space is available
                    if (is_heap_enough(ALIGN4(uiSizeToAllocate) - ALIGN4(pBlocksCopy->uiSize))) {
                        increment_current_free_heap(ALIGN4(uiSizeToAllocate) - ALIGN4(pBlocksCopy->uiSize));
                    } else {
                        // Not enough space and this is the last block
                        pBuffer = nullptr;
                        break;
                    }
                } else if (ALIGN4(pBlocksCopy->uiSize) > ALIGN4(uiSizeToAllocate)) {
                    // Requested size is smaller — shrink the heap
                    decrement_current_free_heap(ALIGN4(pBlocksCopy->uiSize) - ALIGN4(uiSizeToAllocate));
                }
                // Set new size and mark as used
                pBlocksCopy->uiSize = uiSizeToAllocate;
                pBlocksCopy->isFree = 0;

                pBuffer = get_bufferptr_from_memoryblockptr(pBlocksCopy);
                break;                                  
            } else {
                // There is a next block — we cannot resize the current one
                if (ALIGN4(pBlocksCopy->uiSize) >= uiSizeToAllocate) {
                    pBlocksCopy->isFree = 0;
                    pBuffer = get_bufferptr_from_memoryblockptr(pBlocksCopy);
                    break;                          
                }
            }
        } else {
            // Current block is used — check for next
            if (pBlocksCopy->pNext == nullptr) {
                // Reached the end — try allocating a new block
                if (is_heap_enough(ALIGN4(sizeof(MEMORY_BLOCK)) + ALIGN4(uiSizeToAllocate))) {
                    pBlocksCopy->pNext = pCurrentFreeHeap;
                    increment_current_free_heap(sizeof(MEMORY_BLOCK));

                    MEMORY_BLOCK* pNewMemoryBlock = pBlocksCopy->pNext;
                    pNewMemoryBlock->uiSize = uiSizeToAllocate;
                    pNewMemoryBlock->isFree = 0;    
                    pNewMemoryBlock->pNext = nullptr;

                    increment_current_free_heap(uiSizeToAllocate);
                    pBuffer = get_bufferptr_from_memoryblockptr(pNewMemoryBlock);
                    break;
                } else {
                    pBuffer = nullptr;
                    break;
                }
            } else {
                // Continue to next block
                pBlocksCopy = pBlocksCopy->pNext;
                continue;
            }
        }
    } 
    return pBuffer;
}

// Frees a buffer using its pointer
void kfree(PVOID pBuffer) {
    MEMORY_BLOCK* pMemoryBlock = get_memoryblockptr_from_bufferptr(pBuffer);
    pMemoryBlock->isFree = 1;   // Mark block as free
}

// Fills pBuffer with uiValue, repeated uiSize times
void kmemset(PVOID pBuffer, uint8_t uiValue, uint32_t uiSize) {
    // Prepare 1DW value for optimized writing
    uint32_t uiValue32 = (uint32_t)uiValue;
    uiValue32 |= uiValue32 << 8;
    uiValue32 |= uiValue32 << 16;

    uint32_t uiDwSize = uiSize >> 2;        // Number of DWORDs
    uint32_t uiBRestSize = uiSize & 0x3;    // Remaining bytes

    if (uiDwSize != 0) {
        __asm__ volatile (
            "cld\n\t" 
            "rep stosl"
            : 
            : "a"(uiValue32), "D"(pBuffer), "c"(uiDwSize)
            : "memory"
        );
    }
    
    if (uiBRestSize != 0) {
        PVOID pTail = (uint8_t*)pBuffer + (uiDwSize << 2);
        __asm__ volatile (
            "cld\n\t"
            "rep stosb"
            :
            : "a"(uiValue32), "D"(pTail), "c"(uiBRestSize)
            : "memory"
        );
    }
}