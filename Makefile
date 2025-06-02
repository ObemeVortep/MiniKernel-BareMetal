BOOTLOADER       = Bootloader/bootloader.asm
PM_ENTRY         = Bootloader/pm_entry.asm
KERNEL           = Kernel/kernel.c
KMEMORY          = Memory/kmemory.c
KPORTS           = Ports/kports.c
IDT              = Interrupts/IDT/idt.c
PIC              = Interrupts/PIC/pic.c
PIT              = Interrupts/PIT/pit.c
IRQ_HANDLERS     = Interrupts/IRQ/irq_handlers.asm
KTHREADS         = Threads/kthreads.c
KTHREAD_CREATE   = Threads/kthread_create.asm
SCHEDULER        = Threads/Scheduler/scheduler.c
KSLEEP           = Threads/Scheduler/ksleep.asm
KDEBUG           = Debug/kdebug.c

INCLUDES         = -I.

AS               = nasm
CC               = i686-elf-gcc
LD               = i686-elf-ld
OBJCOPY          = i686-elf-objcopy

ASFLAGS          = -f elf32
CFLAGS           = -m32 -O2 -ffreestanding -nostdlib -c $(INCLUDES)
LDFLAGS          = -T linker.ld -m elf_i386

all: Build/os-image.img

# 1) Build two-stage bootloader (16-bit and 32-bit), both in flat binary format
Build/bootloader.bin: $(BOOTLOADER)
	$(AS) -f bin $< -o $@
Build/pm_entry.bin: $(PM_ENTRY)
	$(AS) -f bin $< -o $@

# 2) Compile 32-bit object files
# Kernel main module
Build/kernel.o: $(KERNEL)
	$(CC) $(CFLAGS) $< -o $@

# Memory management
Build/kmemory.o: $(KMEMORY)
	$(CC) $(CFLAGS) $< -o $@

# I/O port control
Build/kports.o: $(KPORTS)
	$(CC) $(CFLAGS) $< -o $@

# Interrupt handling and dependencies
Build/idt.o: $(IDT)
	$(CC) $(CFLAGS) $< -o $@
Build/pic.o: $(PIC)
	$(CC) $(CFLAGS) $< -o $@
Build/pit.o: $(PIT)
	$(CC) $(CFLAGS) $< -o $@
Build/irq_handlers.o: $(IRQ_HANDLERS)
	$(AS) $(ASFLAGS) $< -o $@

# Thread management and scheduler
Build/kthreads.o: $(KTHREADS)
	$(CC) $(CFLAGS) $< -o $@
Build/kthread_create.o: $(KTHREAD_CREATE)
	$(AS) $(ASFLAGS) $< -o $@
Build/scheduler.o: $(SCHEDULER)
	$(CC) $(CFLAGS) $< -o $@
Build/ksleep.o: $(KSLEEP)
	$(AS) $(ASFLAGS) $< -o $@

# Debugging module
Build/kdebug.o: $(KDEBUG)
	$(CC) $(CFLAGS) $< -o $@

# 3) Link all object files into a single ELF executable (with symbol table)
Build/kernel.elf: Build/kernel.o Build/kmemory.o Build/kports.o Build/idt.o Build/pic.o Build/pit.o Build/irq_handlers.o Build/kthreads.o Build/kthread_create.o Build/scheduler.o Build/ksleep.o Build/kdebug.o
	$(LD) $(LDFLAGS) -o $@ $^

# 4) Extract a flat binary from the ELF (stripped of headers)
Build/kernel.bin: Build/kernel.elf
	$(OBJCOPY) -O binary $< $@

# 5) Combine bootloader, protected mode stub, and kernel into a final image
Build/os-image.img: Build/bootloader.bin Build/pm_entry.bin Build/kernel.bin
	cat $^ > $@

# Run in QEMU
run: Build/os-image.img
	qemu-system-i386 -drive format=raw,file=$<

# Clean build files
clean:
	rm -f Build/*.o Build/*.bin Build/*.elf Build/*.img
