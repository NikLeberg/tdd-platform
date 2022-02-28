/**
 * @file task.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Multitasking implementation for "esp32"
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "platform/platform.h"


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
 * Private function prototypes
 *
 */

/* ... */


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
    assert(priority <= configMAX_PRIORITIES);
    assert(stackDepth >= configMINIMAL_STACK_SIZE);
    if (xTaskCreate(function, NULL, stackDepth, argument, priority,
                    (TaskHandle_t *)task) != pdPASS) {
        return ENOMEM;
    }
    return 0;
}

void task_delete(task_handle_t task) {
    vTaskDelete((TaskHandle_t)task);
}

void task_delay(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t task_getMs() {
    if (xPortInIsrContext()) {
        return pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    } else {
        return pdTICKS_TO_MS(xTaskGetTickCount());
    }
}

int task_send(task_handle_t task, uint32_t value, uint32_t msWait) {
    if (xPortInIsrContext()) {
        BaseType_t woken;
        BaseType_t result = xTaskNotifyFromISR(
            (TaskHandle_t)task, value, eSetValueWithoutOverwrite, &woken);
        portYIELD_FROM_ISR(woken);
        if (result == pdPASS) {
            return 0;
        } else {
            return ETIME;
        }
    } else {
        TickType_t ticksStart = xTaskGetTickCount();
        do {
            if (xTaskNotify((TaskHandle_t)task, value,
                            eSetValueWithoutOverwrite) == pdPASS) {
                return 0;
            }
            vTaskDelay(1);
        } while (pdTICKS_TO_MS(xTaskGetTickCount() - ticksStart) < msWait);
        return ETIME;
    }
}

int task_receive(uint32_t *value, uint32_t msWait) {
    return (xTaskNotifyWait(0, UINT32_MAX, value, pdMS_TO_TICKS(msWait)) ==
            pdTRUE)
               ? 0
               : ETIME;
}


/*
 * Implementation of private functions
 *
 */

/* ... */
