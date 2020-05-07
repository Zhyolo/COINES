/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app30.c
 * @date    Mar 26, 2019
 * @brief   COINES support file for APP3.0 MCU
 *
 */

#include <stddef.h>
#include "coines.h"
#include "mcu_app30.h"

uint32_t serial_connected = false;
uint32_t baud_rate = 0;
uint32_t g_millis = 0;

volatile bool tx_pending = false;
static uint32_t prev_millis = 0;

uint8_t multi_io_map[24] = {
    [0 ... 0x0f] = 0,
    /* Native APP3.0 pins */
    [COINES_MINI_SHUTTLE_PIN_1_4] = GPIO_0,
    [COINES_MINI_SHUTTLE_PIN_1_5] = GPIO_1,
    [COINES_MINI_SHUTTLE_PIN_1_6] = GPIO_2,
    [COINES_MINI_SHUTTLE_PIN_1_7] = GPIO_3,
    [COINES_MINI_SHUTTLE_PIN_2_5] = GPIO_4,
    [COINES_MINI_SHUTTLE_PIN_2_6] = GPIO_5,
    [COINES_MINI_SHUTTLE_PIN_2_1] = GPIO_CS,
    [COINES_MINI_SHUTTLE_PIN_2_3] = GPIO_SDO,
};

const nrfx_twim_t i2c_instance = NRFX_TWIM_INSTANCE(I2C_SEN_INSTANCE);
static nrfx_twim_config_t i2c_config = NRFX_TWIM_DEFAULT_CONFIG;
nrfx_twim_xfer_desc_t i2c_tx_desc;

const nrfx_spim_t spi_instance = NRFX_SPIM_INSTANCE(SPI_INSTANCE);
nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
nrfx_spim_xfer_desc_t spi_tx_desc;

static nrfx_gpiote_in_config_t gpio_config = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(true);

flog_write_file_t write_file[MAX_FILE_DESCRIPTORS];
flog_read_file_t  read_file[MAX_FILE_DESCRIPTORS];
volatile bool fd_in_use[MAX_FILE_DESCRIPTORS]={},fd_rw[MAX_FILE_DESCRIPTORS]={};
volatile int file_descriptors_used = 0;

/*!
 *
 * @brief       : API to get shuttle ID
 *
 * @param[in]   : None
 * @return      : shuttle id
 */
static uint16_t get_shuttle_id()
{
    uint16_t shuttle_id = 0;
    NRFX_IRQ_DISABLE(USBD_IRQn);
    app30_eeprom_read(0x01, (uint8_t *)&shuttle_id, 2);
    NRFX_IRQ_ENABLE(USBD_IRQn);
    return shuttle_id;
}
/*!
 * @brief       : API to check communication port connection
 */
void check_com_port_connection(int set)
{
    static uint32_t reset_board = false;

    if (set == 1)
        serial_connected = true;
    else
        serial_connected = false;
    /*
     *  Trigger MCU reset if USB CDC port is opened and closed at 1200bps
     *  https://www.arduino.cc/en/Main/Arduino_BoardLeonardo
     *  See # Automatic (Software) Reset and Bootloader Initiation
     */
    if (set == 1 && (baud_rate == 1200 || baud_rate == 2400))
    {
        reset_board = true;
    }

    if (set == 0 && reset_board == true)
    {
        if (baud_rate == 1200)
            APP_START_ADDR = 0; //Jump to USB DFU Bootloader
        else if (baud_rate == 2400)
            APP_START_ADDR = 0x28000; //Jump to USB MTP Firmware

        memcpy(MAGIC_INFO_ADDR, "COIN", 4); //Write magic string "COIN"
        nrf_delay_ms(100);
        NVIC_SystemReset();
    }

}
/*!
 *
 * @brief       : USB event callback handler
 *
 * @param[in]   : event
 *
 * @return      : None
 */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            break;
        case APP_USBD_EVT_DRV_RESUME:
            break;
        case APP_USBD_EVT_STARTED:
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;
        default:
            break;
    }
}
/*!
 *
 * @brief       :Event handler for USB CDC ACM
 *
 * @param[in]   :* p inst,event
 *
 * @return      : None
 */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{

    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);
    app_usbd_cdc_line_coding_t p_line_encoding = p_cdc_acm->specific.p_data->ctx.request.payload.line_coding;
    baud_rate = *(uint32_t *)&p_line_encoding.dwDTERate[0];

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
            check_com_port_connection(1);
            tx_pending = false;
            break;
        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
            check_com_port_connection(0);
            tx_pending = false;
            break;
        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
            tx_pending = false;
            break;
        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
            break;
        default:
            break;
    }
}
/*!
 * @brief This API is used to initialize the communication according to interface type.
 */
