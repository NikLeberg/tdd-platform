/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform implementation for "carme-m4"
 * @version 0.1
 * @date 2021-10-03
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include <stdio.h>

#include "stm32f4xx.h" // Processor is STM32F407IG
#include "carme.h"     // CARME Module
#include "carme_io1.h"
#include "uart.h" // CARME BSP UART port

#include "FreeRTOS.h"
#include "task.h"

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

/* ... */


/*
 * Implementation of private functions
 *
 */

/**
 * @brief C entry point.
 *
 * In the startup.s assembler code this function gets called after initializing
 * static variables and basic system functionality. Setup a few other things
 * e.g. UART and then start a FreeRTOS task for the application_main() function.
 *
 */
int main() {
    // Ensure all priority bits are assigned as preemption priority bits.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Initialize CARME-M4 Kit and clear all LEDs
    CARME_IO1_Init();
    CARME_IO1_LED_Set(0, 0xFF);

    // Initialize UART for stdout
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_InitStruct);

    // Start the main application as a task.
    xTaskCreate((TaskFunction_t)application_main, "main",
                configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    // Start scheduler, will never return.
    vTaskStartScheduler();

    // Code never reached
    while (1) {
    }
    return 0;
}
