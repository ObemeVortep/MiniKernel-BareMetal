#ifndef _THREADS_SCHEDULER_TYPEDEFS_H
#define _THREADS_SCHEDULER_TYPEDEFS_H

typedef struct _SLEEPING_THREADS {
    uint8_t     uiCount;                 // Number of currently sleeping threads
    uint8_t     uiThreadIndexes[255];    // Indices of up to 255 threads that can sleep simultaneously
    uint64_t    uiTickToAwake[255];      // PIT tick count required to wake each sleeping thread
} SLEEPING_THREADS;

#endif // _THREADS_SCHEDULER_TYPEDEFS_H