#include "Types/common.h"
#include "Debug/kdebug.h"

/*
*   This module handles debug output for our system.
*   It contains utility functions for terminal output and testing.
*/

#define VGA_ADDRESS 0xB8000     // VGA memory address for text mode output
#define VGA_WIDTH   80          // Max number of characters per line
#define VGA_HEIGHT  25          // Max number of lines on screen

static uint16_t* pVGABuffer = (uint16_t*)VGA_ADDRESS;
static uint8_t uiCursorRow = 0;
static uint8_t uiCursorCol = 0;

// Clears the entire terminal and resets cursor position
void kclear_terminal(void) {
    for (int i = 0; i < VGA_HEIGHT; i++) {
        for (int j = 0; j < VGA_WIDTH; j++) {
            pVGABuffer[i * VGA_WIDTH + j] = (0x00 << 8) | '\0';
        }
    }
    uiCursorRow = 0;     // Reset cursor row
    uiCursorCol = 0;     // Reset cursor column
}

// Outputs a single character to the terminal and updates the cursor
void kputchar(char c) {
    if (c == '\n') {
        uiCursorRow++;       // Move to next line on newline
        uiCursorCol = 0;     // Reset column to start of line
    } else {
        // Calculate buffer position
        uint16_t uiPosition = uiCursorRow * VGA_WIDTH + uiCursorCol;

        // Write character with attribute byte: 0x0F = white on black
        pVGABuffer[uiPosition] = (0x0F << 8) | c;

        uiCursorCol++;       // Move cursor to the right

        // Wrap to next line if end of line reached
        if (uiCursorCol >= VGA_WIDTH) {
            uiCursorCol = 0;
            uiCursorRow++;
        }
    }

    // Wrap back to top if bottom of screen reached
    if (uiCursorRow >= VGA_HEIGHT) {
        uiCursorRow = 0;
    }
}

// Prints a null-terminated string using kputchar internally
void kprint_string(const char* s) {
    for (int i = 0; s[i] != '\0'; i++) {
        kputchar(s[i]);
    }
}

// Example test threads that print different characters with delay
void thread_test1(void) {
    while (1) {
        ksleep(1000);
        kprint_string("1");
    }
}
void thread_test2(void) {
    while (1) {
        ksleep(2000);
        kprint_string("2");
    }
}
void thread_test3(void) {
    while (1) {
        ksleep(3000);
        kprint_string("3");
    }
}