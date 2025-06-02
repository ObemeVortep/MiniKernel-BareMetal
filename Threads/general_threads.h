#ifndef _THREADS_GENERAL_THREADS_H
#define _THREADS_GENERAL_THREADS_H

#include "Threads/typedefs.h"

extern uint16_t uiTotalInitedThreads;                   // Current number of initialized threads
extern THREAD   threads[256];                           // Maximum of 256 threads supported

// Sets the execution context of a thread
FORCE_INLINE void change_thread_context(const uint8_t uiThreadIndex, const THREAD_CONTEXT newContext) {
    threads[uiThreadIndex].context = newContext;
}

// Sets the execution state of a thread
FORCE_INLINE void change_thread_state(const uint8_t uiThreadIndex, const thread_state_t newState) {
    threads[uiThreadIndex].state = newState;
}

#endif // _THREADS_GENERAL_THREADS_H