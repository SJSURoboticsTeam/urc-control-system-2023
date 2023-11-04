#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../../hardware_map.hpp"
#include "../../platform-implementations/pressure_sensor_bme680.cpp"

using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(sjsu::hardware_map& p_map)
{
  // configure drivers
  auto& i2c2 = *p_map.i2c;
  auto& clock = *p_map.steady_clock;
  auto& terminal = *p_map.terminal;

  auto pressure_sensor = HAL_CHECK(science::pressure_sensor_bme680::create(i2c2, clock, terminal));
  hal::print<64>(terminal, "\nApplication related stuff initialized\n");

  HAL_CHECK(pressure_sensor.start());

  while (true) {

    // HAL_CHECK(pressure_sensor.set_focus_mode()); 


    HAL_CHECK(hal::delay(clock, 100ms));

    HAL_CHECK(pressure_sensor.get_parsed_data());
    int16_t temp = HAL_CHECK(pressure_sensor.get_temperature());

    int32_t pres = HAL_CHECK(pressure_sensor.get_pressure());

    hal::print<64>(terminal, "Temperature %d\n", temp);
    hal::print<64>(terminal, "Pressure %d\n", pres);

    HAL_CHECK(hal::delay(clock, 10ms));
    //senor automatically returns to sleep mode so we need to change that
  }

  return hal::success();
}