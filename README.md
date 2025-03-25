# **🖥️ FOS: Operating System Project (2024)**  

### **📌 Overview**  
**FOS (2024)** is an **educational operating system project** developed as part of an **Operating Systems course**. It focuses on implementing **core OS functionalities** such as **memory management, CPU scheduling, process synchronization, and kernel heap management**.  

---

## **🚀 Features & Implemented Functionalities**  

### **📦 Memory Management**  
✔ **Kernel Heap Management** (`kmalloc`, `kfree`, `krealloc`)  
✔ **User Heap Management** (`sys_sbrk`, `allocate_user_mem`)  
✔ **Dynamic Allocator** (**First Fit, Best Fit**)  
✔ **Shared Memory (`smalloc`, `sget`)**  
✔ **Page Fault Handling** (Lazy Allocation & Page Replacement)  

### **⚡ CPU Scheduling**  
✔ **Priority Round Robin (PRR) Scheduler**  
✔ **Quantum-Based Preemptive Scheduling**  
✔ **Aging Mechanism for Starvation Prevention**  

### **🔄 Process Synchronization**  
✔ **Spinlocks & Sleeping Locks**  
✔ **User-Level Semaphores (`wait_semaphore`, `signal_semaphore`)**  

### **⏳ Timer & Interrupt Handling**  
✔ **Kernel Clock (`kclock.c`) for Scheduling**  
✔ **Programmable Interval Timer (PIT) Configuration**  
✔ **Trap Handling & IRQ Handlers**  

---

## **📂 Project Structure & Key Components**  

### **🛠️ Milestone 1 (MS1): Core System Setup & Dynamic Memory Management**  
**Implemented:**  
✅ System Calls & Command Processing  
✅ Dynamic Memory Allocator (**First Fit, Best Fit**)  
✅ Spinlocks & Sleeping Locks for Synchronization  

🔹 **Code Files:**  
- `dynamic_allocator.c` → Implements **heap allocation strategies**  
- `sleeplock.c` → Implements **blocking locks**  
- `spinlock.c` → Implements **mutual exclusion**  

---

### **🖥️ Milestone 2 (MS2): Memory Management & Virtual Memory Handling**  
**Implemented:**  
✅ Kernel Heap (`sbrk`, `kmalloc`, `kfree`)  
✅ User Heap (`sys_sbrk`, `allocate_user_mem`)  
✅ Shared Memory Allocation (`smalloc`, `sget`)  

🔹 **Code Files:**  
- `kheap.c` → Implements **kernel heap management**  
- `chunk_operations.c` → Implements **memory operations**  
- `syscalls.c` → Implements **user heap system calls**  

---

### **⚡ Milestone 3 (MS3): CPU Scheduling & Advanced Memory Management**  
**Implemented:**  
✅ **Page Fault Handling II (Page Replacement)** using **Nth-Chance Clock Algorithm**  
✅ **User-Level Semaphores (`wait_semaphore`, `signal_semaphore`)**  
✅ **Priority-Based Round Robin (PRR) CPU Scheduler**  
✅ **Kernel Clock & Timer Interrupts (`kclock.c`)**  

🔹 **Code Files:**  
- `page_fault_handler.c` → Implements **Nth-Chance Clock Algorithm**  
- `semaphore.c` → Implements **user-level semaphores**  
- `scheduler.c` → Implements **Priority RR Scheduler**  
- `kclock.c` → Implements **CPU quantum handling**
