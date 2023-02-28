#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/mission-control-handler.hpp"
#include "../implementation/routers/joint-router.hpp"
#include "../implementation/rules-engine.hpp"

#include "../hardware_map.hpp"
#include "src/util.hpp"

#include <string_view>

hal::status application(arm::hardware_map& p_map)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;
  auto& can = *p_map.can;
  auto& i2c = *p_map.i2c;

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

  HAL_CHECK(hal::write(terminal, "Starting program...\n"));
  HAL_CHECK(hal::write(terminal, "Beginning Can \n"));

  auto can_router = hal::can_router::create(can).value();

  HAL_CHECK(hal::write(terminal, "Can router created\n"));

  auto rotunda_motor = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x141));
  auto shoulder_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8 * 65 / 16, 0x142));
  auto elbow_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8 * 5 / 2, 0x143));
  auto left_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x144));
  auto right_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x145));

  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  auto pwm0 = pca9685.get_pwm_channel<0>();
  HAL_CHECK(pwm0.frequency(50.0_Hz));

  arm::JointRouter arm(rotunda_motor,
                       shoulder_motor,
                       elbow_motor,
                       left_wrist_motor,
                       right_wrist_motor,
                       pwm0);

  arm::mc_commands commands;
  arm::motors_feedback feedback;
  arm::RulesEngine rules_engine;
  arm::MissionControlHandler mission_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(counter, 1000ms));

  commands.Print(terminal);
  while (true) {
      // serial
    auto received = HAL_CHECK(terminal.read(buffer)).data;
    auto result = std::string_view(reinterpret_cast<const char*>(received.data()),
                              received.size());
  //   auto start = result.find('{');
  //   auto end = result.find('}');

  //   if (start != std::string::npos && end != std::string::npos) {
  //     result = result.substr(start, end - start + 1);
  //     commands =
  //       HAL_CHECK(mission_control.ParseMissionControlData(result, terminal));
  //   }

  //   commands = rules_engine.ValidateCommands(commands);
  //   arm.SetJointArguments(commands);
    HAL_CHECK(hal::write(terminal,result));
    HAL_CHECK(hal::delay(counter,1000ms));
  //   commands.Print(terminal);
  }

  return hal::success();
}