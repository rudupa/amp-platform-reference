# AMP Contract Specification

## Overview

This document defines the Asymmetric Multiprocessing (AMP) contract for dual-core MCU systems. The contract establishes the minimal interface and guarantees required for deterministic operation of multiple cores with separate runtime domains.

## Version

Phase 1 - Version 1.0.0

## Core Concepts

### 1. Domain Isolation

Each core operates in its own domain with:
- Dedicated memory regions
- Independent execution context
- Separate stack space
- Defined entry point

### 2. Shared Resources

Cores communicate through:
- Shared memory regions
- Inter-Process Communication (IPC) primitives
- Hardware synchronization mechanisms

## Boot Sequence

### Boot Order

1. **Primary Core (Core 0)**
   - Initializes first (hardware reset)
   - Configures shared memory regions
   - Initializes IPC primitives
   - Boots secondary cores

2. **Secondary Core(s) (Core 1+)**
   - Remain in reset/idle state initially
   - Boot when triggered by primary core
   - Signal readiness after initialization
   - Begin application-specific work

### Boot Protocol

```c
/* Primary Core */
amp_boot_init();                    // Initialize boot system
amp_shmem_init(base, size);         // Setup shared memory
amp_boot_core(CORE1, entry, sp);    // Boot secondary core
amp_boot_wait_core_ready(CORE1);    // Wait for ready signal

/* Secondary Core */
amp_boot_signal_ready();            // Signal initialization complete
```

## Domain Configuration

### Memory Regions

Each domain must define:

```c
typedef struct {
    uint32_t base;      // Base address
    uint32_t size;      // Size in bytes
    uint32_t flags;     // Access permissions (R/W/X)
} amp_memory_region_t;
```

### Domain Configuration Structure

```c
typedef struct {
    amp_core_t core_id;
    uint32_t stack_base;
    uint32_t stack_size;
    uint32_t entry_point;
    amp_memory_region_t *regions;
    uint32_t region_count;
} amp_domain_config_t;
```

## Shared Memory

### Initialization

```c
int amp_shmem_init(void *base, size_t size);
```

- Must be called by primary core before booting secondary cores
- Defines the shared memory pool accessible by all cores
- Memory must be in non-cacheable or cache-coherent region

### Allocation

```c
void *amp_shmem_alloc(size_t size);
```

- Simple bump allocator
- Thread-safe for multi-core access
- 8-byte alignment guaranteed
- No individual free support (Phase 1 limitation)

### Access Guarantees

- All cores have symmetric read/write access
- Memory barriers required for ordering
- No hardware cache coherency assumed

## Inter-Process Communication (IPC)

### 1. Mailbox

Fixed-size message passing between cores.

**Properties:**
- Bounded message slots (power-of-2)
- Fixed message size per mailbox
- FIFO ordering
- Blocking and non-blocking modes

**Usage Pattern:**
```c
amp_mailbox_t mbox = amp_mailbox_create(&config);
amp_mailbox_send(mbox, &msg, timeout);
amp_mailbox_recv(mbox, &msg, timeout);
```

**Synchronization:**
- Lock-free ring buffer implementation
- Memory barriers on read/write index updates
- Busy-wait for blocking operations

### 2. Semaphore

Counting semaphore for resource synchronization.

**Properties:**
- Atomic increment/decrement
- Maximum count limit
- Timeout support
- No priority inheritance (Phase 1)

**Usage Pattern:**
```c
amp_semaphore_t sem = amp_semaphore_create(initial, max);
amp_semaphore_wait(sem, timeout);    // Decrement
amp_semaphore_post(sem);             // Increment
```

**Synchronization:**
- Compare-and-swap atomics
- Memory barriers on count updates

### 3. Ring Buffer

Lock-free streaming data buffer.

**Properties:**
- Power-of-2 size requirement
- Single producer / single consumer
- Variable-length reads/writes
- Non-blocking operations

**Usage Pattern:**
```c
amp_ringbuf_t rb = amp_ringbuf_create(size);
amp_ringbuf_write(rb, data, len);
amp_ringbuf_read(rb, data, len);
```

**Synchronization:**
- Separate read/write indices
- Memory barriers on index updates
- Returns actual bytes transferred

## Memory Ordering

### Requirements

All IPC primitives guarantee:
- **Data Memory Barrier (DMB)** after writes before index updates
- **DMB** after index reads before data access
- Proper compiler barriers to prevent reordering

### Example

```c
/* Producer */
memcpy(buffer, data, size);          // Write data
__asm__ volatile("dmb" ::: "memory");  // Memory barrier
write_index++;                       // Update index

/* Consumer */
uint32_t idx = read_index;           // Read index
__asm__ volatile("dmb" ::: "memory");  // Memory barrier
memcpy(data, buffer, size);          // Read data
```

## Error Handling

### Return Codes

- `0` - Success
- `-1` - Generic error
- Specific error codes for boot operations

### Timeout Values

- `0` - No timeout (infinite wait)
- `>0` - Timeout in milliseconds
- Timeout mechanism is platform-dependent

## Platform Requirements

### Minimum Requirements

1. **Dual-core processor**
   - Separate execution contexts
   - Shared memory space

2. **Atomic Operations**
   - Load-Exclusive / Store-Exclusive (LDREX/STREX)
   - Or equivalent atomic primitives

3. **Memory Barriers**
   - Data Memory Barrier (DMB)
   - Or equivalent ordering guarantees

4. **Shared Memory**
   - At least 16KB recommended
   - Non-cacheable or cache-coherent

### Recommended Features

- Hardware semaphores
- Inter-core interrupts
- DMA support
- Memory protection unit (MPU)

## Limitations (Phase 1)

1. **No dynamic memory reclaim** - Shared memory uses bump allocator
2. **No priority mechanisms** - Simple FIFO ordering
3. **No multi-producer/multi-consumer** - IPC assumes specific producer/consumer
4. **Limited error handling** - Basic error codes only
5. **No runtime core affinity changes** - Static core assignment

## Future Extensions (Post Phase 1)

- Dynamic memory management
- Priority-based scheduling
- Multi-producer/multi-consumer queues
- Advanced error recovery
- Runtime performance monitoring
- Power management integration

## References

- ARM Cortex-M33 Technical Reference Manual
- Raspberry Pi RP2350 Datasheet
- Memory Ordering in Modern Microprocessors (Paul McKenney)
