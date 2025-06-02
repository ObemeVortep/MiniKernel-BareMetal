[BITS 32]
[GLOBAL ksleep]
[EXTERN is_ksleep_possible]     ; Checks if sleeping is currently possible
[EXTERN init_sleep]             ; Updates the SLEEPING_THREADS list by adding the current thread
[EXTERN switch_thread]          ; Switches to a different thread

; This function saves EFLAGS, CS, EIP, and REGISTERS on the stack (as expected by irq0_handler)
; Keep in mind that when the sleeping thread is resumed via irq0_handler, the context will be restored in this order:
; popad -> pop eip -> pop cs -> pop eflags 
; Therefore, we must push the context to the stack in the exact same order
; ksleep has one argument — a uint32_t value representing the number of milliseconds the thread should sleep
; void ksleep(uint32_t ms);
ksleep:
    ; First, check whether the current thread can go to sleep
    call is_ksleep_possible     ; Returns 0 if sleeping is not allowed
    test eax, eax               ; Check for 0
    jz .nosleep                 ; Jump if sleep is not allowed

    ; If we are here, sleeping is allowed. Prepare the stack and call switch_thread with new_state = 2 (SLEEP)
.init_sleep:
    mov eax, [esp+4]            ; Retrieve the first argument
    push eax                    ; Call init_sleep with the sleep duration in ms
    call init_sleep
    add esp, 4                  ; Clean up the stack after the call

.sleep:
    ; Save EIP (instruction pointer)
    mov ecx, [esp]              ; EIP is currently at the top of the stack (esp points to it)

    ; Push context to the stack in this order: EFLAGS -> CS -> EIP -> REGS
    pushfd                      ; Save flags
    xor eax, eax
    mov ax, cs                  ; Save code segment (CS)
    push eax
    push ecx                    ; Save EIP
    pushad                      ; Save general-purpose registers
; This is done so that irq0_handler can correctly restore everything in the order:
; popad -> pop eip -> pop cs -> pop eflags

    ; Now, call switch_thread and switch to the new thread
    mov eax, esp                ; Store current ESP in EAX
    push 2                      ; new_state == SLEEP (second argument)
    push eax                    ; current_esp (first argument)
    call switch_thread          ; Switches to another thread, returns new ESP in EAX

    ; Update ESP, restore registers, and return from ksleep into the new thread
    mov esp, eax                ; ESP must now point to the stack structured as:
                                ; | EFLAGS |
                                ; | CS     |
                                ; | EIP    |
                                ; | REGS   |
                                ; Switching threads happens via: popad -> iretd
    popad                       ; Restore general-purpose registers
    iretd                       ; Return from interrupt to the new thread

    ; If we reach here, the thread continues without going to sleep
.nosleep:
    ret