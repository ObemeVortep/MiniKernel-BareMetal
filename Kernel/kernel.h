#ifndef _KERNEL_H
#define _KERNEL_H

#include "Types/common.h"           // Core type definitions
#include "Memory/kmemory.h"         // Memory management functions
#include "Interrupts/IDT/idt.h"     // Interrupt Descriptor Table (IDT) and interrupt handling
#include "Threads/kthreads.h"       // Thread management and scheduling
#include "Debug/kdebug.h"           // Debug output via VGA text mode

#endif // _KERNEL_H