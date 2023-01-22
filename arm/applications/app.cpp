#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

hal::status application(drive::hardware_map& p_map)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;
  auto& can = *p_map.can;

  auto can_router = hal::can_router::create(can).value();

  auto rotunda_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 8.0, 0x141));
  auto shoulder_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 8 * 65 / 16, 0x142));
  auto elbow_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 8 * 5 / 2, 0x143));
  auto left_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 8.0, 0x144));
  auto right_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 8.0, 0x145));

  while (true) {
    HAL_CHECK(hal::write(*p_map.terminal, "Hello, Will!\n"));
    HAL_CHECK(hal::delay(*p_map.steady_clock, 1000ms));
  }

  return hal::success();
}