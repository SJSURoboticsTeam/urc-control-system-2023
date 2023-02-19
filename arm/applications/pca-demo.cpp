#include <libhal-esp8266/util.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/map.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/mission-control-handler.hpp"

#include "../hardware_map.hpp"
// #include "../implementation/routers/rr9-router.hpp"

#include <string>
#include <string_view>

float convert_to_duty_cycle(int angle)
{
  std::pair<float, float> from;
  std::pair<float, float> to;
  from.first = 0.0f;
  from.second = 180.0f;
  to.first = 0.025f;
  to.second = 0.125f;
  return hal::map(static_cast<float>(angle), from, to);
}

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& console = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& i2c = *p_map.i2c;

  HAL_CHECK(hal::write(console, "Starting program new...\n"));

  hal::print(console, "[pca9685] Application Starting...\n\n");
  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  auto pwm0 = pca9685.get_pwm_channel<0>();
  // Arm::RR9Router RR9(pca9685);

  HAL_CHECK(pwm0.frequency(50.0_Hz));
  hal::print(console, "pca9685 Application Starting...\n\n");

  HAL_CHECK(hal::write(console, "first step...\n"));
  HAL_CHECK(pwm0.duty_cycle(.0255f));

  HAL_CHECK(hal::delay(clock, 2s));
  HAL_CHECK(hal::write(console, "second step...\n"));
  HAL_CHECK(pwm0.duty_cycle(0.13f));
  HAL_CHECK(hal::delay(clock, 2s));

  while (true) {
    // for (int i = 0; i < 180; i++) {
    //   HAL_CHECK(pwm0.duty_cycle(convert_to_duty_cycle(i)));
    //   HAL_CHECK(hal::delay(clock, 100ms));
    // }

    // set to angle 0
    HAL_CHECK(pwm0.duty_cycle(convert_to_duty_cycle(0)));
    HAL_CHECK(hal::delay(clock, 2s));

    // set to angle 180
    HAL_CHECK(pwm0.duty_cycle(convert_to_duty_cycle(180)));
    HAL_CHECK(hal::delay(clock, 2s));
  }

  return hal::success();
}
