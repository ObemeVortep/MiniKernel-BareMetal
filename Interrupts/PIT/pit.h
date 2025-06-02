#ifndef _INTERRUPTS_PIT_PIT_H
#define _INTERRUPTS_PIT_PIT_H

// Initializes the Programmable Interval Timer (PIT)
void pit_init(void);

// Global PIT tick counter — updated automatically by irq0_handler
extern volatile uint32_t uiPitTicks;

// Returns the current value of uiPitTicks
FORCE_INLINE uint32_t get_pit_ticks(void) {
    return uiPitTicks;
}

#endif // _INTERRUPTS_PIT_PIT_H