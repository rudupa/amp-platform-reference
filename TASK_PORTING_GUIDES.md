# Task Description: Write Porting Guide for RP2040 and ESP32-S3

## Goal
Create comprehensive porting guides that enable developers to adapt the AMP MCU reference implementation (currently supporting RP2350 and generic platforms) to RP2040 and ESP32-S3 dual-core microcontrollers. These guides will serve as templates for porting to other platforms.

## Background
The Phase 1 AMP implementation is designed to be vendor-neutral and portable. While it currently supports:
- **RP2350** (dual ARM Cortex-M33) - Primary target with full implementation
- **Generic** (portable C) - Reference implementation for testing

Many developers want to use the AMP framework on other popular dual-core MCUs:
- **RP2040** (dual ARM Cortex-M0+) - Previous generation Raspberry Pi Pico
- **ESP32-S3** (dual Xtensa LX7) - Popular WiFi/BLE MCU from Espressif

Porting guides will:
- Accelerate adoption across different platforms
- Demonstrate vendor-neutral design
- Identify platform-specific requirements
- Serve as templates for other MCUs

## Files/Modules Involved

### New Files to Create
- `docs/PORTING_RP2040.md` - Complete RP2040 porting guide
- `docs/PORTING_ESP32S3.md` - Complete ESP32-S3 porting guide
- `docs/PORTING_TEMPLATE.md` - Generic porting template for other platforms
- `cmake/platforms/rp2040.cmake` - CMake configuration for RP2040
- `cmake/platforms/esp32s3.cmake` - CMake configuration for ESP32-S3
- `runtime/platform/rp2040/` - RP2040-specific implementations (if needed)
- `runtime/platform/esp32s3/` - ESP32-S3-specific implementations (if needed)

### Files to Update
- `README.md` - Add RP2040 and ESP32-S3 to supported platforms list
- `docs/AMP_CONTRACT.md` - Note platform-specific variations
- `CMakeLists.txt` - Add new platform options

### Reference Files (No Changes)
- `docs/RP2350_PLATFORM.md` - Template for platform guides
- `cmake/platforms/rp2350.cmake` - Example platform configuration
- `runtime/src/amp_boot.c` - Contains weak symbols for platform overrides

## Expected Inputs and Outputs

### Inputs
1. **Platform Documentation**:
   - RP2040 datasheet and SDK documentation
   - ESP32-S3 Technical Reference Manual
   - Manufacturer boot sequence documentation
   - Memory map specifications

2. **AMP Contract Requirements**:
   - Core boot API requirements
   - Shared memory requirements
   - Synchronization primitive requirements
   - Memory barrier requirements

### Outputs
1. **Porting Guides** (Markdown):
   - Platform overview and capabilities
   - Hardware requirements and limitations
   - Memory layout recommendations
   - Step-by-step porting instructions
   - Platform-specific implementation notes
   - Testing and validation procedures

2. **Platform Implementations**:
   - CMake platform configuration files
   - Platform-specific boot code (if needed)
   - Linker scripts for memory layout
   - Example adaptations for platform

## Functional Requirements

### FR1: RP2040 Porting Guide

#### Platform Overview Section
- **Content**: RP2040 architecture, dual Cortex-M0+ cores, 264KB SRAM
- **Differences from RP2350**: Cortex-M0+ vs M33, no hardware semaphores, less RAM
- **Capabilities**: SIO spinlocks for synchronization, multicore FIFO

#### Memory Layout Section
- **Content**: Recommended memory regions for Core 0, Core 1, and shared
- **Details**:
  - 264KB total SRAM (0x20000000 - 0x20041FFF)
  - Suggested: 128KB Core 0, 16KB shared, 120KB Core 1
  - Stack placement considerations
- **Diagrams**: Memory map showing regions

#### Boot Sequence Section
- **Content**: How to boot Core 1 on RP2040
- **Details**:
  - Using multicore_launch_core1() from Pico SDK
  - Stack setup and entry point
  - Synchronization with SIO spinlocks
- **Code examples**: Platform-specific boot implementation

#### Synchronization Primitives Section
- **Content**: Implementing semaphores without hardware support
- **Details**:
  - Use SIO spinlocks (32 available)
  - Map amp_semaphore to spinlock API
  - Memory barriers with DMB instruction
