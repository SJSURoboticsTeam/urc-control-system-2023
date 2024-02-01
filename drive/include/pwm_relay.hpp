#pragma once

#include "relay.hpp"
#include <libhal/pwm.hpp>
#include <libhal/units.hpp>
#include <libhal-util/steady_clock.hpp>

namespace sjsu::drive {

  class pwm_relay : public relay {
    public:
    struct settings {
      hal::hertz frequency = 0.0f;
      float initial_duty_cycle = 0.0f;
      float tapered_duty_cycle = 0.0f;
    };

    static hal::result<pwm_relay> create(const settings& p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock);

    private:
    pwm_relay(const settings& p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock);

    hal::result<status_t> driver_status() override;

    hal::result<set_status_t> driver_toggle(bool p_toggle) override;

    const settings& m_settings;
    hal::pwm* m_pwm;
    hal::steady_clock* m_clock;
    status_t m_status;
  };
  
}