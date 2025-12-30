/**
 * @file pingpong.c
 * @brief Ping-Pong Example - Bidirectional message passing
 * 
 * Demonstrates:
 * - Bidirectional communication using mailboxes
 * - Message sequencing and acknowledgment
 * - Continuous inter-core message exchange
 */

#include "amp_boot.h"
#include "amp_config.h"
#include "amp_mailbox.h"
#include "amp_shmem.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Shared memory configuration */
#ifndef SHMEM_BASE
#define SHMEM_BASE 0x20040000
#endif
#ifndef SHMEM_SIZE
#define SHMEM_SIZE (16 * 1024)
#endif

#define PING_PONG_COUNT 10

/* Message types */
typedef enum {
    MSG_PING,
    MSG_PONG,
    MSG_DONE
} msg_type_t;

/* Message structure */
typedef struct {
    msg_type_t type;
    uint32_t sequence;
    uint32_t core_id;
} pingpong_msg_t;

/* Shared mailboxes - one for each direction */
static amp_mailbox_t g_mbox_to_core1 = NULL;
static amp_mailbox_t g_mbox_to_core0 = NULL;

/**
 * Core 1 entry point
 */
void core1_main(void)
{
    amp_boot_signal_ready();
    printf("Core 1: Starting ping-pong receiver\n");

    for (int i = 0; i < PING_PONG_COUNT; i++) {
        /* Wait for PING from Core 0 */
        pingpong_msg_t msg;
        if (amp_mailbox_recv(g_mbox_to_core1, &msg, 2000) == 0) {
            if (msg.type == MSG_PING) {
                printf("Core 1: Received PING #%u\n", msg.sequence);

                /* Send PONG response */
                pingpong_msg_t pong = {
                    .type = MSG_PONG,
                    .sequence = msg.sequence,
                    .core_id = 1
                };
                
                amp_mailbox_send(g_mbox_to_core0, &pong, 1000);
                printf("Core 1: Sent PONG #%u\n", pong.sequence);
            }
        } else {
            printf("Core 1: Timeout waiting for PING\n");
            break;
        }
    }

    /* Send completion message */
    pingpong_msg_t done = {
        .type = MSG_DONE,
        .sequence = 0,
        .core_id = 1
    };
    amp_mailbox_send(g_mbox_to_core0, &done, 1000);
    printf("Core 1: Ping-pong complete\n");

    while (1) {
#if defined(__ARM_ARCH) || defined(__arm__)
        __asm__ volatile("wfi");
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
    printf("=== Ping-Pong Example ===\n");
    printf("Core 0: Initializing...\n");

    /* Initialize shared memory */
    if (amp_shmem_init((void *)SHMEM_BASE, SHMEM_SIZE) != 0) {
        printf("Core 0: Failed to initialize shared memory\n");
        return 1;
    }

    /* Initialize AMP */
    if (amp_boot_init() != AMP_BOOT_SUCCESS) {
        printf("Core 0: Failed to initialize AMP\n");
        return 1;
    }

    /* Create bidirectional mailboxes */
    amp_mailbox_config_t mbox_config = {
        .msg_size = sizeof(pingpong_msg_t),
        .msg_slots = 4
    };

    g_mbox_to_core1 = amp_mailbox_create(&mbox_config);
    g_mbox_to_core0 = amp_mailbox_create(&mbox_config);

    if (!g_mbox_to_core1 || !g_mbox_to_core0) {
        printf("Core 0: Failed to create mailboxes\n");
        return 1;
    }

    printf("Core 0: Starting Core 1...\n");

    /* Boot core 1 */
    #ifdef PLATFORM_RP2350
    extern void multicore_launch_core1(void (*entry)(void));
    multicore_launch_core1(core1_main);
    #else
    amp_boot_core(AMP_CORE1, (uint32_t)(uintptr_t)core1_main, 0);
    #endif

    /* Wait for core 1 */
    if (amp_boot_wait_core_ready(AMP_CORE1, 1000) != AMP_BOOT_SUCCESS) {
        printf("Core 0: Timeout waiting for Core 1\n");
        return 1;
    }

    printf("Core 0: Starting ping-pong exchange...\n");

    /* Send PINGs and wait for PONGs */
    for (uint32_t i = 0; i < PING_PONG_COUNT; i++) {
        /* Send PING */
        pingpong_msg_t ping = {
            .type = MSG_PING,
            .sequence = i,
            .core_id = 0
        };

        if (amp_mailbox_send(g_mbox_to_core1, &ping, 1000) != 0) {
            printf("Core 0: Failed to send PING #%u\n", i);
            break;
        }
        printf("Core 0: Sent PING #%u\n", i);

        /* Wait for PONG */
        pingpong_msg_t pong;
        if (amp_mailbox_recv(g_mbox_to_core0, &pong, 2000) == 0) {
            if (pong.type == MSG_PONG && pong.sequence == i) {
                printf("Core 0: Received PONG #%u\n", pong.sequence);
            }
        } else {
            printf("Core 0: Timeout waiting for PONG #%u\n", i);
            break;
        }
    }

    /* Wait for done message */
    pingpong_msg_t msg;
    if (amp_mailbox_recv(g_mbox_to_core0, &msg, 2000) == 0) {
        if (msg.type == MSG_DONE) {
            printf("Core 0: Received completion from Core 1\n");
        }
    }

    printf("Core 0: Ping-pong complete!\n");
    printf("=========================\n");

    return 0;
}
