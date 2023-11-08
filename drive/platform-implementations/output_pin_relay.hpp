#pragma once

#include <libhal/output_pin.hpp>

namespace sjsu::drive {

  class output_pin_relay : public relay {

    static hal::result<output_pin_relay> create(hal::output_pin& p_pin);

    private:
    output_pin_relay(hal::output_pin& p_pin);

    hal::result<status_t> driver_status();

    hal::result<set_status_t> driver_toggle(bool p_toggle);

    hal::output_pin* m_pin;
  }
  
}