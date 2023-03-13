#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"
#include "../implementations/mq4_methane_sensor.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map)
{
  // science robot entry point here. This function may yield an error.
  // configure drivers

  while (true) {
    // demo for methane sensor driver.
    science::Mq4MethaneSensor methane_driver =
      science::Mq4MethaneSensor(p_map.adc_4, p_map.is_methane);

    bool is_methane = HAL_CHECK(methane_driver.detect_signal());
    float methane_value = HAL_CHECK(methane_driver.get_parsed_data());

    if (is_methane) {
      hal::print<64>(*p_map.terminal, "CH4: %f\n", methane_value);
    } else {
      hal::print<64>(*p_map.terminal, "No methane detected.\n");
    }
    HAL_CHECK(hal::delay(*p_map.steady_clock, 100ms));
  }

  return hal::success();
}
