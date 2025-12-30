/**
 * @file amp_config.c
 * @brief AMP Configuration Implementation
 */

#include "amp_config.h"

/**
 * Platform-specific function to get core ID
 * This is a generic implementation - should be overridden for specific platforms
 */
__attribute__((weak)) amp_core_t amp_get_core_id(void)
{
    /* Generic implementation - returns CORE0 by default
     * Platform-specific implementations should override this
     * For RP2350: use SIO->CPUID
     */
    return AMP_CORE0;
}

/**
 * Configure domain for a specific core
 */
int amp_configure_domain(const amp_domain_config_t *config)
{
    if (!config) {
        return -1;
    }

    /* Validate configuration */
    if (config->core_id >= AMP_CORE_COUNT) {
        return -1;
    }

    if (config->stack_size == 0 || config->entry_point == 0) {
        return -1;
    }

    /* Platform-specific domain configuration would go here
     * For RP2350: Configure MPU, memory regions, etc.
     */

    return 0;
}
