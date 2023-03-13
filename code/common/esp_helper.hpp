#pragma once

#include <string_view>

#include <libhal-util/serial.hpp>
#include <libhal/serial.hpp>

class esp_helper : public hal::esp8266
{
public:
  struct esp_settings
  {
    std::string_view ssid = "";
    std::string_view password = "";
    std::string_view domain = "";
    std::string_view port = "";
    std::string_view endpoint = "";
    std::string_view parameters = "";
  };

  esp_helper(hal::serial& esp, hal::serial& debug, esp_settings& esp_settings)
    : esp(&esp)
    , debug(&debug)
    , esp_settings(&esp_settings)
  {
  }

  void connect() override
  {
    // wrap in catch statement?
    HAL_CHECK(hal::write(debug, "Connecting to WiFi...\n"));
    connect_to_wifi();
    establish_tcp_connection();
  }

  void get_commands() override
  {
    std::array<hal::byte, 8192> buffer{};
    auto command = get_http_request_message();
    HAL_CHECK(hal::write(debug, command));
  }

private:
  void connect_to_wifi() override
  {
    auto wlan_client_result = false;
    while (!wlan_client_result) {
      wlan_client_result = hal::esp8266::at::wlan_client::create(
        esp,
        esp_settings.ssid.data(),
        esp_settings.password.data(),
        HAL_CHECK(hal::create_timeout(counter, 10s)));
      if (!wlan_client_result) {
        HAL_CHECK(hal::write(debug, "Failed to create wifi client!\n"));
        // return wlan_client_result.error();
      }
    }
  }

  void establish_tcp_connection() override
  {
    auto tcp_socket_result = false;
    while (!tcp_socket_result) {
      tcp_socket_result = hal::esp8266::at::socket::create(
        wlan_client,
        HAL_CHECK(hal::create_timeout(counter, 1s)),
        {
          .type = hal::socket::type::tcp,
          .domain = esp_settings.domain.data(),
          .port = esp_settings.port.data(),
        });
      if (!tcp_socket_result) {
        HAL_CHECK(hal::write(debug, "TCP Socket couldn't be established\n"));
        // return tcp_socket_result.error();
      } else {
        auto tcp_socket = std::move(tcp_socket_result.value());
      }
    }
  }

  std::string_view get_http_request_message()
  {
    std::string_view request_http_message =
      "GET /" + esp_settings.endpoint.data() + "?" +
      esp_settings.parameters.data() +
      " HTTP/1.1\r\n"
      "Host: " +
      esp_settings.domain.data() + ":" + esp_settings.port.data() + "\r\n";
    return commands;
  }

  std::string_view get_http_response_message()
  {
    auto result =
      tcp_socket.write(command.data(),
                       command.size(),
                       HAL_CHECK(hal::create_timeout(counter, 50ms)));
    if (!result) {
      HAL_CHECK(hal::write(debug, "Failed to write to socket!\n"));
      continue;
    }
    std::string_view response_http_message = "";
  }

  esp_settings& esp_settings;
  hal::serial* esp;
  hal::serial* debug;
}