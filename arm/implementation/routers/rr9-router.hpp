#pragma once
#include "../../dto/arm-dto.hpp"
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/map.hpp>
#include <libhal-util/units.hpp>

namespace Arm {
class RR9Router
{
public:
  RR9Router(hal::pca::pca9685 pca)
    : pca_(pca)
  {
    pwm0_ = pca.get_pwm_channel<0>();
    HAL_CHECK(pwm0_.frequency(50_Hz));
  }

  mc_commands SetRR9Arguments(mc_commands args)
  {
    // do logic for RR9 moving here
    std::pair<float, float> from;
    std::pair<float, float> to;
    from.first = 0;
    from.second = 180;
    to.first = 0.025f;
    to.second = 0.125f;
    duty_cycle = hal::map(args.rr9_angle, from, to);
    HAL_CHECK(pwm0_.duty_cycle(duty_cycle));
    return args;
  }

private:
  hal::pca::pca9685 pca_;
  hal::pca::pca9685::pwm_channel pwm0_;
};
}  // namespace Arm