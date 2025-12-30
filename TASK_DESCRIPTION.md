# Task Description: Implement Inter-Core Communication Module for AMP MCU

## Goal
Implement a lightweight, deterministic inter-core communication (IPC) module that enables message passing between two cores in an Asymmetric Multi-Processing (AMP) dual-core MCU system using shared memory and hardware semaphores.

## Background
In AMP systems, each core runs its own independent software stack. Core 0 (primary) typically handles system initialization and high-level tasks, while Core 1 (secondary) handles real-time processing. This task focuses on establishing reliable communication between the cores.

## Files/Modules Involved

### New Files to Create
- `src/ipc/ipc_core.c` - Core IPC implementation with send/receive functions
- `src/ipc/ipc_core.h` - Public API and data structure definitions
- `include/ipc_config.h` - Configuration parameters (buffer sizes, memory regions)
- `src/ipc/ringbuffer.c` - Lock-free ring buffer implementation for message queues
- `src/ipc/ringbuffer.h` - Ring buffer interface
- `tests/test_ipc.c` - Unit tests for IPC module

### Supporting Files
- `src/platform/hw_semaphore.c` - Hardware semaphore abstraction layer
- `src/platform/hw_semaphore.h` - Hardware semaphore API
- `linker/shared_memory.ld` - Linker script fragment defining shared memory region

## Expected Inputs and Outputs

### Inputs
1. **Message Structure**:
   - Message ID (uint32_t)
   - Payload data (byte array, max 256 bytes)
   - Payload length (uint32_t)
   - Priority (uint8_t): 0=low, 1=normal, 2=high

2. **Configuration Parameters** (compile-time):
   - Shared memory base address
   - Ring buffer size (number of messages)
   - Maximum message payload size
   - Timeout values for blocking operations

### Outputs
1. **API Return Values**:
   - `IPC_SUCCESS` (0): Operation completed successfully
   - `IPC_ERROR_FULL` (-1): Message queue is full
   - `IPC_ERROR_EMPTY` (-2): No messages available
   - `IPC_ERROR_TIMEOUT` (-3): Operation timed out
   - `IPC_ERROR_INVALID` (-4): Invalid parameters

2. **Status Information**:
   - Number of messages pending
   - Buffer utilization percentage
   - Dropped message count
   - Last error code

## Functional Requirements

### FR1: Message Send (Non-blocking)
- **Function**: `int32_t ipc_send(uint32_t msg_id, const uint8_t* payload, uint32_t len, uint8_t priority)`
- Validate input parameters (null checks, length limits)
- Acquire hardware semaphore with timeout
- Write message to ring buffer in shared memory
- Update write pointer atomically
- Release hardware semaphore
- Return status code

### FR2: Message Receive (Non-blocking)
- **Function**: `int32_t ipc_receive(uint32_t* msg_id, uint8_t* payload, uint32_t* len, uint32_t timeout_ms)`
- Check if messages are available
- Acquire hardware semaphore with timeout
- Read message from ring buffer
- Update read pointer atomically
- Release hardware semaphore
- Copy message to caller's buffer
- Return status code

### FR3: Initialization
- **Function**: `int32_t ipc_init(uint8_t core_id)`
- Initialize shared memory region (Core 0 only)
- Clear ring buffer structures
- Initialize hardware semaphores
- Set core-specific configuration
- Perform handshake to verify both cores are ready
- Return status code

### FR4: Status Query
- **Function**: `int32_t ipc_get_status(ipc_status_t* status)`
- Return current queue depth
- Calculate buffer utilization
- Report error statistics
- Return last communication timestamp

### FR5: Thread Safety
- Use hardware semaphores for mutual exclusion
- Implement lock-free algorithms where possible
- Ensure atomic operations for pointer updates
- No dynamic memory allocation (embedded constraint)

## Non-Functional Requirements

### NFR1: Performance
- Message send/receive latency: < 10 microseconds (typical case)
- Zero-copy where possible for large payloads
- Minimal CPU overhead (< 5% for 1000 msg/sec throughput)

