#include "Types/common.h"
#include "Threads/Scheduler/typedefs.h"
#include "Threads/general_threads.h"
#include "Interrupts/PIT/pit.h"

/*
*   This .c file is responsible for proper initialization and operation of the Scheduler.
*/

// Index of the currently running thread in the `threads` array
static uint8_t uiCurrentRunningThread = 0;          // Initially, thread 0 is running

// Structure representing sleeping threads. Used to properly track which threads are sleeping and when they should wake up
static SLEEPING_THREADS sleepingThreads = { 0 };   // Initially, no threads are sleeping

// Initializes the scheduler
void init_scheduler() {
    // Initialize the currently running thread
    uiCurrentRunningThread = 0;
}

// Checks if the current thread is allowed to go to sleep. Returns 0 if not possible, 1 if allowed.
uint8_t is_ksleep_possible() {
    if ((sleepingThreads.uiCount + 1) == uiTotalInitedThreads) {
        return 0;
    } else {
        return 1;
    }
}

/*  Adds the current thread to the sleepingThreads list and calculates
*   the tick count it should wake up at.
*   Must be called only if is_ksleep_possible == 1!
*/
void init_sleep(uint32_t ms) {
    // Add the thread to the sleeping list and set its wake-up tick
    sleepingThreads.uiThreadIndexes[sleepingThreads.uiCount]    = uiCurrentRunningThread; 
    sleepingThreads.uiTickToAwake[sleepingThreads.uiCount]      = get_pit_ticks() + ms;

    // Increment the count of sleeping threads
    sleepingThreads.uiCount++;
}

// Removes a thread from the sleeping list at the given index
static void delete_sleeping_thread(uint8_t index) {
    // Move the last entry into the current index and decrement the count
    sleepingThreads.uiCount--;
    sleepingThreads.uiThreadIndexes[index]  = sleepingThreads.uiThreadIndexes[sleepingThreads.uiCount];
    sleepingThreads.uiTickToAwake[index]    = sleepingThreads.uiTickToAwake[sleepingThreads.uiCount];
}

// Wakes threads when their sleep duration has elapsed (based on PIT timer)
void awake_threads() {
    uint32_t uiCurrentTick = get_pit_ticks();

    // Iterate only through currently sleeping threads (not all 255)
    for (uint8_t i = 0; i < sleepingThreads.uiCount; i++) {
        // Check if the thread should be woken up at the current tick
        if (sleepingThreads.uiTickToAwake[i] <= uiCurrentTick) {
            // Yes — wake the thread
            uint8_t uiThreadIndex = sleepingThreads.uiThreadIndexes[i];
            change_thread_state(uiThreadIndex, WAIT);    
            
            // Remove the thread from the sleep pool and adjust index
            delete_sleeping_thread(i);
            i--;    // Decrement i to recheck this position (as the last element moved here)
        }
    }
}

/*  Switches execution to a different thread. Returns the ESP of the new thread,
*   or the current ESP if no thread switch is needed.
*   Currently triggered by IRQ0 (PIT interrupt every ms), called every 16 ticks.
*/ 
uint32_t switch_thread(const uint32_t uiCurrentEsp, const uint8_t uiNewState) {
    if (uiTotalInitedThreads == 1) {
        // Only one thread is active — nothing to switch
        return uiCurrentEsp;
    }

    /*
    *   Thread switching process (3 steps):
    *   1. Find the next thread in WAIT state that can run
    *   2. Save the context of the current thread, set its state to uiNewState
    *   3. Update the next thread to RUNNING and return its ESP
    */  

    // Look for a thread in WAIT state
    uint8_t uiWaitThread = uiCurrentRunningThread + 1;
    do {
        if (threads[uiWaitThread].state == WAIT) {
            break;  // Found a WAIT thread
        }
        uiWaitThread++;
    } while (uiWaitThread != uiCurrentRunningThread);  // Loop until we're back to the current thread

    // If no other WAIT thread was found, return current ESP
    if (uiWaitThread == uiCurrentRunningThread) {
        return uiCurrentEsp;
    } 
    
    // Save current thread's context and update its state
    change_thread_context(uiCurrentRunningThread, (THREAD_CONTEXT){uiCurrentEsp});
    change_thread_state(uiCurrentRunningThread, uiNewState);  

    // Update uiCurrentRunningThread to the new thread and set it to RUNNING
    uiCurrentRunningThread = uiWaitThread;
    change_thread_state(uiCurrentRunningThread, RUNNING);   

    // Return ESP of the newly running thread
    return threads[uiCurrentRunningThread].context.esp;
}