#include "../hardware_map.hpp"
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/serial.hpp>

#include <libesp8266/at/socket.hpp>
#include <libesp8266/at/wlan_client.hpp>
#include <libesp8266/util.hpp>

#include <string_view>

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;

  // 8kB buffer to read data into
  std::array<hal::byte, 8192> buffer{};

  HAL_CHECK(hal::write(terminal, "Starting program...\n"));

  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp,
    "SJSU Robotics 2.4GHz",
    "R0Bot1cs3250",
    HAL_CHECK(hal::create_timeout(counter, 10s)));

  if (!wifi_result) {
    HAL_CHECK(hal::write(terminal, "Failed to create wifi client!\n"));
    return wifi_result.error();
  }

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(counter, 1s)),
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

  while (true) {
    static constexpr std::string_view get_request =
      "GET /drive?hello=1&world=2 HTTP/1.1\r\n"
      "Host: 192.168.1.183:5000/\r\n"
      "\r\n";

    buffer.fill('.');

    auto write_result =
      socket.write(hal::as_bytes(get_request),
                   HAL_CHECK(hal::create_timeout(counter, 100ms)));
    if (!write_result) {
      continue;
    }
    HAL_CHECK(hal::delay(counter, 100ms));

    auto received = HAL_CHECK(socket.read(buffer)).data;

    auto result = std::string_view(reinterpret_cast<char*>(received.data()),
                                   received.size());
    auto start = result.find('{');
    auto end = result.find('}');
    auto json = result.substr(start, end - start + 1);

    HAL_CHECK(hal::write(terminal, json));
    HAL_CHECK(hal::write(terminal, "\r\n\n"));
  }

  return hal::success();
}