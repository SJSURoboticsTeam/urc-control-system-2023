#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>
#include <algorithm>


#include <libhal-esp8266/at.hpp>


#include "../../platform-implementations/helper.hpp"
#include "../applications/application.hpp"

namespace sjsu::arm {
hal::status initialize_processor()
{
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();

  return hal::success();
}


bool attempt_connect_query(hal::esp8266::at& esp8266, hal::serial& serial, hal::steady_clock& clock, hal::time_duration duration) {
  auto connect_result = esp8266.is_connected_to_ap(hal::create_timeout(clock, duration));
  if(!connect_result) {
    hal::print(serial, "\"esp8266::is_connected_to_ap\" timed out\n");
  }else {
    if(connect_result.value()) {
      hal::print(serial, "ESP is connected\n");
      return true;
    }
    hal::print(serial, "ESP is not connected\n");
  }
  return false;
}

hal::result<sjsu::arm::application_framework> initialize_platform()
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  // Set the MCU to the maximum clock speed
  HAL_CHECK(hal::lpc40::clock::maximum(12.0_MHz));

  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  static std::array<hal::byte, 64> receive_buffer{};
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0,
                                                       receive_buffer,
                                                       hal::serial::settings{
                                                         .baud_rate = 512000,
                                                       })));


  static std::array<hal::byte, 8192> recieve_buffer1{};

  static auto uart1 = HAL_CHECK((hal::lpc40::uart::get(1,
                                                       recieve_buffer1,
                                                       hal::serial::settings{
                                                         .baud_rate = 115200,
                                                       })));

  static constexpr std::string_view ssid =
    "TP-Link_FC30";  // change to wifi name that you are using
  static constexpr std::string_view password =
    "R0Bot1cs3250";  // change to wifi password you are using

  // still need to decide what we want the static IP to be
  static constexpr std::string_view ip = "192.168.0.212";
  static constexpr auto socket_config = hal::esp8266::at::socket_config{
    .type = hal::esp8266::at::socket_type::tcp,
    .domain = "192.168.0.211",
    .port = 5000,
  };
  HAL_CHECK(hal::write(uart0, "created Socket\n"));
  static constexpr std::string_view get_request = "GET /arm HTTP/1.1\r\n"
                                                  "Host: 192.168.0.211:5000\r\n"
                                                  "Keep-Alive: timeout=1000\r\n"
                                                  "Connection: keep-alive\r\n"
                                                  "\r\n";

  static std::array<hal::byte, 2048> buffer{};
  static auto helper = serial_mirror(uart1, uart0);

  static auto esp8266 = HAL_CHECK(hal::esp8266::at::create(uart1, hal::create_timeout(counter, 1s)));

  attempt_connect_query(esp8266, uart0, counter, 1s);
  
  {
    float start = counter.uptime().ticks / counter.frequency().operating_frequency;
    auto result = esp8266.connect_to_ap(ssid, password, hal::create_timeout(counter, 1s));
    float end = counter.uptime().ticks / counter.frequency().operating_frequency;
    float elapsed = end - start;
    if(!result) {
      hal::print(uart0, "\"esp8266::connect_to_ap()\" failed.\n");
    }else {
      hal::print<128>(uart0, "Successfully connected to router. Took %f seconds.\n", elapsed);
    }
  }
  while(1) {

    attempt_connect_query(esp8266, uart0, counter, 10ms);
    hal::delay(counter, 10ms);
  }



  return sjsu::arm::application_framework{

    .terminal = &uart0,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm