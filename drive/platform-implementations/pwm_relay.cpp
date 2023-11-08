#include <pwm_relay.hpp>

namespace sjsu::drive {

  hal::result<pwm_relay> create(settings p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock)
  {
    HAL_CHECK(m_pwm.frequency(m_settings.frequency));
    HAL_CHECK(m_pwm.duty_cycle(0.0f));
    return pwm_relay(p_settings, &p_pwm, &p_clock);
  }

  hal::result<status_t> driver_status() {
    return m_status;
  }

  hal::result<set_status_t> driver_toggle(bool p_toggle) {
    using namespace std::chrono_literals;

    if (p_toggle == true) {
      HAL_CHECK(m_pwm.duty_cycle(m_settings.initial_duty_cycle));
      HAL_CHECK(hal::delay(m_clock, 100ms));
      HAL_CHECK(m_pwm.duty_cycle(m_settings.tapered_duty_cycle));
      m_status = true;
    }
    else {
      HAL_CHECK(m_pwm.duty_cycle(0.0f));
      m_status = false;
    }
  }

  pwm_relay::pwm_relay(settings p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock) : m_settings(p_settings), m_pwm(&p_pwm), m_clock(&p_clock) {}
}