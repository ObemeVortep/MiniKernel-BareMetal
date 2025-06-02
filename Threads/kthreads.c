#include "Types/common.h"
#include "Memory/kmemory.h"
#include "Threads/general_threads.h"
#include "Threads/Scheduler/scheduler.h"    // Scheduler
#include "Threads/general_threads.h"

#define DEFAULT_STACK_SIZE          4096          // Default stack size – 4KB

/*
*   Thread termination logic is currently not implemented.
*/

uint16_t uiTotalInitedThreads = 0;               // Current number of initialized threads
THREAD   threads[256] = { 0 };                   // Maximum of 256 threads

// Initializes all secondary threads as NOT_INITED
static void init_secondary_threads() {
    // Simply initialize each thread starting from index 1 with zeros, and set their state to NOT_INITED
    for (uint16_t i = 1; i < (sizeof(threads) / sizeof(THREAD)); i++) {
        change_thread_context(i, (THREAD_CONTEXT){0});   // Initialize to zeros since it's NOT_INITED. Stack is allocated during create_thread.
        change_thread_state(i, NOT_INITED);              // All 255 threads are marked as NOT_INITED
    }
}

// Properly initializes the main thread
static void init_main_thread() {
    // Add the already running main thread to the thread pool
    change_thread_context(0, (THREAD_CONTEXT){0});   // ESP is set to zero since it's already RUNNING. It will be updated automatically during context switch.
    change_thread_state(0, RUNNING);                 // Main kernel thread is currently RUNNING

    // Increase the total number of initialized threads
    uiTotalInitedThreads = 1;                        // Only 1 thread is currently active
}

// Checks whether a new thread can be created
uint8_t is_kthread_create_possible(PVOID pStart) {
    // Thread creation is not allowed if pStart is invalid or if the maximum number of threads (256) is already reached
    if (uiTotalInitedThreads >= 256 || pStart == nullptr) {
        return 0;
    } else {
        return 1;
    }
}

// Initializes internal structures when creating a new thread.
// Called from kcreate_thread and returns the index of the created thread
uint8_t thread_init(PVOID pStack) {
    // Nothing can prevent thread creation at this point

    // Find the first NOT_INITED thread slot
    uint8_t uiThreadNumber = 1;
    do {
        // Check if the current thread slot can be used
        if (threads[uiThreadNumber].state == NOT_INITED) {
            break;
        }
        uiThreadNumber++;
    } while (uiThreadNumber < 256);

    uiTotalInitedThreads++;   // Increment total initialized thread count

    // Create a new THREAD structure and store it in threads
    change_thread_context(uiThreadNumber, (THREAD_CONTEXT){pStack});    // Assign ESP
    change_thread_state(uiThreadNumber, WAIT);                          // Thread is waiting to start

    return uiThreadNumber;
}

// Initializes thread management and creates the kernel thread
void kthreads_init() {
    // First, initialize the main thread
    init_main_thread();

    // Then, initialize secondary threads
    init_secondary_threads();

    // Finally, initialize the scheduler
    init_scheduler();
}