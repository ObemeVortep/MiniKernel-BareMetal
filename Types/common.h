#ifndef _COMMON_H
#define _COMMON_H

// Common type definitions and macros used across the kernel

// Compiler hints
#define FORCE_INLINE __attribute__((always_inline)) inline

// Fixed-width unsigned integer types
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long      uint64_t;

// Generic pointer type
typedef void*              PVOID;

// Boolean values
#define true               1
#define false              0

// Null pointer definitions
#define nullptr            0
#define NULL               0

#endif // _COMMON_H