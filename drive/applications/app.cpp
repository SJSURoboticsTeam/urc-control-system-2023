#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <liblpc40xx/input_pin.hpp>
#include <liblpc40xx/can.hpp>
#include <librmd/drc.hpp>
#include <libesp8266/at/socket.hpp>
#include <libesp8266/at/wlan_client.hpp>
#include <libesp8266/util.hpp>

#include "../implementation/tri-wheel-router.hpp"
#include "../implementation/rules-engine.hpp"
#include "../implementation/mode-switcher.hpp"
#include "../implementation/mode-select.hpp"
#include "../implementation/mission-control-handler.hpp"
#include "../implementation/command-lerper.hpp"

#include "../hardware_map.hpp"

#include <string_view>

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  // auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock; // check
  auto& magnet0 = *p_map.in_pin0;
  auto& magnet1 = *p_map.in_pin1;
  auto& magnet2 = *p_map.in_pin2; // 
  auto& can = *p_map.can; // check

  auto can_router = hal::can_router::create(can).value();

  auto left_steer_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x141));
  auto left_hub_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x142));
  auto right_steer_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x143));
  auto right_hub_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x144));
  auto back_steer_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x145));
  auto back_hub_motor = HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x146));

  Drive::TriWheelRouter::leg right(right_steer_motor, right_hub_motor, magnet0);
  Drive::TriWheelRouter::leg left(left_steer_motor, left_hub_motor, magnet1);
  Drive::TriWheelRouter::leg back(back_steer_motor, back_hub_motor, magnet2);

  Drive::TriWheelRouter tri_wheel{right, left, back};
  Drive::MissionControlHandler mission_control;
  Drive::drive_commands commands;
  Drive::motor_feedback motor_speeds;
  Drive::tri_wheel_router_arguments arguments;

  Drive::RulesEngine rules_engine;
  Drive::ModeSwitch mode_switch;
  Drive::CommandLerper lerp;

  HAL_CHECK(hal::delay(counter, 1000ms));
  tri_wheel.HomeLegs(terminal);
  HAL_CHECK(hal::delay(counter, 1000ms));
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
        // if (response.find('{') != std::string::npos && response.find('}') != std::string::npos)
        // {
            // commands = HAL_CHECK(mission_control.ParseMissionControlData(response), terminal);
            commands = rules_engine.ValidateCommands(commands);
            commands = mode_switch.SwitchSteerMode(commands, arguments, motor_speeds);
            commands = lerp.Lerp(commands);
        // }
        commands.Print();
        arguments = Drive::ModeSelect::SelectMode(commands);
        arguments = tri_wheel.SetLegArguments(arguments);
        motor_speeds = HAL_CHECK(tri_wheel.GetMotorFeedback());
  }

  return hal::success();
}