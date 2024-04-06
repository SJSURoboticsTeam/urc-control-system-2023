#pragma once
#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

namespace sjsu::drive {
class tmag5273 
{
public:
/**
 * @brief Constructor for the hall effect sensor
 * 
 * @param p_i2c The initialized I2C object to be passed for the sensor to use
 * @param p_clock The initialized clock object for I2C to use for its timing
 * @param p_device_version The version of the device {A1 : 1, B1 : 2, ..., D2 : 8}
 */
tmag5273(hal::i2c& p_i2c, hal::steady_clock& p_clock, uint8_t p_device_version);

void write(uint8_t p_register_address, uint8_t p_data);
void write(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes);

void read(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes);

private:
/**
 * @brief This function is to be used in the constructor to note the sensor's
 * address, as it can come with 1 of 4 default addresses
 * 
 * @param p_device_version The version of the device {A1 : 1, B1 : 2, ..., D2 : 8}
 */
uint8_t address_select(uint8_t p_device_version);

void configure_registers();

hal::i2c& m_i2c;
hal::steady_clock& m_clock;
uint8_t device_address;

enum register_addresses {
    DEVICE_CONFIG_1 = 0x0,
    DEVICE_CONFIG_2 = 0x1,
    SENSOR_CONFIG_1 = 0x2,
    SENSOR_CONFIG_2 = 0x3,
    X_THR_CONFIG = 0x4,
    Y_THR_CONFIG = 0x5,
    Z_THR_CONFIG = 0x6,
    T_CONFIG = 0x7,
    INT_CONFIG_1 = 0x8,
    MAG_GAIN_CONFIG = 0x9,
    MAG_OFFSET_CONFIG_1 = 0xA,
    MAG_OFFSET_CONFIG_2 = 0xB,
    I2C_ADDRESS = 0xC,
    DEVICE_ID = 0xD,
    MANUFACTURER_ID_LSB = 0xE,
    MANUFACTURER_ID_MSB = 0xF,
    T_MSB_RESULT = 0x10,
    T_LSB_RESULT = 0x11,
    X_MSB_RESULT = 0x12,
    X_LSB_RESULT = 0x13,
    Y_MSB_RESULT = 0x14,
    Y_LSB_RESULT = 0x15,
    Z_MSB_RESULT = 0x16,
    Z_LSB_RESULT = 0x17,
    CONV_STATUS = 0x18,
    ANGLE_RESULT_MSB = 0x19,
    ANGLE_RESULT_LSB = 0x1A,
    MAGNITUDE_RESULT = 0x1B,
    DEVICE_STATUS = 0x1C,
};

}
}