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
  auto& esp = *p_map.esp;

  HAL_CHECK(hal::write(terminal, "Starting RMD + WiFi Demo...\n"));
  HAL_CHECK(hal::delay(*p_map.steady_clock, 1s));

  auto router = HAL_CHECK(hal::can_router::create(can));
  auto drc = HAL_CHECK(hal::rmd::drc::create(router, 8.0f, 0x141));

  HAL_CHECK(hal::write(terminal, "Connecting to wifi...\n"));
  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp,
    "SJSU Robotics 2.4GHz",
    "R0Bot1cs3250",
    HAL_CHECK(hal::create_timeout(*p_map.steady_clock, 10s)));

  if (!wifi_result) {
    HAL_CHECK(hal::write(terminal, "Failed to create wifi client!\n"));
    return wifi_result.error();
  }

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(*p_map.steady_clock, 1s)),
    {
      .type = hal::socket::type::tcp,
      .domain = "192.168.1.183",
      .port = "5000",
    });

  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }

  auto socket = std::move(socket_result.value());
  HAL_CHECK(hal::write(
    terminal,
    "Connected to wifi! Starting main control loop in 1 second...\n"));
  drc.system_control(hal::rmd::drc::system::running);

  HAL_CHECK(hal::delay(*p_map.steady_clock, 1s));

  HAL_CHECK(hal::write(terminal, "Starting!\n"));

  while (true) {
    buffer.fill('.');
    get_request = "GET /drive" + get_rover_status() +
                  " HTTP/1.1\r\n"
                  "Host: 192.168.1.183:5000/\r\n"
                  "\r\n";

    auto write_result =
      socket.write(hal::as_bytes(get_request),
                   HAL_CHECK(hal::create_timeout(*p_map.steady_clock, 500ms)));
    if (!write_result) {
      continue;
    }
    HAL_CHECK(hal::delay(*p_map.steady_clock, 100ms));
    auto received = HAL_CHECK(socket.read(buffer)).data;
    auto result = to_string_view(received);
    auto start = result.find('{');
    auto end = result.find('}');
    auto json = result.substr(start, end - start + 1);

    HAL_CHECK(hal::write(terminal, "Received:\n"));
    HAL_CHECK(hal::write(terminal, json));
    HAL_CHECK(hal::write(terminal, "\r\n\n"));

    std::string json_string(json);
    commands =
      HAL_CHECK(mission_control.ParseMissionControlData(json_string, terminal));

    auto rmd_speed = commands.speed;
    drc.velocity_control(rmd_speed);
  }

  return hal::success();
}