#include "../hardware_map.hpp"
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/serial.hpp>

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

  // auto wlan_client_result = hal::esp8266::at::wlan_client::create(
  //   esp,
  //   "Glizzy 2.4G",
  //   "Iamv3ryZpeed",
  //   HAL_CHECK(hal::create_timeout(counter, 10s)));

  // if (!wlan_client_result) {
  //   HAL_CHECK(hal::write(terminal, "Failed to create wifi client!\n"));
  //   return wlan_client_result.error();
  // }

  // auto wlan_client = wlan_client_result.value();
  // auto tcp_socket_result =
  //   hal::esp8266::at::socket::create(wlan_client,
  //                             HAL_CHECK(hal::create_timeout(counter, 1s)),
  //                             {
  //                               .type = hal::socket::type::tcp,
  //                               .domain = "192.168.0.11",
  //                               .port = "5000",
  //                             });

  // if (!tcp_socket_result) {
  //   HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
  //   return tcp_socket_result.error();
  // }

  // // Move tcp_socket out of the result object
  // auto tcp_socket = std::move(tcp_socket_result.value());

  while (true) {
    HAL_CHECK(hal::write(terminal, "Hello, World!\n"));
    //   static constexpr std::string_view get_request =
    //     "GET /drive?hello=1&world=2 HTTP/1.1\r\n"
    //     "Host: 192.168.1.183:5000/\r\n"
    //     "\r\n";
    //   // Fill buffer with underscores to determine which blocks aren't
    //   filled. buffer.fill('.'); auto write_result =
    //     tcp_socket.write(hal::as_bytes(get_request),
    //                      HAL_CHECK(hal::create_timeout(counter, 50ms)));
    //   if (!write_result) {
    //     continue;
    //   }
    HAL_CHECK(hal::delay(counter, 1000ms));
    //   auto received = HAL_CHECK(tcp_socket.read(buffer)).data;

    //   auto result = to_string_view(received);
    //   auto start = result.find('{');
    //   auto end = result.find('}');
    //   auto json = result.substr(start, end - start + 1);
    //   HAL_CHECK(hal::write(terminal, json));
    //   HAL_CHECK(hal::write(terminal, "\r\n"));
  }

  return hal::success();
}