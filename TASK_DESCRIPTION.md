# Task Description: Add Priority-Based Message Scheduling to AMP Mailbox

## Goal
Extend the existing AMP mailbox implementation to support priority-based message scheduling, allowing high-priority messages to be processed before lower-priority messages. This enhancement moves beyond the Phase 1 basic FIFO mailbox to support more sophisticated real-time communication patterns.

## Background
The current AMP MCU reference implementation (Phase 1) includes a basic FIFO mailbox for inter-core communication. However, many real-time embedded systems require priority-based message handling where urgent messages (e.g., sensor alerts, safety-critical events) must be processed before routine messages (e.g., status updates, logging). This task implements priority queuing as identified in the Phase 1 limitations.

## Files/Modules Involved

### Files to Modify
- `runtime/include/amp_mailbox.h` - Add priority field to message structure and priority-aware API
- `runtime/src/amp_mailbox.c` - Implement priority queue logic
- `runtime/include/amp_config.h` - Add configuration for number of priority levels

### New Files to Create
- `runtime/src/amp_priority_queue.c` - Priority queue implementation (internal)
- `runtime/include/amp_priority_queue.h` - Priority queue interface (internal)
- `examples/priority-messaging/priority_msg.c` - Example demonstrating priority messaging
- `tests/test_priority_mailbox.c` - Unit tests for priority mailbox functionality

### Files to Review (No Changes)
- `runtime/src/amp_semaphore.c` - Existing synchronization (reused)
- `runtime/src/amp_shmem.c` - Existing shared memory allocator (reused)

## Expected Inputs and Outputs

### Inputs
1. **Message Structure** (Enhanced):
   - Message ID (uint32_t)
   - Payload data (byte array, existing max size per mailbox config)
   - Payload length (uint32_t)
   - **Priority (amp_priority_t)**: NEW field
     - `AMP_PRIORITY_LOW` (0)
     - `AMP_PRIORITY_NORMAL` (1) - default
     - `AMP_PRIORITY_HIGH` (2)
     - `AMP_PRIORITY_CRITICAL` (3)

2. **Configuration Parameters** (compile-time):
   - Number of priority levels (default: 4)
   - Per-priority queue depth limits
   - Priority preemption policy (strict vs. weighted)

### Outputs
1. **API Return Values** (Compatible with existing):
   - `AMP_SUCCESS` (0): Operation completed successfully
   - `AMP_ERROR_FULL` (-1): Message queue is full for this priority
   - `AMP_ERROR_EMPTY` (-2): No messages available
   - `AMP_ERROR_TIMEOUT` (-3): Operation timed out
   - `AMP_ERROR_INVALID` (-4): Invalid parameters or priority

2. **Status Information** (Enhanced):
   - Number of messages pending per priority level
   - Buffer utilization per priority
   - Dropped message count per priority
   - Highest priority message waiting
   - Priority inversion events (if any)

## Functional Requirements

### FR1: Priority-Based Message Send
- **Function**: `amp_status_t amp_mailbox_send_priority(amp_mailbox_t* mbox, const void* data, size_t len, amp_priority_t priority)`
- Extends existing `amp_mailbox_send()` with priority parameter
- Validate priority level is within configured range
- Place message in appropriate priority queue
- Maintain ordering within same priority (FIFO per priority)
- Return appropriate status code

### FR2: Priority-Based Message Receive
- **Function**: `amp_status_t amp_mailbox_recv_priority(amp_mailbox_t* mbox, void* data, size_t* len, uint32_t timeout_ms)`
- Always dequeue highest priority message first
- If multiple priorities have messages, select from highest
- Maintain FIFO order within same priority level
- Compatible with existing `amp_mailbox_recv()` behavior

### FR3: Backward Compatibility
- **Function**: Maintain existing `amp_mailbox_send()` and `amp_mailbox_recv()` APIs
- Default priority is `AMP_PRIORITY_NORMAL` for legacy sends
- Existing examples continue to work without modification
- New priority field in mailbox structure is optional

### FR4: Priority Configuration
- **Function**: `amp_status_t amp_mailbox_create_priority(amp_mailbox_t* mbox, const amp_mailbox_config_t* config)`
- Allow configuration of number of priority levels (2-8)
- Configure per-priority queue depths
- Set total memory budget for all priority queues
- Initialize priority queue data structures in shared memory

### FR5: Status and Diagnostics
- **Function**: `amp_status_t amp_mailbox_get_priority_stats(amp_mailbox_t* mbox, amp_priority_stats_t* stats)`
- Return per-priority queue depths
- Report dropped messages by priority
- Identify priority starvation conditions
- Provide debugging information for priority issues

## Non-Functional Requirements

### NFR1: Performance
- Priority determination overhead: < 1 microsecond
- Send/receive latency similar to existing mailbox (< 10 microseconds)
- Minimal memory overhead: < 16 bytes per priority level
- No heap allocation (use existing shared memory allocator)

### NFR2: Reliability
- No message loss under normal operation
- No priority inversion (high priority always processed first)
- Graceful degradation when priority queues are full
- Detect and report starvation of low-priority messages

