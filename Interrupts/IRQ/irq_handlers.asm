[BITS 32]
[GLOBAL irq0_handler]
[EXTERN uiPitTicks]          ; uint32_t – tick counter incremented by PIT
[EXTERN awake_threads]      ; Function to wake sleeping threads
[EXTERN switch_thread]      ; Function to perform thread switching

; This file contains all IRQ interrupt handlers

; IRQ0 Handler (PIT interrupt, mapped to vector 0x20 in the IDT)
irq0_handler:
    ; Increment the PIT tick counter
    inc dword [uiPitTicks]

.awake_threads:
    pushad
    call awake_threads       ; Check if any sleeping threads need to be resumed
    popad

    ; Every 1024 ticks (0x400), perform a context switch
    test dword [uiPitTicks], 0x3FF  ; Check if lower 10 bits are zero (i.e. tick % 1024 == 0)
    jnz .end_of_irq          ; If not the 1024th tick, skip context switch

.switch_thread:
    pushad                   ; Save all general-purpose registers

    mov eax, esp             ; Save current stack pointer into EAX
    push 1                   ; Push new state (1 = WAIT) for current thread
    push eax                 ; Push current ESP
    call switch_thread       ; Attempt thread switch – returns new ESP or same if no switch
    mov esp, eax             ; Restore ESP (may point to a new thread’s context)

    popad                    ; Restore registers (may belong to a new thread!)

.end_of_irq:
    ; Send End-of-Interrupt (EOI) to PIC (Master)
    mov al, 0x20             ; 0x20 = EOI command
    out 0x20, al             ; Send to PIC command port (0x20)

    iretd                    ; Return from interrupt (restores EIP, CS, EFLAGS)