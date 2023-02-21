#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/mission-control-handler.hpp"
#include "../implementation/routers/joint-router.hpp"
#include "../implementation/rules-engine.hpp"

#include "../hardware_map.hpp"

#include <string>
#include <string_view>

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;

  std::array<hal::byte, 8192> buffer{};

  Arm::mc_commands commands;
  Arm::RulesEngine rules_engine;
  Arm::MissionControlHandler mission_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(counter, 1s));

  while (true) {
    // TODO: Add the parser for the mission control data and test it here
  }

  return hal::success();
}