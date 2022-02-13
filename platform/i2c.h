/**
 * @file i2c.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief I2C abstraction
 * @version 0.1
 * @date 2022-02-07
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
 * @brief Initialize the i2c bus as master on the selected port and pins.
 *
 * @param port bus port number
 * @param sda pin number for the data line
 * @param scl pin number for the clock line
 * @param clock clock frequency
 * @retval EIO hardware error
 */
int i2c_init(int port, int sda, int scl, int clock);

/**
 * @brief Deinitialize i2c bus.
 *
 * @param port bus port number
 * @retval EIO hardware error
 */
int i2c_deinit(int port);

/**
 * @brief Send data as master to slave.
 *
 * @param port bus port number
 * @param address address of the slave
 * @param[in] data pointer to buffer of data that gets sent
 * @param length how many bytes of data to send
 * @param ms_timeout milliseconds until sending timeouts
 * @retval EIO bus error
 * @retval ETIME timeout
 */
int i2c_send(int port, uint8_t address, uint8_t *data, size_t length,
             uint32_t ms_timeout);

/**
 * @brief Receive data as master from slave.
 *
 * @param port bus port number
 * @param address address of the slave
 * @param[out] data pointer to buffer that gets filled with received data
 * @param length how many bytes of data to receive
 * @param ms_timeout milliseconds until receiving timeouts
 * @retval EIO bus error
 * @retval ETIME timeout
 */
int i2c_receive(int port, uint8_t address, uint8_t *data, size_t length,
                uint32_t ms_timeout);
