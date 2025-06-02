[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax                  ; Set up segment registers for real mode
    mov sp, 0x7C00              ; Set up stack pointer

    ; Load protected mode entry and kernel into memory
    mov ah, 0x02                ; BIOS function: Read sectors into memory
    mov al, 6                   ; Number of sectors to read | pm_entry(1) + kernel(3) | MUST MATCH kernel.bin size!
    mov ch, 0                   ; Cylinder index
    mov cl, 2                   ; Sector number (starts at 1)
    mov dh, 0                   ; Head
    mov dl, 0x80                ; 0x80 = first HDD
    mov bx, 0x7E00              ; Memory address to load data
    int 0x13                    ; BIOS disk interrupt
    jc disk_error               ; Jump to error handler on failure

    ; No error — continue to protected mode entry
    lgdt [gdt_descriptor]       ; Load Global Descriptor Table

    mov eax, cr0
    or eax, 1
    mov cr0, eax                ; Enable protected mode

    jmp far [pm_jump]           ; Far jump to protected mode entry point

; BIOS error handler: print 'E' to screen
disk_error:
    mov ah, 0x0E                ; BIOS: Teletype output
    mov al, 0x45                ; ASCII 'E'
    int 0x10                    ; Print character
hang:
    jmp hang                    ; Infinite loop

; Far jump pointer: offset and segment of protected mode entry
pm_jump:
    dw 0x7E00                   ; Offset
    dw 0x08                     ; Code segment selector (GDT index 1)

; Global Descriptor Table
gdt_start:
    dq 0x0000000000000000       ; Null segment
    dq 0x00CF9A000000FFFF       ; Code segment
    dq 0x00CF92000000FFFF       ; Data segment
gdt_end:

; GDT descriptor: size and base address
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Base address of GDT

times 510 - ($ - $$) db 0       ; Fill the rest of the 512-byte boot sector with zeros
dw 0xAA55                       ; Boot sector signature (magic bytes)