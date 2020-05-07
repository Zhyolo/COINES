/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mcu_app20.c
 * @date    Jan 2, 2019
 * @brief   COINES support file for APP2.0 MCU
 *
 */

/**********************************************************************************/
/* system header includes */
/**********************************************************************************/
#include <stddef.h>
#include "coines.h"
#include "mcu_app20.h"

/**********************************************************************************/
/* variable declarations */
/**********************************************************************************/
uint32_t serial_connected = false;
uint32_t baud_rate = 0;
uint32_t g_millis = 0;

typedef void (*ISR_CB)(void);

static ISR_CB isr_cb[9];

/**********************************************************************************/
/* static function declaration */
/**********************************************************************************/
/*!
 *
 * @brief       : GPIO Interrupt Handler
 *
 * @param[in]   : id,index
 *
 * @return      : None
 */
static void pioHandler(uint32_t id, uint32_t index);

/**********************************************************************************/
/* functions */
/**********************************************************************************/
/*!
 *
 * @brief       : API to get the hardware pin
 *
 */
static int32_t get_hw_pin(enum coines_multi_io_pin shuttle_pin)
{

#define PIN_MAP(sp,hp) \
        case COINES_SHUTTLE_PIN_##sp:  \
        return SHUTTLE_##hp		   \

    switch (shuttle_pin)
    {

        PIN_MAP(7,SPI_CS0);
        PIN_MAP(8,IO5);
        PIN_MAP(9,IO0);
        PIN_MAP(14,IO1);
        PIN_MAP(15,IO2);
        PIN_MAP(16,IO3);
        PIN_MAP(19,IO8);
        PIN_MAP(20,IO6);
        PIN_MAP(21,IO7);
        PIN_MAP(22,IO4);

    }

    return COINES_E_NOT_SUPPORTED;

}
/*!
*
* @brief       : API to get the shuttle ID
*/
static uint16_t get_shuttle_id()
{
    uint16_t shuttle_id = 0;

    shuttle_id |= pio_get_pin_value(SHUTTLE_COD0) << 0;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD1) << 1;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD2) << 2;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD3) << 3;

    shuttle_id |= pio_get_pin_value(SHUTTLE_COD4) << 4;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD5) << 5;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD6) << 6;
    shuttle_id |= pio_get_pin_value(SHUTTLE_COD7) << 7;

    shuttle_id |= pio_get_pin_value(SHUTTLE_COD8) << 8;

    return shuttle_id;
}
/*!
 *
 * @brief       : This API is to check communication port connection
 *
 */
void check_com_port_connection(int set)
{
    static uint32_t reset_board = false;

    if (set == 1)
    {
        serial_connected = true;
    }
    /*
     *  Trigger MCU reset if USB CDC port is opened and closed at 1200bps
     *  https://www.arduino.cc/en/Main/Arduino_BoardLeonardo
     *  See # Automatic (Software) Reset and Bootloader Initiation
     */
    if (set == 1 && baud_rate == 1200)
    {
        reset_board = true;
    }

    if (set == 0 && reset_board == true && baud_rate == 1200)
    {
        delay_ms(100);
        APP_START_ADDR = 0; //Jump to USB DFU Bootloader
        rstc_start_software_reset(RSTC);
    }

}

void get_connection_parameters(usb_cdc_line_coding_t *cfg)
{
    baud_rate = cfg->dwDTERate;
}
/*!
 * @brief This API is used to initialize the communication according to interface type.
 *
 */
int16_t coines_open_comm_intf(enum coines_comm_intf intf_type)
{

    sysclk_init();
    wdt_disable(WDT);

    /*For coines_get_millis() API*/
    SysTick_Config(sysclk_get_peripheral_hz()/1000);

    sysclk_enable_peripheral_clock(ID_PIOA);
    sysclk_enable_peripheral_clock(ID_PIOB);
    sysclk_enable_peripheral_clock(ID_PIOC);

    pio_configure_pin(SHUTTLE_VDD_EN, PIO_OUTPUT_0);
    pio_configure_pin(SHUTTLE_VDDIO_EN, PIO_OUTPUT_0);

    pio_configure_pin(SHUTTLE_COD0, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD1, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD2, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD3, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD4, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD5, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD6, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD7, PIO_INPUT);
    pio_configure_pin(SHUTTLE_COD8, PIO_INPUT);

    /* For coines_attach_interrupt() API*/
    NVIC_DisableIRQ(PIOC_IRQn);
    NVIC_ClearPendingIRQ(PIOC_IRQn);
    NVIC_SetPriority(PIOC_IRQn, 6);

    irq_initialize_vectors();
    cpu_irq_enable();
    stdio_usb_init();

    while (!serial_connected)
    {
        delay_ms(100);
    }
    return COINES_SUCCESS;
}
/*!
 *
 * @brief 		This API is used to close the active communication(USB,COM or BLE).
 *
 */
