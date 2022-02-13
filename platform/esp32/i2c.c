/**
 * @file i2c.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief i2c implementation for "esp32"
 * @version 0.1
 * @date 2022-02-13
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/i2c.h"

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

/**
 * @brief General maximum delay for various operations.
 *
 * Not all platform interfaces support a timeout argument. For those this
 * maximum delay is used.
 */
#define FAIL_DELAY pdMS_TO_TICKS(10)

/**
 * @brief State of each i2c bus port.
 *
 */
static struct {
    SemaphoreHandle_t lock; //!< lock for api calls
    int driver_installed;   //!< 1 if driver was installed on port
} g_i2c_esp32[I2C_NUM_MAX];


/*
 * Private function prototypes
 *
 */

/**
 * @brief Generic read or write to the i2c bus.
 *
 * Read and write operations are almost identical with esp-idf except a few
 * settings. This enables us to combine the two.
 *
 * @param port bus port number
 * @param address address of the slave
 * @param[in] data pointer to buffer of data (to be sent if \par write = 1, or
 *                 where to save if \par write = 0)
 * @param length how many bytes of data to send or receive
 * @param ms_timeout milliseconds until operation timeouts
 * @param write 0 = read / receive, 1 = write / send
 * @retval
 */
int read_or_write(int port, uint8_t address, uint8_t *data, size_t length,
                  uint32_t ms_timeout, int write);


/*
 * Implementation of public functions
 *
 */

int i2c_init(int port, int sda, int scl, int clock) {
    assert(port >= 0 && port < I2C_NUM_MAX);
    assert(sda >= 0 && sda < GPIO_NUM_MAX);
    assert(scl >= 0 && scl < GPIO_NUM_MAX);
    assert(sda != scl);
    if (!g_i2c_esp32[port].lock) {
        // api lock doesnt exist already, create it
        g_i2c_esp32[port].lock = xSemaphoreCreateMutex();
    }
    assert(pdTRUE == xSemaphoreTake(g_i2c_esp32[port].lock, FAIL_DELAY));
    if (g_i2c_esp32[port].driver_installed) {
        // driver was already installed on this port, deinstall it
        assert(ESP_OK == i2c_driver_delete(port));
        g_i2c_esp32[port].driver_installed = 0;
    }
    esp_err_t err;
    err = i2c_param_config(port,
                           &(i2c_config_t){.mode = I2C_MODE_MASTER,
                                           .sda_io_num = sda,
                                           .sda_pullup_en = GPIO_PULLUP_ENABLE,
                                           .scl_io_num = scl,
                                           .scl_pullup_en = GPIO_PULLUP_ENABLE,
                                           .master = {.clk_speed = clock}});
    if (err == ESP_OK) {
        err = i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);
        if (err == ESP_OK) {
            g_i2c_esp32[port].driver_installed = 1;
        }
    }
    xSemaphoreGive(g_i2c_esp32[port].lock);
    return (err == ESP_OK) ? 0 : EIO;
}

int i2c_deinit(int port) {
    assert(port >= 0 && port < I2C_NUM_MAX);
    if (g_i2c_esp32[port].lock) {
        assert(g_i2c_esp32[port].lock);
        xSemaphoreTake(g_i2c_esp32[port].lock, FAIL_DELAY);
        vSemaphoreDelete(g_i2c_esp32[port].lock);
        g_i2c_esp32[port].lock = NULL;
    }
    if (g_i2c_esp32[port].driver_installed) {
        i2c_driver_delete(port);
        g_i2c_esp32[port].driver_installed = 0;
    }
    return 0;
}

int i2c_send(int port, uint8_t address, uint8_t *data, size_t length,
             uint32_t ms_timeout) {
    return read_or_write(port, address, data, length, ms_timeout, 1);
}

int i2c_receive(int port, uint8_t address, uint8_t *data, size_t length,
                uint32_t ms_timeout) {
    return read_or_write(port, address, data, length, ms_timeout, 0);
}


/*
 * Implementation of private functions
 *
 */

int read_or_write(int port, uint8_t address, uint8_t *data, size_t length,
                  uint32_t ms_timeout, int write) {
    assert(port >= 0 && port < I2C_NUM_MAX);
    assert(g_i2c_esp32[port].driver_installed);
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t timeout = pdMS_TO_TICKS(ms_timeout);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    if (write) {
        i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, true);
        i2c_master_write(cmd, data, length, true);
    } else {
        i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_READ, true);
        i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    }
    i2c_master_stop(cmd);
    if (pdFALSE == xSemaphoreTake(g_i2c_esp32[port].lock, timeout)) {
        return ETIME;
    }
    if (portMAX_DELAY != timeout) {
        TickType_t delta = xTaskGetTickCount() - start_tick;
        if (delta >= timeout) {
            timeout = 0;
        } else {
            timeout -= delta;
        }
    }
    esp_err_t err = i2c_master_cmd_begin(port, cmd, timeout);
    xSemaphoreGive(g_i2c_esp32[port].lock);
    i2c_cmd_link_delete(cmd);
    if (ESP_OK == err) {
        return 0;
    } else if (ESP_ERR_TIMEOUT == err) {
        return ETIME;
    } else {
        return EIO;
    }
}
