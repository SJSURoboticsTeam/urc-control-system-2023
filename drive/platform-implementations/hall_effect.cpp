#include "hall_effect.hpp"

using tmag5273_nm = sjsu::drive::tmag5273;

void write(uint8_t p_register_address, uint8_t p_data) {


}

void write(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes) {

}

void read(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes) {

}

tmag5273_nm::tmag5273(hal::i2c& p_i2c, hal::steady_clock& p_clock) : m_i2c(p_i2c), m_clock(p_clock) {}

void device_select(int p_device_version) {
    if (p_device_version == 1 || p_device_version == 5) {
        this->device_address = 0x35;
    }
    else if (p_device_version == 2 || p_device_version == 6) {
        this->device_address = 0x22;
    }
    else if (p_device_version == 3 || p_device_version == 7) {
        this->device_address = 0x78;
    }
    else if (p_device_version == 4 || p_device_version == 8) {
        this->device_address = 0x44;
    }
    else {
        // throw error for device version not valid
    }

    if (p_device_version >= 1 && p_device_version <= 4) {
        this->device_version = 1;
    }
    else if (p_device_version >= 5 && p_device_version <= 8) {
        this->device_version = 2;
    }
}

void configure_registers() {


}

void tmag5273_nm::start(hal::i2c& p_i2c, hal::steady_clock& p_clock, int p_device_version) {
    tmag5273 tmag5273(p_i2c, p_clock);



    return tmag5273.deviceAddress;
}