- **Code examples**: Spinlock-based semaphore

#### Limitations and Workarounds Section
- **Content**: RP2040-specific constraints
- **Details**:
  - Cortex-M0+ lacks some atomic instructions
  - Smaller RAM than RP2350
  - No hardware semaphores
- **Workarounds**: Software solutions for each limitation

#### Testing and Validation Section
- **Content**: How to verify the port works correctly
- **Details**:
  - Build and flash examples
  - Expected serial output
  - Debug tips for RP2040
- **Checklist**: Validation steps

### FR2: ESP32-S3 Porting Guide

#### Platform Overview Section
- **Content**: ESP32-S3 architecture, dual Xtensa LX7 cores, up to 512KB SRAM
- **Differences from ARM platforms**: Different ISA, ESP-IDF framework, RTOS-based
- **Capabilities**: Hardware spinlocks, dual-core scheduler, inter-processor interrupts

#### Memory Layout Section
- **Content**: ESP32-S3 memory organization
- **Details**:
  - SRAM0, SRAM1, SRAM2 regions
  - Internal RAM (up to 512KB configurable)
  - External PSRAM (if available)
  - Recommended shared memory region
- **Diagrams**: ESP32-S3 memory architecture

#### Boot Sequence Section
- **Content**: ESP32-S3 core initialization
- **Details**:
  - ESP-IDF app_main() and FreeRTOS considerations
  - Pinning tasks to specific cores
  - Starting Core 1 (APP CPU) from Core 0 (PRO CPU)
- **Code examples**: ESP-IDF specific initialization

#### Synchronization Primitives Section
- **Content**: Using ESP32-S3 synchronization features
- **Details**:
  - Hardware spinlocks (compare-and-swap)
  - Portability layer for FreeRTOS primitives
  - Memory barriers and cache coherency
- **Code examples**: ESP32-S3 semaphore implementation

#### ESP-IDF Integration Section
- **Content**: Integrating AMP runtime with ESP-IDF
- **Details**:
  - Component structure (components/amp_runtime/)
  - CMakeLists.txt for ESP-IDF
  - Kconfig for configuration
  - Interaction with FreeRTOS
- **Code examples**: ESP-IDF component files

#### Limitations and Workarounds Section
- **Content**: ESP32-S3-specific constraints
- **Details**:
  - Different ISA (Xtensa vs ARM)
  - FreeRTOS overhead
  - Cache coherency considerations
- **Workarounds**: Bare-metal vs RTOS approaches

#### Testing and Validation Section
- **Content**: ESP-IDF build and flash process
- **Details**:
  - idf.py build/flash commands
  - Serial monitor output
  - Debugging with JTAG
- **Checklist**: Validation steps

### FR3: Generic Porting Template

#### Platform Overview Template
- **Content**: Placeholder sections for platform details
- **Guidance**: What information to include

#### Porting Checklist Template
- **Content**: Step-by-step porting tasks
- **Items**:
  - [ ] Document platform architecture
  - [ ] Define memory layout
  - [ ] Implement core boot sequence
  - [ ] Port synchronization primitives
  - [ ] Create platform CMake configuration
  - [ ] Adapt examples
  - [ ] Test and validate
  - [ ] Document limitations

#### Implementation Guidance Template
- **Content**: Functions that must be implemented
- **Details**: amp_get_core_id(), amp_boot_core(), barrier functions
- **Examples**: Code templates with TODOs

## Non-Functional Requirements

### NFR1: Clarity
- Clear, step-by-step instructions
- Code examples for all platform-specific implementations
- Diagrams for memory layouts and boot sequences
- Troubleshooting sections

### NFR2: Completeness
- Cover all aspects of porting (boot, memory, sync, build)
- Address common pitfalls and issues
- Provide working code examples
- Include validation procedures

### NFR3: Consistency
- Follow format of existing RP2350_PLATFORM.md
- Use consistent terminology
- Maintain same structure across guides
- Cross-reference AMP_CONTRACT.md

### NFR4: Accuracy
- Verify against actual hardware datasheets
- Test recommendations on real hardware (if possible)
- Cite official documentation sources
- Note any untested assumptions

## Acceptance Criteria

