#ifndef _THREADS_TYPEDEFS_H
#define _THREADS_TYPEDEFS_H

// Active state of a thread
typedef enum {
    RUNNING     = 0,    // Currently running
    WAIT        = 1,    // Ready but waiting
    SLEEP       = 2,    // Sleeping for a period
    NOT_INITED  = 32    // Not yet initialized
} thread_state_t;

// Saved context of a thread when it leaves the RUNNING state
typedef struct _THREAD_CONTEXT {
    void* esp;          // ESP at the time the thread is switched out
    /*  EFLAGS/CS/EIP/REGS are saved on the stack in the following order (from bottom to top of the stack):
    *   | EFLAGS |
    *   | CS     |
    *   | EIP    |
    *   | REGS   |
    *   These values are restored from the stack using popad -> iretd
    */
} THREAD_CONTEXT;

// Thread structure and its state
typedef struct _THREAD {
    THREAD_CONTEXT context;     // Thread context. Updated only when leaving the RUNNING state.
    thread_state_t state;       // Current state of the thread
} THREAD;

#endif // _THREADS_TYPEDEFS_H
