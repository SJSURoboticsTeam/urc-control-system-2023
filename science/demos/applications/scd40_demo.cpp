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

  while(true){
        // periodic readings are only updated every 5000ms (temperature)
        hal::delay(clock, 5000ms);
        scd40_sensor.read();
        auto co2_levels = HAL_CHECK(scd40_sensor.get_CO2_buffer());
        auto temp = HAL_CHECK(scd40_sensor.get_temp_buffer());
        auto RH_levels = HAL_CHECK(scd40_sensor.get_RH_buffer());


        hal::print<64>(terminal, "%-5.2f\t%-5.2f\t%-5.2f\n", co2_levels, temp, RH_levels);
        // hal::delay(clock, 500ms);
        // hal::print<64>(terminal, "CO2 Levels: %f\n", co2_levels);
        // hal::print<64>(terminal, "Temperature %f\n", temp);
        // hal::print<64>(terminal, "RH Levels: %f\n", RH_levels);

    }

  return hal::success();
}
}  // namespace sjsu::science