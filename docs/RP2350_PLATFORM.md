# RP2350 Platform Notes

## Overview

The Raspberry Pi RP2350 is a dual-core ARM Cortex-M33 microcontroller with extensive multiprocessing support. This document covers RP2350-specific considerations for AMP implementation.

## Hardware Specifications

### Processor Cores

- **CPU**: Dual ARM Cortex-M33 @ 150 MHz (default)
- **Architecture**: ARMv8-M with TrustZone
- **FPU**: Single-precision floating-point unit per core
- **MPU**: 8-region Memory Protection Unit per core

### Memory Layout

#### SRAM

| Region | Address Range | Size | Description |
|--------|--------------|------|-------------|
| SRAM0 | 0x20000000 - 0x2003FFFF | 256 KB | Main SRAM bank 0 |
| SRAM1 | 0x20040000 - 0x2007FFFF | 256 KB | Main SRAM bank 1 |
| Total | | 512 KB | Available SRAM |

#### Flash

- **Size**: 2 MB - 16 MB (external QSPI flash)
- **XIP Region**: 0x10000000 - 0x11FFFFFF
- **Execute-in-place** from flash supported

### Inter-Core Communication Hardware

#### SIO (Single-cycle IO)

- **Base Address**: 0xd0000000
- **Core ID Register**: `SIO->CPUID` (0 or 1)
- **FIFOs**: 
  - `SIO_FIFO_ST`: Status register
  - `SIO_FIFO_WR`: Write to other core
  - `SIO_FIFO_RD`: Read from other core
- **Spinlocks**: 32 hardware spinlocks
- **Interrupts**: Per-core interrupt routing

#### Multicore FIFO Protocol

The RP2350 provides a dedicated FIFO for core-to-core communication:

```
Core 0                          Core 1
  |                               |
  | -- Write to FIFO_WR -------> | (appears in FIFO_RD)
  | <------- Write to FIFO_WR -- | (appears in FIFO_RD)
  |                               |
```

## Boot Sequence

### Hardware Reset

1. **Power-on**: Both cores are held in reset
2. **Boot ROM**: Executed on Core 0 only
3. **Flash Boot**: Core 0 loads and executes application
4. **Core 1**: Remains in low-power wait state

### Software Core 1 Boot

The recommended boot sequence for Core 1:

```c
// Core 0 boots Core 1
void boot_core1(void (*entry)(void)) {
    // 1. Drain FIFOs
    while (multicore_fifo_rvalid()) {
        multicore_fifo_pop_blocking();
    }
    
    // 2. Write sequence to Core 1
    uint32_t cmd = 1;  // Boot command
    multicore_fifo_push_blocking(cmd);
    multicore_fifo_push_blocking((uint32_t)entry);
    multicore_fifo_push_blocking((uint32_t)stack_ptr);
    
    // 3. Wait for acknowledgment
    uint32_t ack = multicore_fifo_pop_blocking();
}

// Core 1 startup (in bootrom or custom startup)
void core1_entry_wrapper(void) {
    // Read entry point and stack from FIFO
    uint32_t entry = multicore_fifo_pop_blocking();
    uint32_t stack = multicore_fifo_pop_blocking();
    
    // Send acknowledgment
    multicore_fifo_push_blocking(1);
    
    // Setup stack and jump to entry
    __asm__ volatile (
        "mov sp, %0\n"
        "bx %1\n"
        : : "r"(stack), "r"(entry)
    );
}
```

## Shared Memory Recommendations

### Memory Regions for AMP

#### Recommended Layout

```
0x20000000 - 0x2003FFFF (256 KB) - Core 0 private (SRAM0)
  ├─ 0x20000000 - 0x20007FFF: Core 0 stack & data (32 KB)
  └─ 0x20008000 - 0x2003FFFF: Core 0 heap (224 KB)

0x20040000 - 0x2007FFFF (256 KB) - Shared & Core 1 (SRAM1)
  ├─ 0x20040000 - 0x20043FFF: Shared memory pool (16 KB)
  ├─ 0x20044000 - 0x2004BFFF: Core 1 stack & data (32 KB)
  └─ 0x2004C000 - 0x2007FFFF: Core 1 heap (208 KB)
```

### Cache Coherency

**RP2350 does not have data caches**, which simplifies AMP:
- No cache coherency protocol needed
- Memory barriers sufficient for ordering
- Shared memory immediately visible to all cores

### Memory Barriers

Use ARM Cortex-M33 barrier instructions:

```c
// Data Memory Barrier - ensures memory access ordering
__asm__ volatile("dmb" ::: "memory");

// Data Synchronization Barrier - ensures all memory accesses complete
__asm__ volatile("dsb" ::: "memory");

// Instruction Synchronization Barrier - flushes pipeline
__asm__ volatile("isb" ::: "memory");
```

## Hardware Spinlocks

The RP2350 provides 32 hardware spinlocks.

### Usage

