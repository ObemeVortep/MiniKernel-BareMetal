[BITS 32]

start:
    ; Set up segment registers (GDT is already loaded by bootloader)
    mov ax, 0x10              ; Load data segment selector (from GDT) into AX
    mov ds, ax                ; Assign segment registers (can't load immediate values directly)
    mov es, ax
    mov ss, ax
    mov esp, 0x90000          ; Initialize stack pointer (stack grows downward)

    ; Copy kernel to high memory
    ; ESI = source address, EDI = destination, CLD = clear direction flag (forward copy), 
    ; ECX = number of bytes to copy, MOVSB = copy byte from [ESI] to [EDI]
    mov esi, 0x00008000       ; Source address: where the kernel is currently loaded
    mov edi, 0x00100000       ; Destination address: where we want to copy the kernel
    mov ecx, 0x1000           ; Number of bytes to copy (must match kernel size!)

    cld                       ; Clear direction flag (copy forward)
    rep movsb                 ; Repeat MOVSB ECX times

    mov eax, 0x00100000       ; Jump to kernel entry point
    jmp eax

hang:
    jmp hang                  ; Infinite loop to prevent falling into random memory

times 512 - ($ - $$) db 0     ; Fill remaining space with zeros to complete 512-byte sector