int16_t coines_open_comm_intf(enum coines_comm_intf intf_type)
{
    static const app_usbd_config_t usbd_config = {
            .ev_state_proc = usbd_user_ev_handler
    };

    flog_initialize_params_t params = {
        .number_of_blocks = 1024,
        .pages_per_block = 64,
    };

    nrf_drv_clock_init();
    nrf_drv_power_init(NULL);
    nrf_drv_clock_lfclk_request(NULL);
    nrf_drv_clock_hfclk_request(NULL);

    nrf_gpio_cfg_input(SWITCH1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(SWITCH2, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_output(MCU_LED_R);
    nrf_gpio_cfg_output(MCU_LED_G);
    nrf_gpio_cfg_output(MCU_LED_B);

    nrf_gpio_pin_set(MCU_LED_R);
    nrf_gpio_pin_set(MCU_LED_G);
    nrf_gpio_pin_set(MCU_LED_B);

    while (!nrf_drv_clock_lfclk_is_running() &&
            !nrf_drv_clock_hfclk_is_running());

    app30_eeprom_init();

    app30_eeprom_read(0x60, multi_io_map, 10);

    flogfs_initialize(&params);

    if (flogfs_mount() == FLOG_FAILURE)
    {
        for (uint16_t i = 0; i < 1024; i++)
            W25N01GW_eraseBlock((i * W25N01GW_BLOCK_SIZE) + 1, W25N01GW_BLOCK_SIZE);
        flogfs_format();
    }
    else
    {
        /* Do nothing */
    }

    /*For coines_get_millis() API*/
    SysTick_Config(64000);

    app_usbd_serial_num_generate();
    app_usbd_init(&usbd_config);

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    app_usbd_class_append(class_cdc_acm);
    app_usbd_power_events_enable();

    nrf_gpio_cfg_output(VDD_PS_EN);
    nrf_gpio_cfg_output(VDDIO_PS_EN);
    nrf_gpio_cfg_output(VDD_SEL);

    nrf_gpio_pin_clear(VDD_PS_EN);
    nrf_gpio_pin_clear(VDD_SEL);
    nrf_gpio_pin_clear(VDDIO_PS_EN);

    nrfx_gpiote_init();

    while (!(serial_connected || nrf_gpio_pin_read(SWITCH2) == 0))
    {
        nrf_delay_ms(100);
    }

    return COINES_SUCCESS;
}

/*!
 * @brief This API is used to close the active communication(USB,COM or BLE).
 */
int16_t coines_close_comm_intf(enum coines_comm_intf intf_type)
{
    return COINES_SUCCESS;
}
/*!
 *  @brief This API is used to get the board information.
 */
int16_t coines_get_board_info(struct coines_board_info *data)
{

    if (data != NULL)
    {
        data->board = 5;
        data->hardware_id = 0x11;
        data->shuttle_id = get_shuttle_id();
        data->software_id = 0x10;
        return COINES_SUCCESS;
    }
    else
        return COINES_E_NULL_PTR;

}
/*!
 *  @brief This API is used to configure the pin(MULTIIO/SPI/I2C in shuttle board).
 */
int16_t coines_set_pin_config(enum coines_multi_io_pin pin_number, enum coines_pin_direction direction,
                              enum coines_pin_value pin_value)
{
    nrf_gpio_pin_pull_t pin_pull;

    uint32_t pin_num = multi_io_map[pin_number];
    if (pin_num == 0 || pin_num == 0xff)
        return COINES_E_FAILURE;

    if (direction == COINES_PIN_DIRECTION_IN)
    {
        pin_pull = direction ? NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_PULLDOWN;
        nrf_gpio_cfg_input(pin_num, pin_pull);
    }
    else if (direction == COINES_PIN_DIRECTION_OUT)
    {
        nrf_gpio_cfg_output(pin_num);
        nrf_gpio_pin_write(pin_num, pin_value);

    }

    return COINES_SUCCESS;
}
/*!
 *  @brief This API function is used to get the pin direction and pin state.
 */
int16_t coines_get_pin_config(enum coines_multi_io_pin pin_number, enum coines_pin_direction *pin_direction,
                              enum coines_pin_value *pin_value)
{
    uint32_t pin_num = multi_io_map[pin_number];

    if (pin_num == 0 || pin_num == 0xff)
        return COINES_E_FAILURE;

    if ((pin_value != NULL) || (pin_direction != NULL))
    {
        if (pin_value != NULL)
        {
            *pin_value = nrf_gpio_pin_read(pin_num);
        }

        if (pin_direction != NULL)
        {
            *pin_direction = nrf_gpio_pin_dir_get(pin_num);
        }

        return COINES_SUCCESS;
    }
    else
        return COINES_E_NULL_PTR;

}
/*!
 *  @brief This API function is used to get the pin direction and pin state.
 */
int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt)
{
    if (vdd_millivolt == 0)
    {
        nrf_gpio_pin_write(VDD_PS_EN, 0);
        nrf_gpio_pin_write(VDD_SEL, 0);
    }
    else if ((vdd_millivolt > 0) && (vdd_millivolt <= 1800))
    {
        nrf_gpio_pin_write(VDD_PS_EN, 1);
        nrf_gpio_pin_write(VDD_SEL, 0);
    }
    else
    {
        nrf_gpio_pin_write(VDD_PS_EN, 1);
        nrf_gpio_pin_write(VDD_SEL, 1);
    }

    if (vddio_millivolt == 0)
        nrf_gpio_pin_write(VDDIO_PS_EN, 0);
    else
        nrf_gpio_pin_write(VDDIO_PS_EN, 1);

    return COINES_SUCCESS;

}
/*!
 *  @brief This API is used to configure the SPI bus
 */
int16_t coines_config_spi_bus(enum coines_spi_bus bus, enum coines_spi_speed spi_speed, enum coines_spi_mode spi_mode)
{
    spi_config.miso_pin = SPI_SEN_MISO;
    spi_config.mosi_pin = SPI_I2C_SEN_MOSI;
    spi_config.sck_pin = SPI_I2C_SEN_SCK;
    spi_config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    spi_config.mode = (nrf_spim_mode_t)spi_mode;

    #define COINES_NRF_SPEED_MAP(coines_spi,nrf_spi)  \
        case  COINES_SPI_SPEED_##coines_spi:         \
        spi_config.frequency = NRF_SPIM_FREQ_##nrf_spi;\
        break \

    switch (spi_speed)
    {
            COINES_NRF_SPEED_MAP(250_KHZ,250K);
            COINES_NRF_SPEED_MAP(300_KHZ,250K);

            COINES_NRF_SPEED_MAP(400_KHZ,500K);
            COINES_NRF_SPEED_MAP(500_KHZ,500K);
            COINES_NRF_SPEED_MAP(600_KHZ,500K);

            COINES_NRF_SPEED_MAP(750_KHZ,1M);
            COINES_NRF_SPEED_MAP(1_MHZ,1M);
            COINES_NRF_SPEED_MAP(1_2_MHZ,1M);
            COINES_NRF_SPEED_MAP(1_25_MHZ,1M);

            COINES_NRF_SPEED_MAP(1_5_MHZ,2M);
            COINES_NRF_SPEED_MAP(2_MHZ,2M);
            COINES_NRF_SPEED_MAP(2_5_MHZ,2M);

            COINES_NRF_SPEED_MAP(3_MHZ,4M);
            COINES_NRF_SPEED_MAP(3_75_MHZ,4M);
            COINES_NRF_SPEED_MAP(5_MHZ,4M);
            COINES_NRF_SPEED_MAP(6_MHZ,4M);
            COINES_NRF_SPEED_MAP(7_5_MHZ,4M);
            COINES_NRF_SPEED_MAP(10_MHZ,4M);

            default:
            spi_config.frequency = NRF_SPIM_FREQ_2M;
        }

    nrfx_spim_init(&spi_instance, &spi_config, NULL, NULL);

    return COINES_SUCCESS;
}
/*!
 *  @brief This API is used to configure the I2C bus
 */
int16_t coines_config_i2c_bus(enum coines_i2c_bus bus,
        enum coines_i2c_mode i2c_mode)
{
    // Make SDO Pin Low
    nrf_gpio_cfg_output(SPI_SEN_MISO);
    nrf_gpio_pin_clear(SPI_SEN_MISO);

    nrf_gpio_cfg_input(SPI_I2C_SEN_SCK, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(SPI_I2C_SEN_MOSI, NRF_GPIO_PIN_PULLUP);

    i2c_config.sda = I2C_SEN_SDA;
    i2c_config.scl = I2C_SEN_SCL;

    nrf_gpio_cfg(i2c_config.sda, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT,
                 NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(i2c_config.scl, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT,
                 NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_H0D1, NRF_GPIO_PIN_NOSENSE);

    nrfx_twim_init(&i2c_instance, &i2c_config, NULL, NULL);

    nrfx_twim_enable(&i2c_instance);

    if (i2c_mode == COINES_I2C_STANDARD_MODE)
        nrf_twim_frequency_set(i2c_instance.p_twim, NRF_TWIM_FREQ_100K);
    else
        nrf_twim_frequency_set(i2c_instance.p_twim, NRF_TWIM_FREQ_400K);

    return COINES_SUCCESS;
}
/*!
 *  @brief This API is used to write the data in I2C communication.
 */
int8_t coines_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
        uint16_t count)
{
    nrfx_err_t error;

    uint8_t buffer[count + 1];

    buffer[0] = reg_addr;
    memcpy(&buffer[1], reg_data, count);

    nrfx_twim_xfer_desc_t write_desc = NRFX_TWIM_XFER_DESC_TX(dev_addr,buffer,count+1);

    error = nrfx_twim_xfer(&i2c_instance, &write_desc, NRFX_TWIM_FLAG_TX_POSTINC);

    while (nrfx_twim_is_busy(&i2c_instance));

    if (error == NRFX_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_FAILURE;
}
/*!
 *  @brief This API is used to read the data in I2C communication.
 */
int8_t coines_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
        uint16_t count)
{
    nrfx_err_t error;

    nrfx_twim_xfer_desc_t read_desc = NRFX_TWIM_XFER_DESC_TXRX(dev_addr,&reg_addr,1,reg_data,count);

    error = nrfx_twim_xfer(&i2c_instance, &read_desc, NRFX_TWIM_FLAG_RX_POSTINC | NRFX_TWIM_FLAG_REPEATED_XFER);

    while (nrfx_twim_is_busy(&i2c_instance));

    if (error == NRFX_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_FAILURE;
}
/*!
 *  @brief This API is used to read the data in SPI communication.
 */
int8_t coines_write_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
        uint16_t count)
{
    nrfx_err_t error;
    uint8_t buffer[count + 1];

    uint32_t pin_num = multi_io_map[dev_addr];
    if (pin_num == 0 || pin_num == 0xff)
        return COINES_E_FAILURE;
    else
        nrf_gpio_cfg_output(pin_num);

    buffer[0] = reg_addr;
    memcpy(&buffer[1], reg_data, count);

    spi_tx_desc.p_tx_buffer = buffer;
    spi_tx_desc.tx_length = count + 1;
    spi_tx_desc.p_rx_buffer = NULL;
    spi_tx_desc.rx_length = 0;

    nrf_gpio_pin_write(pin_num, 0);
    error = nrfx_spim_xfer(&spi_instance, &spi_tx_desc, 0);
    nrf_gpio_pin_write(pin_num, 1);

    if (error == NRFX_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_FAILURE;
}
/*!
 *  @brief This API is used to read the data in SPI communication.
 */
int8_t coines_read_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
        uint16_t count)
{
    nrfx_err_t error;
    uint8_t buffer[count + 1];

    uint32_t pin_num = multi_io_map[dev_addr];
    if (pin_num == 0 || pin_num == 0xff)
        return COINES_E_FAILURE;
    else
        nrf_gpio_cfg_output(pin_num);

    buffer[0] = reg_addr;

    spi_tx_desc.p_tx_buffer = &reg_addr;
    spi_tx_desc.tx_length = 1;
    spi_tx_desc.p_rx_buffer = buffer;
    spi_tx_desc.rx_length = count + 1;

    nrf_gpio_pin_write(pin_num, 0);
    error = nrfx_spim_xfer(&spi_instance, &spi_tx_desc, 0);
    nrf_gpio_pin_write(pin_num, 1);

    memcpy(reg_data, &buffer[1], count);

    if (error == NRFX_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_FAILURE;
}
/*!
 *  @brief This API is used for introducing a delay in milliseconds
 */
void coines_delay_msec(uint32_t delay_ms)
{
    nrf_delay_ms(delay_ms);
}

/*!
 *  @brief This API is used for introducing a delay in microseconds
 */
void coines_delay_usec(uint32_t delay_us)
{
    nrf_delay_us(delay_us);
}

/*!
 * @brief This API is used to send the streaming settings to the board.
 */
int16_t coines_config_streaming(uint8_t channel_id, struct coines_streaming_config *stream_config,
        struct coines_streaming_blocks *data_blocks)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to send the streaming settings to the board.
 */
int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode, uint8_t start_stop)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to read the streaming sensor data.
 */
int16_t coines_read_stream_sensor_data(uint8_t sensor_id, uint32_t number_of_samples, uint8_t *data,
                                       uint32_t *valid_samples_count)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API is used to trigger the timer in firmware and enable or disable system time stamp
 */
int16_t coines_trigger_timer(enum coines_timer_config tmr_cfg, enum coines_time_stamp_config ts_cfg)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief This API returns the number of milliseconds passed since the program started
 */
uint32_t coines_get_millis()
{
    return g_millis;
}

/*!
 * @brief Attaches a interrupt to a Multi-IO pin
 */
void coines_attach_interrupt(enum coines_multi_io_pin pin_number, void (*callback)(void),
        enum coines_pin_interrupt_mode int_mode)
{
    uint32_t pin_num = multi_io_map[pin_number];
    if (pin_num == 0 || pin_num == 0xff)
        return;

    if (int_mode == COINES_PIN_INTERRUPT_CHANGE)
        gpio_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    if (int_mode == COINES_PIN_INTERRUPT_RISING_EDGE)
        gpio_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
    if (int_mode == COINES_PIN_INTERRUPT_FALLING_EDGE)
        gpio_config.sense = NRF_GPIOTE_POLARITY_HITOLO;

    nrfx_gpiote_in_init(pin_num, &gpio_config, gpioHandler);
    nrfx_gpiote_in_event_enable(pin_num, true);
    isr_cb[pin_number] = callback;

}

/*!
 *
 * @brief Detaches a interrupt from a Multi-IO pin
 *
 */

void coines_detach_interrupt(enum coines_multi_io_pin pin_number)
{
    uint32_t pin_num = multi_io_map[pin_number];
    if (pin_num == 0 || pin_num == 0xff)
        return;

    /* Cleanup */
    isr_cb[pin_number] = NULL;
    nrfx_gpiote_in_uninit(pin_num);
    nrfx_gpiote_in_event_disable(pin_num);
}


/*!
 * @brief GPIO Interrupt handler
 */
static void gpioHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    for (int i = 0; i < 24; i++)
    {
        if (pin == multi_io_map[i] && isr_cb[i] != NULL)
            isr_cb[i]();
    }
}

/*!
 * @brief SysTick timer handler
 */
void SysTick_Handler(void)
{
    g_millis++;

    if(g_millis % LED_BLINK_MAX_DELAY == 0)
    {
        nrf_gpio_pin_set(MCU_LED_R);
        nrf_gpio_pin_set(MCU_LED_G);
    }
}

/* For stdio fuctions */

int __attribute__((weak)) _write(int fd, const void *buffer, int len)
{
    if (coines_get_millis() - prev_millis > LED_BLINK_MAX_DELAY)
    {
        nrf_gpio_pin_clear(MCU_LED_R);
        prev_millis = coines_get_millis();
    }

    if((fd == 1 || fd == 2) && serial_connected == true)
    {
        while (tx_pending);
        app_usbd_cdc_acm_write(&m_app_cdc_acm, buffer, len);
        tx_pending = true;
        return len;
    }
    else if (fd >= 3)
        return flogfs_write(&write_file[fd - 3], buffer, len);
    else
        return len;
}

int __attribute__((weak)) _read(int fd, void *buffer, int len)
{
    if (coines_get_millis() - prev_millis > LED_BLINK_MAX_DELAY)
    {
        nrf_gpio_pin_clear(MCU_LED_G);
        prev_millis = coines_get_millis();
    }

    if (fd == 0)
    {
        app_usbd_cdc_acm_read(&m_app_cdc_acm, buffer, len);
        return len;
    }
    else
    {
        return flogfs_read(&read_file[fd - 3], buffer, len);
    }
}

int __attribute__((weak)) _open(const char *file_name, int flags)
{
    int fd = -1;
    if (file_descriptors_used > MAX_FILE_DESCRIPTORS)
        return fd;

    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
    {
        if (fd_in_use[i] == false)
        {
            fd = i + 3;
            fd_in_use[i] = true;
            break;
        }
    }

    if (flags == O_RDONLY)
    {
        flogfs_open_read(&read_file[fd - 3], file_name);
        fd_rw[fd - 3] = false;
    }
    else
    {
        flogfs_open_write(&write_file[fd - 3], file_name);
        fd_rw[fd - 3] = true;
    }

    file_descriptors_used++;
    return (fd);

}

int __attribute__((weak)) _close(int fd)
{
    if (fd_rw[fd - 3] == true)
        flogfs_close_write(&write_file[fd - 3]);
    else
        flogfs_close_read(&read_file[fd - 3]);

    fd_in_use[fd - 3] = false;
    file_descriptors_used--;
    return 0;
}

int __attribute__((weak)) _unlink(const char *file_name)
{
    if (flogfs_rm(file_name) == FLOG_SUCCESS)
        return 0;
    else
        return -1;
}
