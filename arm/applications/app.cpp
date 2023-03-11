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
  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp,
    "SJSU Robotics 2.4GHz",
    "R0Bot1cs3250",
    hal::create_timeout(counter, 10s).value());

  while (true)
  {
    wifi_result = hal::esp8266::at::wlan_client::create(
      esp,
      "SJSU Robotics 2.4GHz",
      "R0Bot1cs3250",
      hal::create_timeout(counter, 10s).value());

    if (wifi_result) {
      break;
    }
    HAL_CHECK(hal::write(terminal, "failed to connect to wifi"));
  }
  HAL_CHECK(hal::write(terminal, "ESP created!\n"));
      

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(counter, 1s)),
    {
      .type = hal::socket::type::tcp,
      .domain = "192.168.1.110",
      .port = "5000",
    });

  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }

  auto socket = std::move(socket_result.value());

  HAL_CHECK(hal::write(terminal, "Beginning Can \n"));

  auto can_router = hal::can_router::create(can).value();

  auto rotunda_motor = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x141));
  auto shoulder_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8 * 65 / 16, 0x142));
    HAL_CHECK(hal::write(terminal, "Here 1 \n"));
  auto elbow_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8 * 5 / 2, 0x143));
    HAL_CHECK(hal::write(terminal, "Here 2 \n"));
  auto left_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x144));
    HAL_CHECK(hal::write(terminal, "Here 3 \n"));
  auto right_wrist_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x145));
    HAL_CHECK(hal::write(terminal, "Here 4 \n"));

  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  HAL_CHECK(hal::write(terminal, "Here 5 \n"));
  auto pwm0 = pca9685.get_pwm_channel<0>();
  HAL_CHECK(pwm0.frequency(50.0_Hz));
  std::string_view json;

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

  HAL_CHECK(hal::delay(counter, 1000ms));

  while (true) {
    buffer.fill('.');
    get_request =
      "GET /arm?HB=0&IO=1 HTTP/1.1\r\n Host: 192.168.1.110:5000/\r\n\r\n";

    hal::print(terminal, "here");
    auto write_result =
      socket.write(hal::as_bytes(get_request),
                   hal::create_timeout(counter, 1000ms).value());

    hal::print(terminal, "here1");
    if (!write_result) {
      HAL_CHECK(hal::write(terminal, "Failed:  \n"));
      continue;
    }
    HAL_CHECK(hal::delay(counter, 100ms));

    hal::print(terminal, "here2");

    auto received = HAL_CHECK(socket.read(buffer)).data;
    auto result = to_string_view(received);
    auto start = result.find('{');

    hal::print<128>(terminal, "%d", start);
    auto end = result.find('}');
    hal::print<128>(terminal, "%d", end);
    if(start != -1 && end != -1) {
      json = result.substr(start, end - start + 1); 
      std::string json_string(json);
      HAL_CHECK(hal::write(terminal, json)); 

      HAL_CHECK(hal::write(terminal, json_string));
      HAL_CHECK(hal::write(terminal, "\r\n\n"));

      commands =
        HAL_CHECK(mission_control.ParseMissionControlData(json_string, terminal));
    }
    

    commands = rules_engine.ValidateCommands(commands);


    commands.Print(terminal);


    arm.SetJointArguments(commands);

    HAL_CHECK(hal::write(terminal, "Set joint arguments \n"));
  }

  return hal::success();
}
//http://192.168.137.123:5000/arm