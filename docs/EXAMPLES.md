# Example Descriptions

This directory contains three reference examples demonstrating the AMP MCU runtime.

## 1. hello-amp

**Purpose**: Basic AMP initialization and inter-core communication.

**Demonstrates**:
- Boot sequence initialization
- Secondary core startup
- Basic mailbox communication
- Core synchronization

**Expected Output**:
```
=== Hello AMP Example ===
Core 0: Starting AMP initialization
Core 0: Booting Core 1...
Core 1: Hello from secondary core!
Core 0: Core 1 is ready!
Core 1: Message sent to Core 0
Core 0: Received: 'Hello from Core 1!' from Core 1
Core 0: Response sent to Core 1
Core 1: Received: 'Hello from Core 0!' from Core 0
Core 1: Done!
Core 0: Example complete!
=========================
```

**Key Concepts**:
- `amp_boot_init()` - Initialize boot system
- `amp_boot_core()` - Start secondary core
- `amp_boot_signal_ready()` - Signal core ready
- `amp_mailbox_send/recv()` - Message passing

## 2. pingpong

**Purpose**: Bidirectional message passing with sequencing.

**Demonstrates**:
- Two mailboxes for bidirectional communication
- Message sequencing and validation
- Continuous inter-core exchange
- Completion signaling

**Expected Output**:
```
=== Ping-Pong Example ===
Core 0: Initializing...
Core 0: Starting Core 1...
Core 1: Starting ping-pong receiver
Core 0: Starting ping-pong exchange...
Core 0: Sent PING #0
Core 1: Received PING #0
Core 1: Sent PONG #0
Core 0: Received PONG #0
Core 0: Sent PING #1
...
Core 0: Received PONG #9
Core 1: Ping-pong complete
Core 0: Received completion from Core 1
Core 0: Ping-pong complete!
=========================
```

**Key Concepts**:
- Multiple mailboxes for different directions
- Message sequencing
- Coordinated termination
- Timeout handling

## 3. shared-counter

**Purpose**: Concurrent shared memory access with synchronization.

**Demonstrates**:
- Shared memory allocation
- Semaphore-based mutual exclusion
- Concurrent increments from both cores
- Result validation

**Expected Output**:
```
=== Shared Counter Example ===
Core 0: Initializing...
Core 0: Starting Core 1...
Core 1: Starting counter increments
Core 0: Both cores running, starting increments...
Core 0: Completed 20 increments
Core 1: Completed 20 increments
...
Core 0: Completed 100 increments
Core 0: Finished all increments
Core 1: Completed 100 increments
Core 1: Finished all increments

=== Results ===
Expected counter value: 200
Actual counter value:   200
Core 0 increments:      100
Core 1 increments:      100
SUCCESS: Counter value is correct!
===============================
```

**Key Concepts**:
- `amp_shmem_alloc()` - Allocate shared memory
- `amp_semaphore_create()` - Create synchronization primitive
- `amp_semaphore_wait()` - Acquire lock
- `amp_semaphore_post()` - Release lock
- Critical section protection

## Building the Examples

See the main [README.md](../README.md) for build instructions.

## Running on Hardware

### RP2350 (Pico 2)

1. Build with RP2350 platform:
   ```bash
   cmake -B build -DAMP_PLATFORM=rp2350
   cmake --build build
   ```

2. Flash the `.uf2` file to your Pico 2 board

3. Monitor serial output (115200 baud)

### Generic Platform

The examples can be compiled for testing on host systems, though actual dual-core execution requires hardware support.

## Modifying Examples

Each example is self-contained in its own directory:
- `hello-amp/hello_amp.c`
- `pingpong/pingpong.c`
- `shared-counter/shared_counter.c`

Modify the source files and rebuild to experiment with:
- Different message sizes
- Various synchronization patterns
- Custom communication protocols
- Performance measurements

## Troubleshooting

### Example doesn't complete

- Check timeout values (may need adjustment for slower cores)
- Verify shared memory region is correctly configured
- Ensure core 1 boots successfully

### Wrong counter value in shared-counter

- Memory barriers may not be working correctly
- Semaphore implementation issue
- Check atomic operations support

### No output from Core 1

- Core 1 boot may have failed
- Check platform-specific boot implementation
- Verify stack and entry point configuration

## Next Steps

After understanding these examples:
1. Create custom communication patterns
2. Integrate with real peripherals
3. Add ring buffer streaming
4. Implement application-specific protocols
5. Optimize for your use case
