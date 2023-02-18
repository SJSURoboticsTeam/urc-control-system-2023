#include <libhal-esp8266/util.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/mission-control-handler.hpp"

#include "../hardware_map.hpp"

#include <string>
#include <string_view>

float convertAngleToDutyCycle(int angle)
{
  // operating travel 500us to 2500us
  // 0 degrees = 500us
  // 180 degrees = 2500
  // return (angle * 2.0f + 500.0f) / 20000.0f;
  // return angles map to 0.05 to 0.25
  return (angle * 0.001f + 0.05f);
  // result of angle  180 = 0.25
  // result of angle 0 = 0.05
}

hal::status application(drive::hardware_map& p_map)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& console = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& i2c = *p_map.i2c;

  HAL_CHECK(hal::write(console, "Starting program...\n"));

  hal::print(console, "[pca9685] Application Starting...\n\n");
  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  auto pwm0 = pca9685.get_pwm_channel<0>();

  // Setting the frequency of one channel will set the frequency of all channels
  HAL_CHECK(pwm0.frequency(50.0_Hz));

  while (true) {
    // for (int i = 0; i < 180; i += 10) {
    //   HAL_CHECK(hal::delay(clock, 50ms));
    //   HAL_CHECK(pwm0.duty_cycle(convertAngleToDutyCycle(i)));
    // }
    // for (float duty_cycle = 0.0f; duty_cycle < 1.05f; duty_cycle += 0.05f) {
    //   HAL_CHECK(hal::delay(clock, 50ms));
    //   HAL_CHECK(pwm0.duty_cycle(duty_cycle));
    // }
    for (int i = 0; i < 180; i += 1) {
      HAL_CHECK(hal::delay(clock, 50ms));
      // cast i to string
      std::string str = std::to_string(i);
      HAL_CHECK(hal::write(console, i));

      HAL_CHECK(pwm0.duty_cycle(convertAngleToDutyCycle(i)));
    }
  }

  return hal::success();
}
