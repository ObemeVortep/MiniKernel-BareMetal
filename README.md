# 🔧 Low-Level Kernel Prototype

> A personal learning project focused on OS internals, protected mode, and threading from scratch.

---

## 📖 Background

After completing my first major programming project (5 months of development), I found my motivation for learning programming starting to wane. I had just finished writing **FSParser** and studying the STL, so I decided to take a short break — waiting for a new idea to spark my interest.

In late February, I returned with a fresh focus: diving into **SystemVerilog**. This led to a large project (now 15k+ LOC, developed solo), involving deep low-level access and an entirely new type of development.

During that time, I encountered a task requiring **Ring -1 access** — below the hypervisor level. It intrigued me, and I began building a prototype hypervisor-like kernel to explore the internals of the CPU, assembly, and operating systems.

---

## ⏱️ 10 Days of Deep Dive

Over 10 days, I spent 6–10 hours per day researching and building this kernel. While I ultimately decided against continuing the full hypervisor (due to its complexity), the experience I gained was invaluable.

---

## 🗂️ Project Structure

Each folder in the project serves a distinct purpose within the kernel:

### `Bootloader/`
- Two `.asm` files responsible for:
  - Loading the kernel into memory
  - Switching from **real mode** to **protected mode**
  - Transferring control to the kernel

### `Interrupts/`
- Handles hardware interrupts, timers, and exceptions
- Organized into:
  - `IDT` (Interrupt Descriptor Table)
  - `IRQ` (Interrupt Requests)
  - `PIC` (Programmable Interrupt Controller)
  - `PIT` (Programmable Interval Timer)

### `Debug/`
- Outputs debug symbols using **VGA text mode**
- Includes tools for:
  - Visual debugging
  - Multithreading stress tests

### `Kernel/`
- The central kernel module
- Initializes other subsystems
- Originally planned to host the hypervisor core logic

### `Memory/`
- Simple block-based RAM allocator
- No paging or advanced memory management yet

### `Ports/`
- Provides convenient access to I/O ports
- A thin wrapper around `inb` / `outb` instructions

### `Threads/`
- Most complex module
- Implements:
  - Thread creation and destruction
  - Thread sleeping and context switching
  - A **Task Scheduler** for cooperative multithreading

### `Types/`
- Shared type definitions across the codebase

---

## 🧠 What I Learned

Although I stopped short of building a full hypervisor, these 10 days gave me:

- A deeper understanding of how CPUs operate at the lowest levels
- Practical knowledge of writing and debugging assembly code
- Insights into real/protected modes, interrupts, memory handling, and multithreading

---

## 📌 Final Thoughts

This was a short but intense journey into the world of operating systems and low-level development. I may return to this project in the future — but even as-is, it stands as a milestone in my growth as a programmer.

Thanks for reading! 🙌
