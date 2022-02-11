/**
 * @file task.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Multitasking abstraction
 * @version 0.1
 * @date 2022-01-05
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
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

/**
 * @brief Opaque task handle.
 *
 */
typedef void *task_handle_t;

/**
 * @brief Prototype of a task function.
 *
 */
typedef void (*task_function_t)(void *);


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
 * @brief Create and start a new task.
 *
 * @param[out] task Handle of the created task, NULL on error.
 * @param function tasks "main" function, must never return.
 * @param argument optional argument to the task function.
 * @param priority priority at which the task will run, 0 being the smallest.
 * @param stackDepth size of the task stack.
 * @retval 0 on success
 * @retval EAGAIN there are already a maximum of tasks running.
 * @retval ENOMEM not enough free memory for task stack.
 */
int task_create(task_handle_t *task, task_function_t function, void *argument,
                int priority, int stackDepth);

/**
 * @brief Stop and delete a task.
 *
 * @param task Task to delete, NULL for the calling task.
 */
void task_delete(task_handle_t task);

/**
 * @brief Delays the calling task.
 *
 * @param ms how many milliseconds the task should be delayed.
 */
void task_delay(uint32_t ms);

/**
 * @brief Get the current system time in milliseconds.
 *
 * @return uint32_t current system time.
 */
uint32_t task_getMs();

/**
 * @brief Try to send a message / value to a task.
 *
 * @note Sending to a task with a higher priority that waits for a message might
 * unlock it immediately and function will initiate a context switch.
 *
 * @param task Handle of the task that should receive the message.
 * @param value Message to send.
 * @param msWait How long to wait if there is no space for the message, use
 *               UINT32_MAX for an indefinite wait.
 * @retval 0 on success
 * @retval ETIME on timeout
 */
int task_send(task_handle_t task, uint32_t value, uint32_t msWait);

/**
 * @brief Try to receive a message / value for the calling task.
 *
 * @param value
 * @param msWait
 * @return int
 */
int task_receive(uint32_t *value, uint32_t msWait);