int16_t coines_close_comm_intf(enum coines_comm_intf intf_type)
{
    fflush(stdout);
    return COINES_SUCCESS;
}
/*!
 *  @brief This API is used to get the board information.
 *
 *
 */
int16_t coines_get_board_info(struct coines_board_info *data)
{

    if (data != NULL)
    {
        data->board = 3;
        data->hardware_id = 0x11;
        data->shuttle_id = get_shuttle_id();
        data->software_id = 0x01;
        return COINES_SUCCESS;
    }
    else
        return COINES_E_NULL_PTR;

}
/*!
 * @brief 	This API is used to close the active communication(USB,COM or BLE).
 *
 *
 */
int16_t coines_set_pin_config(enum coines_multi_io_pin pin_number, enum coines_pin_direction direction, enum coines_pin_value pin_value)
{

    if (direction == COINES_PIN_DIRECTION_IN)
    {

        if (pin_value == COINES_PIN_VALUE_LOW)
            pio_configure_pin(get_hw_pin(pin_number), PIO_INPUT | 0);
        else if (pin_value == COINES_PIN_VALUE_HIGH)
            pio_configure_pin(get_hw_pin(pin_number), PIO_INPUT | PIO_PULLUP);
    }

    else if (direction == COINES_PIN_DIRECTION_OUT)
    {

        if (pin_value == COINES_PIN_VALUE_LOW)
            pio_configure_pin(get_hw_pin(pin_number), PIO_OUTPUT_0);
        else if (pin_value == COINES_PIN_VALUE_HIGH)
            pio_configure_pin(get_hw_pin(pin_number), PIO_OUTPUT_1);
    }

    return COINES_SUCCESS;
}
/*!
 *
 *  @brief This API function is used to get the pin direction and pin state.
 *
 */
int16_t coines_get_pin_config(enum coines_multi_io_pin pin_number, enum coines_pin_direction *pin_direction, enum coines_pin_value *pin_value)
{
    if ((pin_value != NULL) || (pin_direction != NULL))
    {
        if (pin_value != NULL)
        {
            if (pio_get_pin_value(get_hw_pin(pin_number)) == 0)
                *pin_value = COINES_PIN_VALUE_LOW;
            else
                *pin_value = COINES_PIN_VALUE_HIGH;

        }

        if (pin_direction != NULL)
        {
            *pin_direction = COINES_PIN_DIRECTION_IN; //TODO : Temporary
        }

        return COINES_SUCCESS;
    }
    else
        return COINES_E_NULL_PTR;

}

/*!
 *  @brief This API is used to configure the VDD and VDDIO of the sensor.
 *
 */
int16_t coines_set_shuttleboard_vdd_vddio_config(uint16_t vdd_millivolt, uint16_t vddio_millivolt)
{
    if (vdd_millivolt > 0)
        pio_set_pin_high(SHUTTLE_VDD_EN);
    else
        pio_set_pin_low(SHUTTLE_VDD_EN);

    if (vddio_millivolt > 0)
        pio_set_pin_high(SHUTTLE_VDDIO_EN);
    else
        pio_set_pin_low(SHUTTLE_VDDIO_EN);

    return COINES_SUCCESS;

}
/*!
 *  @brief This API is used to configure the SPI bus
 *
 */
int16_t coines_config_spi_bus(enum coines_spi_bus bus, enum coines_spi_speed spi_speed, enum coines_spi_mode spi_mode)
{

    struct spi_device device;
    device.id = 0;

    sysclk_enable_peripheral_clock(ID_SPI);
    sysclk_disable_peripheral_clock(ID_TWI0);

    pio_configure(PIOA, PIO_PERIPH_A, PIO_PA12A_MISO, 1);
    pio_configure(PIOA, PIO_PERIPH_A, PIO_PA13A_MOSI, 1);
    pio_configure(PIOA, PIO_PERIPH_A, PIO_PA14A_SPCK, 1);

    spi_master_init(SPI);
    spi_master_setup_device(SPI, &device, spi_mode, sysclk_get_peripheral_hz() / spi_speed, device.id);
    spi_enable(SPI);

    return COINES_SUCCESS;
}
/*!
 *  @brief This API is used to configure the I2C bus
 *
 */
