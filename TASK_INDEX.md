# AMP MCU Reference - Task Index

This repository contains actionable task descriptions for enhancing and documenting the AMP MCU reference implementation.

## Current Task Descriptions

### 1. Priority-Based Message Scheduling (Phase 2 Enhancement)
**File**: [TASK_DESCRIPTION.md](./TASK_DESCRIPTION.md)  
**Scope**: ~10 hours (single session)  
**Status**: ✅ Appropriate - Extends existing Phase 1 mailbox

Extends the existing FIFO mailbox with priority-based message scheduling. Addresses a Phase 1 limitation: "No priority-based scheduling."

**Key Features**:
- 4 configurable priority levels (LOW, NORMAL, HIGH, CRITICAL)
- Backward compatible with existing mailbox API
- < 1 KB memory overhead
- Maintains FIFO within same priority

**Deliverables**:
- Enhanced `amp_mailbox` with priority support
- New `priority-messaging` example
- Test coverage > 90%
- Full documentation

---

### 2. Phase 1 Architecture Diagrams (Documentation)
**File**: [TASK_PHASE1_DIAGRAMS.md](./TASK_PHASE1_DIAGRAMS.md)  
**Scope**: ~11 hours (single session)  
**Status**: ✅ Needed - Visual documentation missing

Creates comprehensive visual documentation for Phase 1 implementation using PlantUML and SVG diagrams.

**Key Deliverables**:
- Architecture overview diagram
- Boot sequence diagram
- Memory layout diagrams (RP2350 + generic)
- IPC flow diagrams (mailbox, semaphore, ringbuf)
- State machine diagrams
- Component dependency diagram

**Tools**: PlantUML, Makefile for automation

---

### 3. Porting Guides (RP2040 and ESP32-S3)
**File**: [TASK_PORTING_GUIDES.md](./TASK_PORTING_GUIDES.md)  
**Scope**: ~17 hours (1.5-2 sessions)  
**Status**: ✅ Needed - Expand platform support

Comprehensive porting guides to enable adaptation to RP2040 and ESP32-S3 platforms, plus a generic porting template.

**Key Deliverables**:
- `docs/PORTING_RP2040.md` - Complete RP2040 guide
- `docs/PORTING_ESP32S3.md` - Complete ESP32-S3 guide
- `docs/PORTING_TEMPLATE.md` - Generic template
- Platform-specific CMake configs
- Integration instructions (Pico SDK, ESP-IDF)

**Platforms Covered**:
- RP2040 (dual Cortex-M0+)
- ESP32-S3 (dual Xtensa LX7)

---

## Quick Reference Summary
**File**: [TASK_SUMMARY.md](./TASK_SUMMARY.md)  
Quick reference for the priority mailbox task.

---

## Context: What Already Exists (Phase 1)

The main branch already includes a complete Phase 1 implementation:

### ✅ Already Implemented
- **Boot Management** (`amp_boot.h/c`) - Core1 handoff ✓
- **Shared Memory** (`amp_shmem.h/c`) - Allocator ✓
- **Mailbox** (`amp_mailbox.h/c`) - FIFO messaging ✓
- **Semaphore** (`amp_semaphore.h/c`) - Synchronization ✓
- **Ring Buffer** (`amp_ringbuf.h/c`) - Streaming data ✓
- **Configuration** (`amp_config.h/c`) - Domain config ✓

### ✅ Examples Already Implemented
- **hello-amp** - Basic initialization ✓
- **pingpong** - Bidirectional messaging ✓
- **shared-counter** - Concurrent access ✓

### ✅ Documentation Already Exists
- `README.md` - Comprehensive overview
- `docs/AMP_CONTRACT.md` - Complete specification
- `docs/RP2350_PLATFORM.md` - RP2350 implementation notes
- `docs/EXAMPLES.md` - Example descriptions

### ❌ What's Missing (Addressed by These Tasks)
- Priority-based message scheduling
- Visual architecture diagrams
- Porting guides for RP2040 and ESP32-S3

---

## Task Selection Guide

### For Code Enhancement
→ Use **TASK_DESCRIPTION.md** (Priority Mailbox)
- Adds new functionality
- Extends existing components
- Includes code, tests, examples

### For Documentation
→ Use **TASK_PHASE1_DIAGRAMS.md** (Diagrams)
- Improves understanding
- Visual documentation
- No code changes required

### For Platform Expansion
→ Use **TASK_PORTING_GUIDES.md** (Porting)
- Enables new platforms
- Primarily documentation
- Some platform-specific code

---

## Common Task Structure

All tasks follow this structure:
1. **Goal** - Clear objective
2. **Background** - Context and motivation
3. **Files/Modules** - What to create/modify
4. **Inputs/Outputs** - Expected data and results
5. **Functional Requirements** - What it must do
6. **Non-Functional Requirements** - Quality attributes
7. **Acceptance Criteria** - Definition of done
8. **Implementation Notes** - Technical guidance
9. **Estimated Effort** - Time required
10. **Success Metrics** - How to measure success

---

## Phase 1 Limitations (From Main Branch)

These limitations are intentional for Phase 1:
- ✗ No dynamic memory reclamation (bump allocator only)
- ✗ **No priority-based scheduling** ← *Addressed by Priority Mailbox task*
- ✗ No multi-producer/multi-consumer queues
- ✗ Basic error handling only
- ✗ No runtime core affinity changes

Future tasks could address other limitations.

---

## How to Use These Tasks

1. **Review** the task description thoroughly
2. **Understand** existing implementation (see main branch)
3. **Follow** the structure in the task document
4. **Implement** according to requirements
5. **Test** against acceptance criteria
6. **Document** as specified
7. **Validate** against success metrics

---

## Contributing New Tasks

When creating new task descriptions:
- Check main branch to avoid duplicating existing work
- Follow the common task structure above
- Ensure task is scoped for single session (< 20 hours)
- Include clear acceptance criteria
- Provide implementation guidance
- Consider Phase 1 limitations for enhancement ideas

---

## Related Documentation

- **Main Branch README**: Overview of existing implementation
- **AMP_CONTRACT.md**: Specification and requirements
- **RP2350_PLATFORM.md**: Example platform guide (template for porting)
- **EXAMPLES.md**: Example application descriptions

---

*This index helps navigate the task descriptions and understand how they fit into the overall AMP MCU reference implementation project.*
