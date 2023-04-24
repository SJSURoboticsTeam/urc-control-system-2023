#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/map.hpp>
#include <libhal-soft/rc_servo.hpp>
#include "../../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

float ConvertAngleToDutyCycle(int angle)
{
  std::pair<float, float> from;
  std::pair<float, float> to;
  from.first = 0.0f;
  from.second = 180.0f;
  to.first = 0.6f;
  to.second = 0.9f;
  return hal::map(static_cast<float>(angle), from, to);
}
hal::status application(sjsu::hardware_map& p_map)
{
  //frequency is 500Hz
  auto& seal_pwm = *p_map.pwm_1_5;
  auto& steady_clock = *p_map.steady_clock;

  HAL_CHECK(seal_pwm.frequency(50.0_Hz)); //setting pwm frequency
  HAL_CHECK(seal_pwm.duty_cycle(0.5f)); //setting pwm duty cycle not sure if this part is right

  auto servo = hal::rc_servo::create<50,900,2100,0,180>(seal_pwm).value(); //template parameters
  //servo.create(seal_pwm);
  // hal::write(terminal, "Inside RC_SERVO\n");

  while (true) {
    HAL_CHECK(servo.position(90.0_deg));
    HAL_CHECK(hal::delay(steady_clock, 1000ms));  
    HAL_CHECK(servo.position(0.0_deg));
    HAL_CHECK(hal::delay(steady_clock, 1000ms));  



  } 

  return hal::success();
}