### NFR3: Resource Constraints
- Additional RAM usage: < 1 KB for 4 priority levels
- Code size increase: < 2 KB
- Compatible with existing AMP runtime architecture
- Reuse existing semaphore and shared memory infrastructure

### NFR4: Compatibility
- Maintain backward compatibility with existing mailbox API
- Existing examples work without modification
- New priority API is opt-in (use priority variant or standard)
- Platform-independent (works on RP2350 and generic platforms)

## Acceptance Criteria

### AC1: Functional Correctness
- [ ] High-priority message sent after low-priority is received first
- [ ] Messages of same priority are received in FIFO order
- [ ] Backward compatibility: existing mailbox examples still work
- [ ] Priority field can be queried from received messages
- [ ] All priority levels can send and receive messages
- [ ] Buffer full condition reported per-priority correctly
- [ ] Invalid priority levels are rejected with appropriate error

### AC2: Robustness
- [ ] System handles mixed priority message bursts correctly
- [ ] Low-priority messages eventually processed (no starvation)
- [ ] Priority queues handle overflow gracefully
- [ ] Correct operation with all priority queues in use simultaneously
- [ ] No priority inversion issues detected

### AC3: Testing
- [ ] Unit tests achieve > 90% code coverage for new priority code
- [ ] New priority-messaging example demonstrates all priority levels
- [ ] Stress test with mixed priorities for 1 hour passes
- [ ] Backward compatibility test: existing examples pass unchanged
- [ ] Performance benchmarks show < 10% overhead vs. non-priority mailbox

### AC4: Documentation
- [ ] Priority API functions have complete doxygen comments
- [ ] README updated with priority messaging example
- [ ] EXAMPLES.md includes priority-messaging example
- [ ] AMP_CONTRACT.md updated to reflect priority capability
- [ ] Migration guide for upgrading from basic to priority mailbox

### AC5: Code Quality
- [ ] Follows existing AMP codebase style and conventions
- [ ] No compiler warnings with -Wall -Wextra
- [ ] Static analysis (cppcheck) reports no issues
- [ ] Memory-safe (no buffer overflows, null dereferences)
- [ ] Passes CI/CD pipeline checks

## Implementation Notes

### Priority Queue Design

Multiple approaches possible:
1. **Multiple FIFOs** - Separate queue per priority (simple, recommended)
2. **Sorted List** - Single queue with priority ordering (complex)
3. **Bitmap + Arrays** - Fast priority lookup (memory intensive)

Recommended: Multiple FIFOs approach for simplicity and performance.

### Memory Layout (Extension of existing shared memory)

```c
// Priority mailbox extends amp_mailbox_t
typedef struct {
    amp_mailbox_t base;           // Existing mailbox structure
    uint8_t num_priorities;        // Number of priority levels
    uint8_t priority_bitmap;       // Bit per priority (has messages?)
    struct {
        uint32_t head;             // Queue head index
        uint32_t tail;             // Queue tail index
        uint32_t capacity;         // Max messages for this priority
        uint32_t dropped;          // Dropped message count
    } queues[AMP_MAX_PRIORITIES];  // Per-priority queue metadata
    // Message buffers follow in shared memory
} amp_priority_mailbox_t;
```

### Message Format (Enhancement)

```c
typedef struct {
    amp_priority_t priority;       // NEW: Message priority
    uint32_t msg_id;               // Existing fields
    uint32_t length;
    uint8_t data[AMP_MAX_MSG_SIZE];
} amp_priority_msg_t;
```

### API Usage Example

```c
// Initialize priority mailbox (on Core 0)
amp_mailbox_config_t config = {
    .num_priorities = 4,
    .max_msg_size = 256,
    .total_capacity = 32
};
amp_mailbox_t mbox;
amp_mailbox_create_priority(&mbox, &config);

// Send high-priority alert from Core 1
uint8_t alert_data[] = "SENSOR_FAULT";
amp_mailbox_send_priority(&mbox, alert_data, sizeof(alert_data), AMP_PRIORITY_HIGH);

// Send normal-priority status
uint8_t status_data[] = "STATUS_OK";
amp_mailbox_send_priority(&mbox, status_data, sizeof(status_data), AMP_PRIORITY_NORMAL);

// Receive on Core 0 - alert comes first!
uint8_t buffer[256];
size_t len;
amp_mailbox_recv_priority(&mbox, buffer, &len, 1000); // Gets "SENSOR_FAULT" first
```

## Dependencies
- Existing AMP runtime (amp_mailbox, amp_semaphore, amp_shmem)
- ARM CMSIS headers (already in use)
- CMake build system (already configured)
- Existing test framework from examples

## Estimated Effort
- Design and planning: 1 hour
- Core priority queue implementation: 3 hours
- API extensions and integration: 2 hours
- Example application: 1 hour
- Testing and debugging: 2 hours
- Documentation: 1 hour
- **Total: ~10 hours (single development session over 1-2 days)**

## Success Metrics
- All existing examples pass without modification (backward compatibility)
- New priority-messaging example successfully demonstrates priority ordering
- High-priority message latency < 10 microseconds (same as current mailbox)
- Memory overhead < 1 KB for 4-priority configuration
- Test suite passes with 100% success rate
- Code review approved
- Addresses Phase 1 limitation: "No priority-based scheduling"
