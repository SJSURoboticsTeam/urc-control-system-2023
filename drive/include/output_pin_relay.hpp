#pragma once

#include "relay.hpp"
#include <libhal/output_pin.hpp>

namespace sjsu::drive {

  class output_pin_relay : public relay {
    public:

    static hal::result<output_pin_relay> create(hal::output_pin& p_pin);

    private:
    output_pin_relay(hal::output_pin& p_pin);

    hal::result<status_t> driver_status() override;

    hal::result<set_status_t> driver_toggle(bool p_toggle) override;

    hal::output_pin* m_pin;
  };
  
}