```c
#define SPINLOCK_BASE 0xd0000000 + 0x100  // SIO + offset
#define SPINLOCK(n) (*(volatile uint32_t *)(SPINLOCK_BASE + (n) * 4))

// Acquire spinlock
while (!SPINLOCK(0)) {
    // Spin
}

// Critical section
// ...

// Release spinlock  
SPINLOCK(0) = 0;
```

### Integration with AMP

Hardware spinlocks can enhance the AMP implementation:
- Use for semaphore implementation
- Protect shared data structures
- Reduce latency vs. software atomics

## Clock Configuration

### Symmetric Clocking

Both cores should run at the same frequency:

```c
// Set both cores to 150 MHz (default)
set_sys_clock_khz(150000, true);
```

### Considerations

- **Deterministic timing**: Same clock ensures predictable inter-core timing
- **Power consumption**: Can reduce frequency for lower power
- **PLL sharing**: Both cores share the same PLL

## Interrupts and Events

### NVIC (Nested Vectored Interrupt Controller)

Each core has its own NVIC:
- **Core 0 NVIC**: Manages Core 0 interrupts
- **Core 1 NVIC**: Manages Core 1 interrupts

### Inter-Core Interrupts

Cores can signal each other via:

1. **FIFO interrupts**: Triggered when FIFO has data
2. **Software interrupts**: Custom signaling mechanism
3. **Shared peripheral interrupts**: Route to specific core

### Example FIFO Interrupt

```c
// Core 0 - Enable FIFO RX interrupt
irq_set_enabled(SIO_IRQ_PROC0, true);

// Interrupt handler
void sio_irq_handler(void) {
    while (multicore_fifo_rvalid()) {
        uint32_t data = multicore_fifo_pop_blocking();
        // Handle message
    }
}
```

## Debugging

### Core Identification

```c
uint32_t core_id = sio_hw->cpuid;  // 0 or 1
```

### Debug Access

- **SWD**: Both cores accessible via Serial Wire Debug
- **Halt**: Can halt cores independently
- **Breakpoints**: Per-core breakpoint configuration

### Common Issues

1. **Cache coherency**: Not applicable (no data cache)
2. **Stack overflow**: Monitor stack usage per core
3. **Deadlock**: Use timeout mechanisms in IPC
4. **Memory conflicts**: Proper memory partitioning essential

## Power Management

### Core 1 Power Down

Core 1 can be powered down when not needed:

```c
void core1_shutdown(void) {
    multicore_reset_core1();
    // Core 1 enters low-power state
}
```

### Wake-up

Core 1 can be re-booted using the standard boot sequence.

## Performance Characteristics

### Typical Latencies (150 MHz)

- **Spinlock acquire**: ~10-20 cycles (no contention)
- **FIFO write**: ~5 cycles
- **FIFO read**: ~5 cycles
- **Memory barrier (DMB)**: ~1-2 cycles
- **Inter-core message**: ~100-200 ns

### Bandwidth

- **Shared memory**: Full memory bandwidth (limited by SRAM, not cores)
- **FIFO**: 8 entries × 32 bits, ~1 GB/s theoretical

## Integration with Pico SDK

### Using with Pico SDK

The AMP runtime can be integrated with the Raspberry Pi Pico SDK:

```cmake
# In CMakeLists.txt
include(pico_sdk_import.cmake)
pico_sdk_init()

add_executable(my_amp_app
    my_app.c
)

target_link_libraries(my_amp_app
    amp-runtime
    pico_stdlib
    pico_multicore
)
```

### SDK Functions

The Pico SDK provides helpful multicore functions:
- `multicore_launch_core1(entry_func)`
- `multicore_fifo_push_blocking(data)`
- `multicore_fifo_pop_blocking()`
- `multicore_reset_core1()`

## References

- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [ARM Cortex-M33 Technical Reference Manual](https://developer.arm.com/documentation/100230/)

## Example Memory Map

```
┌─────────────────────────────────────┐ 0x00000000
│  ROM (Boot ROM)                     │
├─────────────────────────────────────┤ 0x10000000
│  XIP (Flash)                        │
├─────────────────────────────────────┤ 0x20000000
│  SRAM0 - Core 0 Private            │
│    - Stack & .data & .bss          │
│    - Core 0 heap                    │
├─────────────────────────────────────┤ 0x20040000
│  SRAM1 - Shared & Core 1           │
│    - Shared Memory (IPC)           │
│    - Core 1 Stack & .data          │
│    - Core 1 heap                    │
├─────────────────────────────────────┤ 0x50000000
│  Peripherals                        │
├─────────────────────────────────────┤ 0xd0000000
│  SIO (Single-cycle I/O)            │
│    - Core ID, FIFOs, Spinlocks     │
├─────────────────────────────────────┤ 0xe0000000
│  Cortex-M33 Peripherals            │
│    - NVIC, SysTick, Debug          │
└─────────────────────────────────────┘ 0xffffffff
```
