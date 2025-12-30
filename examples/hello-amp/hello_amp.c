/**
 * @file hello_amp.c
 * @brief Hello AMP - Basic dual-core initialization example
 * 
 * Demonstrates:
 * - Boot sequence and core identification
 * - Basic inter-core communication via mailbox
 * - Core synchronization
 */

#include "amp_boot.h"
#include "amp_config.h"
#include "amp_mailbox.h"
#include "amp_shmem.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Shared memory base and size - platform specific */
#ifndef SHMEM_BASE
#define SHMEM_BASE 0x20040000
#endif
#ifndef SHMEM_SIZE
#define SHMEM_SIZE (16 * 1024)  /* 16KB */
#endif

/* Message structure */
typedef struct {
    uint32_t core_id;
    char message[56];
} hello_msg_t;

/* Shared mailbox */
static amp_mailbox_t g_mailbox = NULL;

/**
 * Core 1 entry point
 */
void core1_main(void)
{
    /* Signal that core 1 is ready */
    amp_boot_signal_ready();

    printf("Core 1: Hello from secondary core!\n");

    /* Send greeting to core 0 */
    hello_msg_t msg = {
        .core_id = 1,
    };
    snprintf(msg.message, sizeof(msg.message), "Hello from Core 1!");

    if (amp_mailbox_send(g_mailbox, &msg, 1000) == 0) {
        printf("Core 1: Message sent to Core 0\n");
    }

    /* Wait for response */
    hello_msg_t response;
    if (amp_mailbox_recv(g_mailbox, &response, 1000) == 0) {
        printf("Core 1: Received: '%s' from Core %u\n", 
               response.message, response.core_id);
    }

    printf("Core 1: Done!\n");
    
    /* Core 1 work complete */
    while (1) {
#if defined(__ARM_ARCH) || defined(__arm__)
        __asm__ volatile("wfi");  /* Wait for interrupt */
#else
        /* Busy wait on non-ARM platforms */
#endif
    }
}

/**
 * Main function - runs on Core 0
 */
int main(void)
{
    printf("=== Hello AMP Example ===\n");
    printf("Core 0: Starting AMP initialization\n");

    /* Initialize shared memory */
    if (amp_shmem_init((void *)SHMEM_BASE, SHMEM_SIZE) != 0) {
        printf("Core 0: Failed to initialize shared memory\n");
        return 1;
    }

    /* Initialize AMP boot system */
    if (amp_boot_init() != AMP_BOOT_SUCCESS) {
        printf("Core 0: Failed to initialize AMP boot\n");
        return 1;
    }

    /* Create mailbox for communication */
    amp_mailbox_config_t mbox_config = {
        .msg_size = sizeof(hello_msg_t),
        .msg_slots = 4
    };
    g_mailbox = amp_mailbox_create(&mbox_config);
    if (!g_mailbox) {
        printf("Core 0: Failed to create mailbox\n");
        return 1;
    }

    printf("Core 0: Booting Core 1...\n");

    /* Boot core 1 - platform specific implementation needed */
    /* This is a placeholder - actual implementation in platform layer */
    #ifdef PLATFORM_RP2350
    extern void multicore_launch_core1(void (*entry)(void));
    multicore_launch_core1(core1_main);
    #else
    /* Generic fallback - requires platform-specific boot implementation */
    amp_boot_core(AMP_CORE1, (uint32_t)(uintptr_t)core1_main, 0);
    #endif

    /* Wait for core 1 to be ready */
    if (amp_boot_wait_core_ready(AMP_CORE1, 1000) != AMP_BOOT_SUCCESS) {
        printf("Core 0: Timeout waiting for Core 1\n");
        return 1;
    }

    printf("Core 0: Core 1 is ready!\n");

    /* Receive message from core 1 */
    hello_msg_t msg;
    if (amp_mailbox_recv(g_mailbox, &msg, 1000) == 0) {
        printf("Core 0: Received: '%s' from Core %u\n", 
               msg.message, msg.core_id);
    }

    /* Send response */
    hello_msg_t response = {
        .core_id = 0,
    };
    snprintf(response.message, sizeof(response.message), "Hello from Core 0!");
    
    if (amp_mailbox_send(g_mailbox, &response, 1000) == 0) {
        printf("Core 0: Response sent to Core 1\n");
    }

    printf("Core 0: Example complete!\n");
    printf("=========================\n");

    return 0;
}
