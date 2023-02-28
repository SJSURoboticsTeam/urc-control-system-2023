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

#include <string>
#include <string_view>

hal::status application(arm::hardware_map& p_map)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

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
  HAL_CHECK(hal::write(terminal, "Connected to wifi...\n"));

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(counter, 1s)),
    {
      .type = hal::socket::type::tcp,
      .domain = "192.168.1.197",
      .port = "5000",
    });

  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }
  HAL_CHECK(hal::write(terminal, "Connected to server...\n"));

  auto socket = std::move(socket_result.value());

  arm::mc_commands commands;
  arm::MissionControlHandler mission_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(counter, 1000ms));

  while (true) {
    buffer.fill('.');
    get_request =
      "GET /drive?HB=0&IO=1 HTTP/1.1\r\n Host: 192.168.1.197:5000/\r\n\r\n";

    auto write_result =
      socket.write(hal::as_bytes(get_request),
                   HAL_CHECK(hal::create_timeout(counter, 500ms)));
    if (!write_result) {
      continue;
    }

    HAL_CHECK(hal::delay(counter, 100ms));

    auto received = HAL_CHECK(socket.read(buffer)).data;

    auto result = std::string_view(
      reinterpret_cast<const char*>(received.data()), received.size());

    HAL_CHECK(hal::write(terminal, result));
    HAL_CHECK(hal::write(terminal, "\r\n"));
  }

  return hal::success();
}
