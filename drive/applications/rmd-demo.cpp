#include <cinttypes>

#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/can.hpp>

#include "../dto/drive-dto.hpp"
#include "../hardware_map.hpp"
#include "../implementation/mission-control-handler.hpp"
#include "src/util.hpp"

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  Drive::drive_commands commands;
  Drive::MissionControlHandler mission_control;
  auto& can = *p_map.can;
  auto& terminal = *p_map.terminal;
  auto& clock = *p_map.steady_clock;

  HAL_CHECK(hal::delay(clock, 1s));

  auto router = HAL_CHECK(hal::can_router::create(can));
  auto left_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x142));

  HAL_CHECK(hal::delay(clock, 1s));
  HAL_CHECK(left_hub_motor.system_control(hal::rmd::drc::system::running));

  HAL_CHECK(hal::delay(clock, 1s));

  HAL_CHECK(hal::write(terminal, "Starting!\n"));

  HAL_CHECK(left_hub_motor.position_control(30.0_deg, 5.0_rpm));

  while (true) {
    HAL_CHECK(hal::delay(clock, 1s));
  }

  return hal::success();
}