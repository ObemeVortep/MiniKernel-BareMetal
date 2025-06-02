[BITS 32]
[GLOBAL kthread_create]                ; Function that creates a new thread and adds it to the queue
[EXTERN is_kthread_create_possible]    ; Checks whether a new thread can be created
[EXTERN kmalloc]                       ; Function needed to allocate the stack. Defaults to 4KB.
[EXTERN thread_init]                   ; Final function to initialize internal thread structures.
                                       ; Called after all checks and allocations succeed, returns the index of the newly created thread (which we must return).

; Our thread creation function. Returns the thread index on success, or 0 on failure.
; kthread_create has one argument – the start function where execution should begin.
; uint8_t kthread_create(PVOID pStart);
kthread_create:
    ; First, check if thread creation is allowed with the current parameters
    mov eax, [esp+4]                    ; Load the pStart argument
    push eax
    call is_kthread_create_possible     ; Validates internal structures and ensures pStart != 0
    add esp, 4                          ; Restore the stack
    test eax, eax                       ; Check the result
    jz .return                          ; If zero, return

    ; If eax != 0, pStart is valid and thread creation is allowed. Try allocating the stack.
    mov eax, 0x1000                     ; 4KB
    push eax
    call kmalloc                        ; kmalloc(uint32_t size) returns a pointer to the bottom of the allocated memory or 0 on failure
    add esp, 4                          ; Restore the stack
    test eax, eax                       ; Check the result
    jz .return                          ; If zero, return

    ; To get the actual stack pointer, add 4096 (0x1000) to eax
    add eax, 0x1000                     ; Now eax contains the ESP for the new thread

; Nothing can prevent thread creation beyond this point.
    ; Prepare the stack for the new thread
    mov ecx, [esp+4]                    ; Save pStart (start address of the thread) in ECX
    
    push ebp                            ; Save current base pointer
    mov ebp, esp                        ; Save current ESP to EBP
    mov esp, eax                        ; Temporarily switch to the new thread’s stack

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    pushfd                              ; Save EFLAGS on the new thread’s stack
    xor eax, eax
    mov ax, cs                          ; Save the current code segment (CS)
    push eax
    push ecx                            ; Save EIP (the start function address)
    pushad                              ; Save general-purpose registers (needed for proper irq0_handler context)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    ; Save the new thread's ESP, then switch back to the original ESP.
    mov eax, esp                        ; Save ESP of the new thread in eax
    mov esp, ebp                        ; Restore original ESP
    pop ebp                             ; Restore EBP
    
    ; Call thread_init
    push eax                            ; Pass the new thread’s stack as a parameter
    call thread_init                    ; Initializes the thread, returns its index
    add esp, 4                          ; Restore the stack

.return: 
    ret