/**
 * @file amp_config.h
 * @brief AMP MCU Configuration and Domain Definitions
 * 
 * Defines the configuration structure and domain assignments for
 * asymmetric multiprocessing on dual-core MCUs.
 */

#ifndef AMP_CONFIG_H
#define AMP_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Core domain identifiers
 */
typedef enum {
    AMP_CORE0 = 0,  /**< Primary core (usually ARM Cortex-M33) */
    AMP_CORE1 = 1,  /**< Secondary core */
    AMP_CORE_COUNT = 2
} amp_core_t;

/**
 * Memory region definition
 */
typedef struct {
    uint32_t base;      /**< Base address of the region */
    uint32_t size;      /**< Size in bytes */
    uint32_t flags;     /**< Access flags (read/write/execute) */
} amp_memory_region_t;

/**
 * Domain configuration
 */
typedef struct {
    amp_core_t core_id;                 /**< Core identifier */
    uint32_t stack_base;                /**< Stack base address */
    uint32_t stack_size;                /**< Stack size in bytes */
    uint32_t entry_point;               /**< Entry function address */
    amp_memory_region_t *regions;       /**< Memory regions */
    uint32_t region_count;              /**< Number of regions */
} amp_domain_config_t;

/**
 * Get the current core ID
 * 
 * @return Current core identifier
 */
amp_core_t amp_get_core_id(void);

/**
 * Configure domain for a specific core
 * 
 * @param config Domain configuration
 * @return 0 on success, negative on error
 */
int amp_configure_domain(const amp_domain_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* AMP_CONFIG_H */