int16_t coines_config_i2c_bus(enum coines_i2c_bus bus,
                              enum coines_i2c_mode i2c_mode)
{
    int16_t rslt;

    twi_options_t i2c_config;
    i2c_config.master_clk = sysclk_get_peripheral_hz();

    sysclk_enable_peripheral_clock(ID_TWI0);
    sysclk_disable_peripheral_clock(ID_SPI);

    pio_configure(PIOA, PIO_PERIPH_A, PIO_PA3A_TWD0, 1);
    pio_configure(PIOA, PIO_PERIPH_A, PIO_PA4A_TWCK0, 1);

    //Make SDO Pin Low
    pio_configure_pin(PIO_PA12_IDX, PIO_OUTPUT_0);

    if (i2c_mode == COINES_I2C_STANDARD_MODE)
        i2c_config.speed = 100000;
    else if (i2c_mode == COINES_I2C_FAST_MODE)
        i2c_config.speed = 400000;
    else if (i2c_mode == COINES_I2C_SPEED_1_7_MHZ)
        i2c_config.speed = 1700000;
    else if (i2c_mode == COINES_I2C_SPEED_3_4_MHZ)
        i2c_config.speed = 3400000;

    rslt = twi_master_init(TWI0, &i2c_config);

    if (rslt == TWI_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_FAILURE;
}
/*!
 *  @brief 		This API is used to write the data in I2C communication.
 *
 */
int8_t coines_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
                        uint16_t count)
{
    int8_t rslt;
    twi_packet_t packet_tx;

    packet_tx.chip = dev_addr;
    packet_tx.addr[0] = reg_addr;
    packet_tx.addr_length = 1;
    packet_tx.buffer = reg_data;
    packet_tx.length = count;

    rslt = twi_master_write(TWI0, &packet_tx);

    if (rslt == TWI_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_COMM_IO_ERROR;
}

/*!
 *  @brief 	This API is used to read the data in I2C communication.
 *
 */
int8_t coines_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
                       uint16_t count)
{
    int8_t rslt;

    twi_packet_t packet_rx;

    packet_rx.chip = dev_addr;
    packet_rx.addr[0] = reg_addr;
    packet_rx.addr_length = 1;
    packet_rx.buffer = reg_data;
    packet_rx.length = count;

    rslt = twi_master_read(TWI0, &packet_rx);

    if (rslt == TWI_SUCCESS)
        return COINES_SUCCESS;
    else
        return COINES_E_COMM_IO_ERROR;
}

/*!
 *
 * @brief       : This API is used to write the data in SPI communication.
 *
 */
