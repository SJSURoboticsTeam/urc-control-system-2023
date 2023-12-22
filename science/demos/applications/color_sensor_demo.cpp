#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../../platform-implementations/color_sensor_opt4048.hpp"
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

  hal::print(terminal, "hi\n");
  

  return hal::success();
}
}  // namespace sjsu::science