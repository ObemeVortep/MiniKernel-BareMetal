#ifndef _KDEBUG_H        // Kernel Debug
#define _KDEBUG_H

// Clears the terminal screen and resets the cursor to the top-left position
void kclear_terminal(void);

// Prints a single character to the screen and advances the cursor
// Automatically wraps or resets if the edge of the screen is reached
void kputchar(char c);

// Prints a null-terminated string to the screen using kputchar internally
void kprint_string(const char* s);

// Example test threads that print different characters with delay
void thread_test1(void);
void thread_test2(void);
void thread_test3(void);

#endif // _KDEBUG_H