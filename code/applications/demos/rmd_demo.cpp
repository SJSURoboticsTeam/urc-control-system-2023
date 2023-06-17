#include <cinttypes>

#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/can.hpp>

#include "drive/dto/drive_dto.hpp"
#include "hardware_map.hpp"
#include "drive/implementation/mission_control_handler.hpp"
#include "common/util.hpp"

hal::status application(sjsu::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  Drive::drive_commands commands;
  auto& can = *p_map.can;
  auto& terminal = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& esp = *p_map.esp;

  HAL_CHECK(hal::delay(clock, 1s));

  auto router = HAL_CHECK(hal::can_router::create(can));

  auto left_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x141));
  auto right_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x143));
  auto back_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x145));
  auto left_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x142));
  auto right_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x144));
  auto back_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x146));

  HAL_CHECK(hal::delay(*p_map.steady_clock, 1s));

  HAL_CHECK(hal::write(terminal, "Starting!\n"));

  left_steer_motor.velocity_control(5.0_rpm);
  right_steer_motor.velocity_control(5.0_rpm);
  back_steer_motor.velocity_control(5.0_rpm);
  left_hub_motor.velocity_control(5.0_rpm);
  right_hub_motor.velocity_control(5.0_rpm);
  back_hub_motor.velocity_control(5.0_rpm);

  while (true) {
    HAL_CHECK(hal::delay(clock, 1s));
  }

  return hal::success();
}