int8_t coines_write_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
                        uint16_t count)
{
    pdc_packet_t spi_packet;
    uint8_t tx_buff[count + 1];

    tx_buff[0] = reg_addr;
    memcpy(&tx_buff[1],reg_data,count);

    /*Setup SPI transfer*/
    Pdc *spi_pdc = spi_get_pdc_base(SPI);
    spi_packet.ul_addr = (uint32_t) tx_buff;
    spi_packet.ul_size = count + 1;
    pdc_rx_init(spi_pdc, &spi_packet, NULL);
    spi_packet.ul_addr = (uint32_t) tx_buff;
    spi_packet.ul_size = count + 1;
    pdc_tx_init(spi_pdc, &spi_packet, NULL);

    if (dev_addr == 0)
        dev_addr = COINES_SHUTTLE_PIN_7;

    /*Do the SPI transfer*/
    pio_configure_pin(get_hw_pin(dev_addr), PIO_OUTPUT_0);
    pdc_enable_transfer(spi_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
    while((spi_read_status(SPI) & SPI_SR_RXBUFF) == 0);
    pio_set_pin_high(get_hw_pin(dev_addr));
    pdc_disable_transfer(spi_pdc,PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);

    return COINES_SUCCESS;
}

/*!
 *
 * @brief       : This API is used to read the data in SPI communication.
 *
 */
int8_t coines_read_spi(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data,
                       uint16_t count)
{
    pdc_packet_t spi_packet;
    uint8_t tx_buff[count + 1];
    uint8_t rx_buff[count + 1];

    tx_buff[0] = reg_addr;

    /*Setup SPI transfer*/
    Pdc *spi_pdc = spi_get_pdc_base(SPI);
    spi_packet.ul_addr = (uint32_t) rx_buff;
    spi_packet.ul_size = count + 1;
    pdc_rx_init(spi_pdc, &spi_packet, NULL);
    spi_packet.ul_addr = (uint32_t) tx_buff;
    spi_packet.ul_size = count + 1;
    pdc_tx_init(spi_pdc, &spi_packet, NULL);

    if (dev_addr == 0)
        dev_addr = COINES_SHUTTLE_PIN_7;

    /*Do the SPI transfer*/
    pio_configure_pin(get_hw_pin(dev_addr), PIO_OUTPUT_0);
    pdc_enable_transfer(spi_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
    while((spi_read_status(SPI) & SPI_SR_RXBUFF) == 0);
    pio_set_pin_high(get_hw_pin(dev_addr));
    pdc_disable_transfer(spi_pdc,PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);
    memcpy(reg_data, &rx_buff[1],count);

    return COINES_SUCCESS;
}

/*!
 *
 *  @brief This API is used for introducing a delay in milliseconds
 */
void coines_delay_msec(uint32_t delay_ms)
{
    delay_ms(delay_ms);
}

/*!
 *  @brief This API is used for introducing a delay in microseconds
 */
void coines_delay_usec(uint32_t delay_us)
{
    delay_us(delay_us);
}
/*!
 *
 * @brief   This API is used to send the streaming settings to the board.
 *
 */
int16_t coines_config_streaming(uint8_t channel_id, struct coines_streaming_config *stream_config, struct coines_streaming_blocks *data_blocks)
{
    return COINES_E_NOT_SUPPORTED;
}
/*!
 *
 * @brief 	This API is used to start or stop the streaming.
 *
 */
int16_t coines_start_stop_streaming(enum coines_streaming_mode stream_mode,
		uint8_t start_stop)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 * @brief	 This API is used to read the streaming sensor data.
 *
 */
int16_t coines_read_stream_sensor_data(uint8_t sensor_id, uint32_t number_of_samples, uint8_t *data, uint32_t *valid_samples_count)
{
    return COINES_E_NOT_SUPPORTED;
}

/*!
 *
 * @brief 	This API is used to trigger the timer in firmware and enable or disable system time stamp
 *
 */
int16_t coines_trigger_timer(enum coines_timer_config tmr_cfg,
		enum coines_time_stamp_config ts_cfg)
{
    return COINES_E_NOT_SUPPORTED;
}

uint32_t coines_get_millis()
{
    return g_millis;
}
/*!
 * @brief This API attaches a interrupt to a Multi-IO pin
 *
 */
void coines_attach_interrupt(enum coines_multi_io_pin pin_number,
		void (*callback)(void), enum coines_pin_interrupt_mode int_mode)
{
    uint32_t mcu_int_mode = 0;

    pio_set_input(PIOC, get_hw_pin(pin_number) - PIO_PC0_IDX, PIO_INPUT);

    if (int_mode == COINES_PIN_INTERRUPT_CHANGE)
        mcu_int_mode = (PIO_IT_FALL_EDGE) & ~(PIO_IT_AIME);
    if (int_mode == COINES_PIN_INTERRUPT_RISING_EDGE)
        mcu_int_mode = PIO_IT_RISE_EDGE;
    if (int_mode == COINES_PIN_INTERRUPT_FALLING_EDGE)
        mcu_int_mode = PIO_IT_FALL_EDGE;

    pio_handler_set(PIOC, ID_PIOC, 1 << (get_hw_pin(pin_number) - PIO_PC0_IDX), mcu_int_mode, pioHandler);
    isr_cb[get_hw_pin(pin_number) - PIO_PC0_IDX] = callback;
    pio_enable_pin_interrupt(get_hw_pin(pin_number));

    NVIC_EnableIRQ(PIOC_IRQn);
}

/*!
 *
 * @brief	This API detaches a interrupt from a Multi-IO pin
 *
 */
void coines_detach_interrupt(enum coines_multi_io_pin pin_number)
{
    pio_disable_pin_interrupt(get_hw_pin(pin_number));
}

/*!
 *
 * @brief       : GPIO Interrupt Handler
 */
static void pioHandler(uint32_t id, uint32_t index)
{
    int i = 0;
    if (id == ID_PIOC)
        for (i = 0; i < 9; i++)
        {
            if (index & (1 << i))
                isr_cb[i]();
        }
}
/*!
 *
 * @brief       : SysTick timer Handler
 */
void SysTick_Handler(void)
{
    g_millis++;
}
