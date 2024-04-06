#include "hall_effect.hpp"

using tmag5273_nm = sjsu::drive::tmag5273;

tmag5273_nm::tmag5273(hal::i2c& p_i2c, hal::steady_clock& p_clock, uint8_t p_device_version) {
    m_i2c = p_i2c;
    m_clock = p_clock;
    device_address = address_select(p_device_version);
    device_version = version_select(p_device_version); 
}

void tmag5273_nm::write(uint8_t p_register_address, uint8_t p_data) {


}

void tmag5273_nm::write(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes) {

}

void tmag5273_nm::read(uint8_t p_register_address, uint8_t *p_data_buffer, uint8_t number_of_bytes) {

}

uint8_t tmag5273_nm::address_select(uint8_t p_device_version) {
    if (p_device_version == 1 || p_device_version == 5) {
        return 0x35;
    }
    else if (p_device_version == 2 || p_device_version == 6) {
        return 0x22;
    }
    else if (p_device_version == 3 || p_device_version == 7) {
        return 0x78;
    }
    else if (p_device_version == 4 || p_device_version == 8) {
        return 0x44;
    }
    else {
        // throw error for device version not valid
    }
}

void tmag5273_nm::configure_registers() {
    


}