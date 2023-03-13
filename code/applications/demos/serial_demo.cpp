#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../drive/implementation/command_lerper.hpp"
#include "../drive/implementation/mission_control_handler.hpp"
#include "../drive/implementation/mode_select.hpp"
#include "../drive/implementation/mode_switcher.hpp"
#include "../drive/implementation/rules_engine.hpp"
#include "../drive/implementation/tri_wheel_router.hpp"

#include "../hardware_map.hpp"

#include <cmath>
#include <string>
#include <string_view>

#include "common/util.hpp"

hal::status application(sjsu::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;

  std::array<hal::byte, 8192> buffer{};

  Drive::MissionControlHandler mission_control;
  Drive::drive_commands commands;
  Drive::tri_wheel_router_arguments arguments;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(counter, 1s));

  while (true) {
    auto received = HAL_CHECK(terminal.read(buffer)).data;
    auto result = std::string(reinterpret_cast<const char*>(received.data()),
                              received.size());

    auto start = result.find('{');
    auto end = result.find('}');

    if (start != std::string::npos && end != std::string::npos) {
      result = result.substr(start, end - start + 1);
      commands =
        HAL_CHECK(mission_control.ParseMissionControlData(result, terminal));
      commands.Print(terminal);
    }
    HAL_CHECK(hal::delay(counter, 1s));
  }

  return hal::success();
}