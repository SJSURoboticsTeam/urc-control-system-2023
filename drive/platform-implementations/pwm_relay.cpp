#include "pwm_relay.hpp"

namespace sjsu::drive {

  hal::result<pwm_relay> pwm_relay::create(const settings& p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock)
  {
    HAL_CHECK(p_pwm.frequency(p_settings.frequency));
    HAL_CHECK(p_pwm.duty_cycle(0.0f));
    return pwm_relay(p_settings, p_pwm, p_clock);
  }

  hal::result<relay::status_t> pwm_relay::driver_status() {
    return m_status;
  }

  hal::result<relay::set_status_t> pwm_relay::driver_toggle(bool p_toggle) {
    using namespace std::chrono_literals;

    if (p_toggle == true) {
      HAL_CHECK(m_pwm->duty_cycle(m_settings.initial_duty_cycle));
      hal::delay(*m_clock, 100ms);
      HAL_CHECK(m_pwm->duty_cycle(m_settings.tapered_duty_cycle));
      m_status.status = true;
    }
    else {
      HAL_CHECK(m_pwm->duty_cycle(0.0f));
      m_status.status = false;
    }

    return set_status_t{};
  }

  pwm_relay::pwm_relay(const settings& p_settings, hal::pwm& p_pwm, hal::steady_clock& p_clock) : m_settings(p_settings), m_pwm(&p_pwm), m_clock(&p_clock) {}
}