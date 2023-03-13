#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../../hardware_map.hpp"
#include "../../science/implementations/co2_sensor.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(sjsu::hardware_map& p_map)
{
  // configure drivers
  auto& i2c2 = *p_map.i2c;
  auto& clock = *p_map.steady_clock;
  auto& terminal = *p_map.terminal;

  auto co2_sensor =
    HAL_CHECK(science::co2_sensor::create(i2c2, clock));

  while (true) {
    HAL_CHECK(hal::delay(clock, 500ms));
    auto co2_level = HAL_CHECK(co2_sensor.read_co2());
    hal::print<64>(terminal, "C02: %d\n", co2_level);
    HAL_CHECK(hal::delay(clock, 1000ms));
  }

  return hal::success();
}
