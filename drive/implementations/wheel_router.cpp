#include "../include/wheel_router.hpp"
#include "../include/settings.hpp"

namespace sjsu::drive {
  wheel_router::wheel_router(std::span<leg*> p_legs) : m_legs(p_legs) {}

  hal::status wheel_router::move(std::span<wheel_setting> p_settings) {
    if(m_legs.size() != p_settings.size()) {
        // Wheel Settings and leg mismatch.
        return hal::success(); // Is there a hal::failure()?
    }

    for(size_t i = 0; i < m_legs.size(); i ++) {
        HAL_CHECK(m_legs[i]->steer->position(p_settings[i].angle * angle_correction_factor));
        HAL_CHECK(m_legs[i]->propulsion->power(p_settings[i].wheel_speed));
    }
    
    return hal::success();
  }

//   hal::result<motor_feedback> wheel_router::get_motor_feedback() {
//     return motor_feedback{};
//   }

}  // namespace sjsu::drive
