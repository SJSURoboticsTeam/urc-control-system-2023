#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../../platform-implementations/pump_manager.hpp"
#include "../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

namespace sjsu::science {

hal::status application(application_framework& p_framework)
{
  // configure drivers
  auto& clock = *p_framework.steady_clock;
  
  auto pump_manager = HAL_CHECK(pump_manager::create(
    clock,
    *p_framework.deionized_water_pump,
    *p_framework.sample_pump,
    *p_framework.molisch_reagent_pump,
    *p_framework.sulfuric_acid_pump,
    *p_framework.biuret_reagent
    ));

    while(true){
        pump_manager.pump(pump_manager::pumps::DEIONIZED_WATER, 1000ms);
        pump_manager.pump(pump_manager::pumps::BIURET_REAGENT, 2000ms);
    }

  return hal::success();
}
}  // namespace sjsu::science