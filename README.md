# amp-mcu-reference
Vendor‑neutral AMP reference implementation for dual‑core MCUs — deterministic boot, domain config, minimal IPC, and reproducible examples (Phase 1).

## 📋 Task Documentation

This repository contains actionable task descriptions for enhancing and documenting the AMP MCU reference implementation.

### Quick Start
- **[TASK_INDEX.md](./TASK_INDEX.md)** - Master index of all tasks with selection guide

### Available Tasks

1. **Priority-Based Message Scheduling** (Phase 2 Enhancement)
   - **[TASK_DESCRIPTION.md](./TASK_DESCRIPTION.md)** - Complete specification
   - **[TASK_SUMMARY.md](./TASK_SUMMARY.md)** - Quick reference
   - Scope: ~10 hours | Extends existing amp_mailbox with priority support
   - Addresses Phase 1 limitation: "No priority-based scheduling"

2. **Phase 1 Architecture Diagrams** (Documentation)
   - **[TASK_PHASE1_DIAGRAMS.md](./TASK_PHASE1_DIAGRAMS.md)** - Complete specification
   - Scope: ~11 hours | Create visual documentation (PlantUML/SVG)
   - Deliverables: Architecture, boot sequence, memory layout, IPC flow diagrams

3. **Porting Guides for RP2040 and ESP32-S3** (Platform Expansion)
   - **[TASK_PORTING_GUIDES.md](./TASK_PORTING_GUIDES.md)** - Complete specification
   - Scope: ~17 hours | Comprehensive guides for new platforms
   - Deliverables: RP2040 guide, ESP32-S3 guide, generic porting template

### What Already Exists (Phase 1 - Main Branch)
The main branch contains a complete Phase 1 implementation:
- ✅ Core boot and handoff (`amp_boot`)
- ✅ Shared memory ring buffer (`amp_ringbuf`)
- ✅ Mailbox IPC (`amp_mailbox`)
- ✅ Semaphores (`amp_semaphore`)
- ✅ Three working examples (hello-amp, pingpong, shared-counter)

See [TASK_INDEX.md](./TASK_INDEX.md) for full details on what exists vs. what these tasks add.
# AMP MCU Reference Implementation

Vendor-neutral Asymmetric Multiprocessing (AMP) reference implementation for dual-core MCUs — featuring deterministic boot, domain configuration, minimal IPC primitives, and reproducible examples.

**Phase 1** - Foundation release with core functionality.

## Overview

This repository provides a minimal, vendor-neutral AMP framework for dual-core microcontrollers. It focuses on:

- **Deterministic Boot**: Controlled secondary core startup with synchronization
- **Domain Configuration**: Memory region and core assignment management
- **Shared Memory**: Simple allocator for inter-core shared data
- **IPC Primitives**: Mailbox, semaphore, and ring buffer implementations
- **Reference Examples**: Three working examples demonstrating core concepts

### Target Platforms

- **Primary**: Raspberry Pi RP2350 (dual ARM Cortex-M33)
- **Generic**: Portable C implementation for other dual-core ARM Cortex-M MCUs

## Features

### Runtime Components

| Component | Description | Header |
|-----------|-------------|--------|
| **Boot Management** | Core initialization and startup sequencing | `amp_boot.h` |
| **Configuration** | Domain and memory region configuration | `amp_config.h` |
| **Shared Memory** | Simple shared memory allocator | `amp_shmem.h` |
| **Mailbox** | Fixed-size message passing (FIFO) | `amp_mailbox.h` |
| **Semaphore** | Counting semaphore synchronization | `amp_semaphore.h` |
| **Ring Buffer** | Lock-free streaming data buffer | `amp_ringbuf.h` |

### Example Applications

1. **hello-amp**: Basic initialization and communication
2. **pingpong**: Bidirectional message exchange
3. **shared-counter**: Concurrent shared memory access

## Quick Start

### Prerequisites

- **CMake** 3.13 or later
- **ARM GCC Toolchain** (for embedded targets)
- **GCC** (for generic/host builds)
- **Git** (for cloning)

Optional:
- **Raspberry Pi Pico SDK** (for RP2350 platform)

### Building

#### Generic Build (Host Testing)

```bash
# Clone repository
git clone https://github.com/rudupa/amp-mcu-reference.git
cd amp-mcu-reference

# Configure with CMake
cmake -B build -DAMP_PLATFORM=generic

# Build all examples
cmake --build build

# Build specific example
cmake --build build --target hello-amp
```

#### RP2350 Build

```bash
# Set up Pico SDK (if not already done)
export PICO_SDK_PATH=/path/to/pico-sdk

# Configure for RP2350
cmake -B build -DAMP_PLATFORM=rp2350

# Build
cmake --build build

# Output: build/examples/*.uf2 files
```

### Running Examples

#### On RP2350 (Pico 2)

1. Build with RP2350 platform (see above)
2. Connect Pico 2 board via USB while holding BOOTSEL
3. Copy `.uf2` file to the mounted drive:
   ```bash
   cp build/examples/hello-amp.uf2 /media/RPI-RP2/
   ```
4. Monitor serial output (115200 baud):
   ```bash
   screen /dev/ttyACM0 115200
   # or
   minicom -D /dev/ttyACM0 -b 115200
   ```

#### Generic Platform

The generic build produces executables that demonstrate the code structure but require actual dual-core hardware for true multi-core execution.

## Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[AMP_CONTRACT.md](docs/AMP_CONTRACT.md)**: Complete AMP specification and contract
- **[RP2350_PLATFORM.md](docs/RP2350_PLATFORM.md)**: RP2350-specific implementation notes
- **[EXAMPLES.md](docs/EXAMPLES.md)**: Detailed example descriptions and expected output

### API Documentation

All public APIs are documented in header files under `runtime/include/`:

```c
// Example: Boot API
#include "amp_boot.h"

amp_boot_init();                        // Initialize boot system
amp_boot_core(core_id, entry, stack);   // Boot secondary core
amp_boot_wait_core_ready(core_id, ms);  // Wait for ready signal
amp_boot_signal_ready();                // Signal core ready
```

## Architecture

```
amp-mcu-reference/
├── runtime/              # Core AMP runtime library
│   ├── include/          # Public API headers
│   │   ├── amp_boot.h
│   │   ├── amp_config.h
│   │   ├── amp_mailbox.h
│   │   ├── amp_ringbuf.h
│   │   ├── amp_semaphore.h
│   │   └── amp_shmem.h
│   └── src/              # Implementation
│       ├── amp_boot.c
│       ├── amp_config.c
│       ├── amp_mailbox.c
│       ├── amp_ringbuf.c
│       ├── amp_semaphore.c
│       └── amp_shmem.c
├── examples/             # Reference examples
│   ├── hello-amp/
│   ├── pingpong/
│   └── shared-counter/
├── cmake/                # Build system
│   └── platforms/        # Platform-specific configs
│       ├── generic.cmake
│       └── rp2350.cmake
├── docs/                 # Documentation
│   ├── AMP_CONTRACT.md
│   ├── EXAMPLES.md
│   └── RP2350_PLATFORM.md
└── tools/                # Build and flash tools
```

## Build Options

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `AMP_PLATFORM` | `generic` | Target platform (`generic`, `rp2350`) |
| `BUILD_EXAMPLES` | `ON` | Build example applications |
| `CMAKE_BUILD_TYPE` | `Debug` | Build type (`Debug`, `Release`) |

### Example

```bash
cmake -B build \
  -DAMP_PLATFORM=rp2350 \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=ON

cmake --build build
```

## Memory Configuration

### Default Shared Memory Layout

```c
// Examples use these defaults (can be overridden)
#define SHMEM_BASE 0x20040000  // Start of SRAM1 on RP2350
#define SHMEM_SIZE (16 * 1024) // 16KB shared pool
```

### Recommended RP2350 Layout

```
0x20000000 - 0x2003FFFF: Core 0 private (SRAM0, 256KB)
0x20040000 - 0x20043FFF: Shared memory (16KB)
0x20044000 - 0x2007FFFF: Core 1 private (240KB)
```

See [RP2350_PLATFORM.md](docs/RP2350_PLATFORM.md) for details.

## Platform Integration

### Adding a New Platform

1. Create platform configuration:
   ```bash
   # cmake/platforms/myplatform.cmake
   message(STATUS "Using myplatform configuration")
   # Add platform-specific settings
   ```

2. Implement platform-specific functions:
   ```c
   // Override weak symbols in runtime
   amp_core_t amp_get_core_id(void) {
       // Platform-specific implementation
   }
   
   amp_boot_status_t amp_boot_core(...) {
       // Platform-specific core boot
   }
   ```

3. Build with your platform:
   ```bash
   cmake -B build -DAMP_PLATFORM=myplatform
   ```

## Limitations (Phase 1)

This is a Phase 1 implementation with intentional limitations:

- ✗ No dynamic memory reclamation (bump allocator only)
- ✗ No priority-based scheduling
- ✗ No multi-producer/multi-consumer queues
- ✗ Basic error handling only
- ✗ No runtime core affinity changes

See [AMP_CONTRACT.md](docs/AMP_CONTRACT.md) for planned future extensions.

## Testing

### Validation

The examples serve as validation tests:

```bash
# Build all examples
cmake --build build

# Expected: All examples build without errors
# Expected: Examples run and show correct output (on hardware)
```

### Example Output Validation

Each example has expected output documented in [EXAMPLES.md](docs/EXAMPLES.md). Verify actual output matches expected behavior.

## Troubleshooting

### Build Issues

**CMake can't find compiler:**
```bash
# Specify toolchain explicitly
cmake -B build -DCMAKE_C_COMPILER=arm-none-eabi-gcc
```

**Missing Pico SDK (RP2350):**
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

### Runtime Issues

**Core 1 doesn't boot:**
- Check platform-specific boot implementation
- Verify stack and entry point addresses
- Check shared memory initialization

**Wrong results in shared-counter:**
- Verify memory barriers are working
- Check atomic operation support
- Ensure semaphore implementation is correct

See platform documentation for platform-specific issues.

## Contributing

Contributions are welcome! Please:

1. Follow existing code style
2. Add tests for new features
3. Update documentation
4. Keep changes minimal and focused

## License

[Specify your license here]

## References

- [ARM Cortex-M33 Documentation](https://developer.arm.com/documentation/100230/)
- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [Pico SDK](https://github.com/raspberrypi/pico-sdk)

## Support

- **Issues**: [GitHub Issues](https://github.com/rudupa/amp-mcu-reference/issues)
- **Discussions**: [GitHub Discussions](https://github.com/rudupa/amp-mcu-reference/discussions)

## Acknowledgments

This implementation follows industry best practices for embedded multicore systems and draws inspiration from various AMP implementations in production embedded systems.