### AC1: RP2040 Porting Guide
- [ ] Complete guide created following template
- [ ] Memory layout recommendations provided
- [ ] Boot sequence documented with code examples
- [ ] Spinlock-based synchronization explained
- [ ] CMake platform file created
- [ ] Limitations clearly documented
- [ ] Testing checklist included

### AC2: ESP32-S3 Porting Guide
- [ ] Complete guide created following template
- [ ] ESP-IDF integration documented
- [ ] Memory layout with SRAM regions explained
- [ ] Core pinning and FreeRTOS interaction covered
- [ ] CMake/ESP-IDF component structure provided
- [ ] Xtensa-specific considerations documented
- [ ] Testing with idf.py explained

### AC3: Generic Template
- [ ] Reusable template created
- [ ] Porting checklist comprehensive
- [ ] Function implementation guidance clear
- [ ] Covers all porting aspects

### AC4: Integration
- [ ] README.md updated with new platforms
- [ ] Links to guides added
- [ ] Platform comparison table (optional)
- [ ] Build instructions for each platform

### AC5: Quality
- [ ] No spelling or grammar errors
- [ ] Code examples are syntactically valid
- [ ] All external references cited
- [ ] Reviewed for technical accuracy
- [ ] Consistent formatting

## Implementation Notes

### RP2040 Specifics

#### Key Differences from RP2350
- Cortex-M0+ (simpler instruction set, no DSP/FPU)
- 264KB SRAM vs 520KB on RP2350
- SIO spinlocks instead of hardware semaphores
- Similar multicore APIs in Pico SDK

#### Memory Layout Example
```
0x20000000 - 0x2001FFFF: Core 0 private (128 KB)
0x20020000 - 0x20023FFF: Shared memory (16 KB)
0x20024000 - 0x20041FFF: Core 1 private (120 KB)
```

#### Boot Code Pattern
```c
// RP2040-specific boot using Pico SDK
void amp_boot_core1_rp2040(void (*entry)(void), uint32_t* stack) {
    multicore_reset_core1();
    multicore_launch_core1_with_stack(entry, stack, STACK_SIZE);
}
```

### ESP32-S3 Specifics

#### Key Differences from ARM Platforms
- Xtensa LX7 architecture (different ISA)
- ESP-IDF framework (FreeRTOS-based)
- PRO CPU (Core 0) and APP CPU (Core 1)
- Hardware spinlocks available
- Cache coherency concerns

#### ESP-IDF Component Structure
```
components/
└── amp_runtime/
    ├── CMakeLists.txt
    ├── Kconfig
    ├── include/
    │   └── amp_*.h
    └── src/
        ├── amp_*.c
        └── platform/
            └── esp32s3/
                └── amp_boot_esp32s3.c
```

#### Task Pinning Pattern
```c
// ESP32-S3 specific - pin tasks to cores
void app_main(void) {
    // Core 0 (PRO CPU) initialization
    amp_boot_init();
    
    // Start Core 1 (APP CPU) task
    xTaskCreatePinnedToCore(core1_task, "Core1", 4096, 
                           NULL, 1, NULL, 1);
}
```

## Dependencies
- RP2040 datasheet and Pico SDK documentation
- ESP32-S3 TRM and ESP-IDF documentation
- Existing RP2350_PLATFORM.md as template
- Existing AMP_CONTRACT.md for requirements
- Access to hardware for testing (optional but recommended)

## Estimated Effort
- Research RP2040 specifics: 2 hours
- Write RP2040 porting guide: 3 hours
- Create RP2040 CMake config: 1 hour
- Research ESP32-S3 specifics: 2 hours
- Write ESP32-S3 porting guide: 4 hours
- Create ESP32-S3 ESP-IDF integration guide: 2 hours
- Create generic template: 1 hour
- Documentation integration and review: 2 hours
- **Total: ~17 hours (1.5-2 development sessions over 2-3 days)**

## Success Metrics
- Both porting guides are comprehensive and clear
- Guides follow existing documentation style
- Code examples compile (verified if possible)
- Generic template is reusable for other platforms
- Positive review from technical reviewers
- Addresses platform portability goal
- Expands supported platform list

## Future Extensions
After initial guides, could add:
- STM32 dual-core (Cortex-M7 + M4) porting guide
- NXP i.MX RT dual-core porting guide
- RISC-V dual-core platform guides
- Automated porting validation scripts
