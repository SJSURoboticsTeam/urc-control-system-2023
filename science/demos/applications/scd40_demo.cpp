#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../../platform-implementations/scd40.hpp"
#include "../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;
namespace sjsu::science {

hal::status application(application_framework& p_framework)
{
  // configure drivers
  auto& i2c2 = *p_framework.i2c;
  auto& clock = *p_framework.steady_clock;
  auto& terminal = *p_framework.terminal;

  auto scd40_sensor = HAL_CHECK(scd40::create(i2c2, clock));

  // while (true) {
  //   hal::delay(clock, 500ms);
  //   auto co2_level = HAL_CHECK(scd40.read_co2());
  //   hal::print<64>(terminal, "C02: %d\n", co2_level);
  //   hal::delay(clock, 1ms);
  // }
  while(true){
        hal::delay(clock, 500ms);
        auto co2_levels = HAL_CHECK(scd40_sensor.get_CO2());
        auto temp = HAL_CHECK(scd40_sensor.get_temp());
        auto RH_levels = HAL_CHECK(scd40_sensor.get_RH());

        hal::print<64>(terminal, "CO2 Levels: %d\n", co2_levels);
        hal::print<64>(terminal, "Temperature %d\n", temp);
        hal::print<64>(terminal, "RH Levels: %d\n", RH_levels);

        hal::delay(clock, 1ms);

    }

  return hal::success();
}
}  // namespace sjsu::science