### NFR2: Reliability
- No message loss under normal operation
- Graceful degradation when buffers are full
- Detect and report communication errors
- Support reset/recovery without system reboot

### NFR3: Resource Constraints
- Total RAM usage: < 8 KB (including buffers)
- Code size: < 4 KB
- No heap allocation (stack and static memory only)
- Compatible with bare-metal and RTOS environments

### NFR4: Portability
- Abstract hardware-specific features (semaphores, memory barriers)
- Support different dual-core MCU architectures (Cortex-M4+M0, Cortex-M7+M4, etc.)
- Configurable memory layout via linker script
- Standard C99 compliance

## Acceptance Criteria

### AC1: Functional Correctness
- [ ] Core 0 can send a message and Core 1 receives it correctly
- [ ] Core 1 can send a message and Core 0 receives it correctly
- [ ] Bidirectional communication works simultaneously
- [ ] Messages are received in FIFO order for same priority
- [ ] High-priority messages are handled before low-priority when enabled
- [ ] Buffer full condition is detected and reported correctly
- [ ] Buffer empty condition is detected and reported correctly
- [ ] Invalid parameters are rejected with appropriate error codes

### AC2: Robustness
- [ ] System handles rapid message bursts (> 1000 messages/sec)
- [ ] No deadlocks occur under any message pattern
- [ ] Graceful behavior when one core stops responding
- [ ] Memory corruption detection (buffer overruns prevented)
- [ ] Correct operation after IPC reset

### AC3: Testing
- [ ] Unit tests achieve > 90% code coverage
- [ ] Integration test with both cores running passes
- [ ] Stress test (sustained load for 1 hour) passes
- [ ] Error injection tests (full buffer, invalid params) pass
- [ ] Performance benchmarks meet latency requirements

### AC4: Documentation
- [ ] API functions have complete doxygen comments
- [ ] Architecture diagram shows memory layout and data flow
- [ ] Usage examples demonstrate common scenarios
- [ ] Configuration guide explains all parameters
- [ ] Known limitations are documented

### AC5: Code Quality
- [ ] Follows project coding standards (MISRA-C subset)
- [ ] No compiler warnings with -Wall -Wextra
- [ ] Static analysis (cppcheck) reports no issues
- [ ] Memory-safe (no buffer overflows, null dereferences)
- [ ] Passes CI/CD pipeline checks

## Implementation Notes

### Memory Layout
```
Shared Memory Region (8 KB @ 0x20000000):
  +0x0000: Core 0 to Core 1 Ring Buffer (3 KB)
  +0x0C00: Core 1 to Core 0 Ring Buffer (3 KB)
  +0x1800: Control/Status Structure (512 bytes)
  +0x1A00: Reserved (1.5 KB)
```

### Ring Buffer Structure
```c
typedef struct {
    volatile uint32_t write_idx;
    volatile uint32_t read_idx;
    volatile uint32_t capacity;
    volatile uint32_t msg_size;
    uint8_t buffer[RING_BUFFER_SIZE];
} ring_buffer_t;
```

### Message Format
```c
typedef struct {
    uint32_t msg_id;
    uint32_t length;
    uint8_t priority;
    uint8_t reserved[3];
    uint8_t payload[MAX_PAYLOAD_SIZE];
} ipc_message_t;
```

## Dependencies
- ARM CMSIS headers (for memory barriers, atomic operations)
- Hardware abstraction layer for target MCU
- Build system: CMake or Makefile
- Testing framework: Unity or custom minimal framework

## Estimated Effort
- Design and setup: 2 hours
- Core implementation: 4 hours
- Testing and debugging: 3 hours
- Documentation: 1 hour
- **Total: ~10 hours (single development session over 1-2 days)**

## Success Metrics
- Successfully demonstrated bidirectional message passing
- All acceptance criteria met
- Clean compilation with no warnings
- Test suite passes with 100% success rate
- Code review approved by peer
