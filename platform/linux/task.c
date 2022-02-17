/**
 * @file task.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Multitasking implementation for "linux"
 * @version 0.1
 * @date 2022-01-05
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>

#include "platform/platform.h"


/*
 * Type declarations
 *
 */

/**
 * @brief Internal structure to represent a linux task.
 *
 */
typedef struct task_linux_s {
    pthread_t thread;  //!< POSIX thread handle.
    sem_t lockSend;    //!< Lock for send operation.
    sem_t lockReceive; //!< Lock for receive operation.
    uint32_t message;  //!< Message that the task received.
} task_linux_t;


/*
 * Variable declarations
 *
 */

/**
 * @brief Number of concurrent tasks.
 *
 */
#define MAX_TASKS (24U)

/**
 * @brief Array of variables for concurrent tasks.
 *
 */
task_linux_t g_tasks[MAX_TASKS];


/*
 * Private function prototypes
 *
 */

/**
 * @brief Get a free task from the global task array.
 *
 * @return task_linux_t* free task or NULL on error.
 */
task_linux_t *getFreeTask();

/**
 * @brief Get a task by its POSIX thread handle.
 *
 * @param pthreadID POSIX thread handle to find.
 * @return task_linux_t* found task or NULL on error.
 */
task_linux_t *getTaskByPthreadID(pthread_t pthreadID);

/**
 * @brief Take a semaphore.
 *
 * @param semaphor semaphore to take, will have a value of 0 on exit.
 * @param msWait max timeout in miliseconds.
 * @retval 0 on success
 * @retval ETIME on timeout
 */
int semaphoreTake(sem_t *semaphor, uint32_t msWait);

/**
 * @brief Give s semaphore.
 *
 * @param semaphor semaphore to give, will have a value of 1 on exit.
 */
void semaphoreGive(sem_t *semaphor);


/*
 * Implementation of public functions
 *
 */

int task_create(task_handle_t *task, task_function_t function, void *argument,
                int priority, int stackDepth) {
    if (task) {
        *task = NULL;
    }
    assert(function);
    // Linux supports thread priorities only on realtime schedulers. And as this
    // would anyway only make a difference if we would use the CPU to 100 %,
    // ignore priority for now.
    (void)priority;
    // Initialize task attributes with (minimal) stack size and scheduler
    // priority. The stack is allowed to grow if it needs to. This isn't
    // possible with other implementations e.g. FreeRTOS.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    size_t stackSize = stackDepth * sizeof(void *);
    int ret = pthread_attr_setstacksize(&attr, stackSize);
    assert(ret == 0);
    task_linux_t *linuxTask = getFreeTask();
    if (!linuxTask) {
        return ENOMEM;
    }
    // Create a new tread. This casts the function return to a (void*). This
    // isn't pretty but as task functions shouldn't return anyway it should not
    // cause any problems.
    if (pthread_create(&linuxTask->thread, &attr, (void *(*)(void *))function,
                       argument)) {
        // Do so as if there was no memory. But could be that there were just
        // too many threads running or no permission.
        return ENOMEM;
    }
    pthread_attr_destroy(&attr);
    // Initialize semaphores for use in receive() and send().
    sem_init(&linuxTask->lockSend, 0, 1);    // sending is unlocked
    sem_init(&linuxTask->lockReceive, 0, 0); // receiving is locked
    if (task) {
        *task = (task_handle_t)linuxTask;
    }
    return 0;
}

void task_delete(task_handle_t task) {
    task_linux_t *linuxTask = (task_linux_t *)task;
    if (!linuxTask) {
        linuxTask = getTaskByPthreadID(pthread_self());
    }
    pthread_t thread = linuxTask->thread;
    linuxTask->thread = 0;
    pthread_cancel(thread);
}

void task_delay(uint32_t ms) {
    assert(ms > 0);
    usleep(1000 * ms);
}

uint32_t task_getMs() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return (spec.tv_sec * 1000) + (spec.tv_nsec / 1000000);
}

int task_send(task_handle_t task, uint32_t value, uint32_t msWait) {
    assert(task);
    task_linux_t *linuxTask = (task_linux_t *)task;
    // Try to take the send semaphore. Is locked if there is already a message
    // pending for the task.
    if (semaphoreTake(&linuxTask->lockSend, msWait)) {
        return ETIME;
    }
    // We are allowed to set the message. Set it and unlock receiver.
    linuxTask->message = value;
    semaphoreGive(&linuxTask->lockReceive);
    return 0;
}

int task_receive(uint32_t *value, uint32_t msWait) {
    task_linux_t *linuxTask = getTaskByPthreadID(pthread_self());
    // Try to take the receive semaphore. Is locked if there is no message
    // pending for the task.
    if (semaphoreTake(&linuxTask->lockReceive, msWait)) {
        return ETIME;
    }
    // We are allowed to read the message. Set it and unlock sender.
    *value = linuxTask->message;
    semaphoreGive(&linuxTask->lockSend);
    return 0;
}


/*
 * Implementation of private functions
 *
 */

task_linux_t *getFreeTask() {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (!g_tasks[i].thread) {
            return &g_tasks[i];
        }
    }
    return NULL;
}

task_linux_t *getTaskByPthreadID(pthread_t pthreadID) {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (pthread_equal(pthreadID, g_tasks[i].thread)) {
            return &g_tasks[i];
        }
    }
    return NULL;
}

int semaphoreTake(sem_t *semaphor, uint32_t msWait) {
    if (msWait == UINT32_MAX) {
        // Take the semaphore, wait indefinitely.
        sem_wait(semaphor);
    } else {
        // Note the current time.
        struct timespec startTime;
        clock_gettime(CLOCK_REALTIME, &startTime);
        // Calculate the timeout as an absolute time in the future.
        struct timespec timeout = startTime;
        timeout.tv_sec += msWait / 1000;
        timeout.tv_nsec += (msWait % 1000) * 1000000;
        if (timeout.tv_nsec >= 1000000000) {
            timeout.tv_nsec -= 1000000000;
            timeout.tv_sec++;
        }
        // Try to take the semaphore.
        if (sem_timedwait(semaphor, &timeout)) {
            return ETIME;
        }
    }
    return 0;
}

void semaphoreGive(sem_t *semaphor) {
    sem_post(semaphor);
}
