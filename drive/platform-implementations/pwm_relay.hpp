#pragma once

#include <libhal/pwm.hpp>
#include <libhal/units.hpp>

namespace sjsu::drive {

  class pwm_relay : public relay {

    struct settings {
      hal::hertz frequency = 0.0f;
      float initial_duty_cycle = 0.0f;
      float tapered_duty_cycle = 0.0f;
    };

    static hal::result<pwm_relay> create(settings p_settings, hal::pwm& p_pwm);

    private:
    pwm_relay(settings p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock);

    hal::result<status_t> driver_status();

    hal::result<set_status_t> driver_toggle(bool p_toggle);

    hal::pwm* m_pwm;
    hal::steady_clock* m_clock
    settings m_settings;
    status_t m_status;
  }
  
}
