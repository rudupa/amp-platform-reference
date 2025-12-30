/**
 * @file amp_boot.h
 * @brief AMP Boot Sequence Management
 * 
 * Provides boot sequence control for multi-core systems including
 * core initialization, synchronization, and startup.
 */

#ifndef AMP_BOOT_H
#define AMP_BOOT_H

#include <stdint.h>
#include "amp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Boot status codes
 */
typedef enum {
    AMP_BOOT_SUCCESS = 0,
    AMP_BOOT_ERROR_INVALID_CORE = -1,
    AMP_BOOT_ERROR_ALREADY_RUNNING = -2,
    AMP_BOOT_ERROR_TIMEOUT = -3,
    AMP_BOOT_ERROR_CONFIG = -4
} amp_boot_status_t;

/**
 * Initialize the AMP runtime
 * 
 * Must be called on the primary core before any other AMP operations.
 * 
 * @return AMP_BOOT_SUCCESS on success, error code on failure
 */
amp_boot_status_t amp_boot_init(void);

/**
 * Boot a secondary core
 * 
 * @param core_id Core to boot
 * @param entry_point Entry function address
 * @param stack_pointer Initial stack pointer
 * @return AMP_BOOT_SUCCESS on success, error code on failure
 */
amp_boot_status_t amp_boot_core(amp_core_t core_id, uint32_t entry_point, uint32_t stack_pointer);

/**
 * Wait for core to complete initialization
 * 
 * @param core_id Core to wait for
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return AMP_BOOT_SUCCESS on success, error code on failure
 */
amp_boot_status_t amp_boot_wait_core_ready(amp_core_t core_id, uint32_t timeout_ms);

/**
 * Signal that current core has completed initialization
 */
void amp_boot_signal_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* AMP_BOOT_H */
