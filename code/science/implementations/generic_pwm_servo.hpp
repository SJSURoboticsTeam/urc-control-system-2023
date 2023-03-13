#pragma once

#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-util/map.hpp>

namespace science {

class generic_pwm_servo
{
public:
  generic_pwm_servo(hal::lpc40xx::pwm pwm,
                    float min_duty_cycle,
                    float max_duty_cycle,
                    hal::hertz frequency)
  {
    servo_pwm = pwm;
    servo_min_cycle = min_duty_cycle;
    servo_max_cycle = max_duty_cycle;
    servo_frequency = servo_frequency;
  }

}

protected : hal::lpc40xx::pwm* servo_pwm;
float servo_max_cycle;
float servo_min_cycle;
hal::hertz servo_frequency;

}  // namespace science