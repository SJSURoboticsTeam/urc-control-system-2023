#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-rmd/drc.hpp>
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

  Arm::mc_commands commands;
  Arm::MissionControlHandler mission_control;

  HAL_CHECK(hal::delay(counter, 1s));

  while (true) {
    HAL_CHECK(hal::delay(counter, 1s));
    auto json = commands.json_string();
    HAL_CHECK(hal::write(terminal, json));
  }

  return hal::success();
}