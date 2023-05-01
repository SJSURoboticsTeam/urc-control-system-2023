#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-rmd/mc_x.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../../arm/implementation/joint_router.hpp"
#include "../../arm/implementation/mission_control_handler.hpp"
#include "../../arm/implementation/rules_engine.hpp"

#include "../../hardware_map.hpp"
#include "common/util.hpp"

#include <string_view>

hal::status application(sjsu::hardware_map& p_map)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& can = *p_map.can;
  auto& i2c = *p_map.i2c;
  auto can_router = hal::can_router::create(can).value();
  float deg = 10;
    auto rotunda_motor =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, clock, 36.0, 0x141));
  auto shoulder_motor =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, clock, 36.0 * 65/30, 0x142));
  auto elbow_motor =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, clock, 36.0 * 40/30, 0x143));
  auto left_wrist_motor =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, clock, 36.0, 0x144));
  auto right_wrist_motor =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, clock, 36.0, 0x145));

  std::string_view json;

  while (true) {
    HAL_CHECK(elbow_motor.position_control(hal::degrees(deg),hal::rpm(5.0f)));
    HAL_CHECK(hal::delay(clock, 5s));
    HAL_CHECK(elbow_motor.position_control(hal::degrees(-deg),hal::rpm(5.0f)));
    HAL_CHECK(hal::delay(clock, 5s));
  }

  return hal::success();
}