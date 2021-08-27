/**
 * @file platform.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform abstraction
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * The platform implementation may provide following POSIX-apis:
 * - C-Standard:
 *      - <stdlib.h>
 *      - <stdbool.h>
 *      - <stddef.h>
 *      - <stdint.h>
 *      - <stdio.h>
 *      - <string.h>
 * - Threads & Semaphores:
 *      - <pthread.h>
 *      - <semaphore.h>
 * - Berkley Sockets:
 *      - <arpa/inet.h>
 *      - <net/if.h>
 *      - <netdb.h>
 *      - <netinet/in.h>
 *      - <netinet/tcp.h>
 *
 * Additionally GPIO and peripheral APIs according to the definitions in this
 * header may be provided.
 *
 * If a platform can't provide such functionality, the API has to be stubbed out
 * and must return sensible errors or trigger an assert.
 */

#pragma once


/*
 * Includes
 *
 */

/* ... */


/*
 * Type declarations
 *
 */

/* ... */


/*
 * Variable declarations
 *
 */

/* ... */


/*
 * Public functions
 *
 */

/**
 * @brief Main entrypoint of the application.
 *
 * @return int errorlevel
 */
int main();
