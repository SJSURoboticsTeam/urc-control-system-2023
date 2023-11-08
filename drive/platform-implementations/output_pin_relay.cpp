#include <output_pin_relay.hpp>

namespace sjsu::drive {

  hal::result<output_pin_relay> create(hal::output_pin& p_pin) 
  {
    return output_pin_relay(&p_pin);
  }

  hal::result<status_t> output_pin_relay::driver_status() {
    status_t current_status;
    current_status.status = HAL_CHECK(m_pin->level()).state;
    return status;
  }

  hal::result<set_status_t> output_pin_relay::driver_toggle(bool p_toggle) {
    set_status_t current_status;
    m_pin->level(p_toggle);
    return current_status;
  }

  output_pin_relay::output_pin_relay(hal::output_pin& p_pin) : m_pin(&p_pin) {}
}