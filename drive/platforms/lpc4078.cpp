#include <string_view>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-lpc40/clock.hpp>

#include <libhal-util/units.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-rmd/drc_servo.hpp>
#include <libhal-rmd/drc_motor.hpp>
#include <libhal-lpc40/input_pin.hpp>

#include "../platform-implementations/drc_speed_sensor.cpp"

#include "../platform-implementations/esp8266_mission_control.cpp"
#include "../platform-implementations/mission_control.hpp"
#include "../applications/application.hpp"
#include "../platform-implementations/home.hpp"
#include "../platform-implementations/offset_servo.hpp"

namespace sjsu::drive {

hal::status initialize_processor(){
  
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();
  return hal::success();
}

hal::result<application_framework> initialize_platform() 
{
  using namespace hal::literals;
  using namespace std::chrono_literals;
  
  // setting clock
  HAL_CHECK(hal::lpc40::clock::maximum(12.0_MHz));
  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  // Serial
  static std::array<hal::byte, 1024> recieve_buffer0{};
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0, recieve_buffer0, hal::serial::settings{
    .baud_rate = 38400,
  })));

  // servos, we need to init all of the mc_x motors then call make_servo 
  // in order to pass servos into the application
  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));
  
  static auto can_router = hal::can_router::create(can).value();

  static auto left_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x141));
  static auto left_leg_drc_servo = HAL_CHECK(hal::make_servo(left_leg_steer_drc, 10.0_rpm));
  auto left_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 22, hal::input_pin::settings{}));

  std::pair<hal::servo*, float> left_servo_offset(&left_leg_drc_servo, 0.0f);
  std::pair<hal::input_pin*, bool> left_mag_home(&left_leg_mag, false);

  static auto left_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x142));
  static auto left_leg_drc_motor = HAL_CHECK(hal::make_motor(left_leg_hub_drc, 100.0_rpm));
  static auto left_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(left_leg_hub_drc));
  

  static auto right_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x143));
  static auto right_leg_drc_servo = HAL_CHECK(hal::make_servo(right_leg_steer_drc, 10.0_rpm));
  auto right_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 15, hal::input_pin::settings{}));

  std::pair<hal::servo*, float> right_servo_offset(&right_leg_drc_servo, 0.0f);
  std::pair<hal::input_pin*, bool> right_mag_home(&right_leg_mag, false);

  static auto right_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x144));
  static auto right_leg_drc_motor = HAL_CHECK(hal::make_motor(right_leg_hub_drc, 100.0_rpm));
  static auto right_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(right_leg_hub_drc));


  static auto back_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x145));
  static auto back_leg_drc_servo = HAL_CHECK(hal::make_servo(back_leg_steer_drc, 10.0_rpm));
  auto back_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  std::pair<hal::servo*, float> back_servo_offset(&back_leg_drc_servo, 0.0f);
  std::pair<hal::input_pin*, bool> back_mag_home(&back_leg_mag, false);

  static auto back_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x146));
  static auto back_leg_drc_motor = HAL_CHECK(hal::make_motor(back_leg_hub_drc, 100.0_rpm));
  static auto back_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(back_leg_hub_drc));

  // for the 4th leg implementation
  // static auto extra_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x147));
  // static auto extra_leg_drc_servo = HAL_CHECK(hal::make_servo(extra_leg_steer_drc, 10.0_rpm));
  // auto extra_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  // std::pair<hal::servo*, float> extra_servo_offset(&extra_leg_drc_servo, 0.0f);
  // std::pair<hal::input_pin*, bool> extra_mag_home(&extra_leg_mag, false);

  // static auto extra_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x148));
  // static auto extra_leg_drc_motor = HAL_CHECK(hal::make_motor(extra_leg_hub_drc, 100.0_rpm));
  // static auto extra_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(extra_leg_hub_drc));

  // change this to 4 for 4 legs
  const int number_of_legs = 3;

  // uncomment extra_servo_offset here
  std::array<std::pair<hal::servo*, float>, number_of_legs> servo_offsets = {
        left_servo_offset, 
        right_servo_offset, 
        back_servo_offset,
        // extra_servo_offset
    };

  // uncomment extra_mag_home here
    std::array<std::pair<hal::input_pin*, bool>, number_of_legs> magnets_home = {
        left_mag_home, 
        right_mag_home, 
        back_mag_home,
        // extra_mag_home
    };
  

  HAL_CHECK(home(servo_offsets, magnets_home, counter));

  static auto left_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(left_leg_drc_servo, servo_offsets[0].second));
  static auto right_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(right_leg_drc_servo, servo_offsets[1].second));
  static auto back_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(back_leg_drc_servo, servo_offsets[2].second));
  // uncomment this for 4 legged imeplementation
  // static auto extra_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(extra_leg_drc_servo, servo_offsets[2].second));

  leg left_leg{.steer = &left_leg_drc_offset_servo, 
              .propulsion = &left_leg_drc_motor,
              .spd_sensor = &left_leg_drc_speed_sensor,
              };

  leg right_leg{.steer = &right_leg_drc_offset_servo, 
              .propulsion = &right_leg_drc_motor,
              .spd_sensor = &right_leg_drc_speed_sensor,
              };

  leg back_leg{.steer = &back_leg_drc_offset_servo, 
              .propulsion = &back_leg_drc_motor,
              .spd_sensor = &back_leg_drc_speed_sensor,
              };
  // uncomment this for 4 legged implementation
  // leg extra_leg{.steer = &extra_leg_drc_offset_servo, 
  //             .propulsion = &extra_leg_drc_motor,
  //             .spd_sensor = &extra_leg_drc_speed_sensor,
  //             };

  std::array<leg, number_of_legs> legs = {
    left_leg, 
    right_leg, 
    back_leg,
    // extra_leg
  };

  // mission control object

  static std::array<hal::byte, 8192> recieve_buffer1{};

  static auto uart1 =
    HAL_CHECK((hal::lpc40::uart::get(1, recieve_buffer1, hal::serial::settings{
      .baud_rate = 115200,
    })));

  constexpr std::string_view ssid = "ssid";
  constexpr std::string_view password = "password";
  constexpr std::string_view ip = "";
  constexpr auto socket_config = hal::esp8266::at::socket_config{
    .type = hal::esp8266::at::socket_type::tcp,
    .domain = "httpstat.us",
    .port = 80,
  };

  std::string_view get_request = "GET /drive HTTP/1.1\r\n"
                                "Host: httpstat.us:80\r\n"
                                "\r\n";


  std::array<hal::byte, 256> buffer{};
  
  auto timeout = hal::create_timeout(counter, 10s);
  auto esp8266 = HAL_CHECK(hal::esp8266::at::create(uart0, timeout));

  static auto esp_mission_control = HAL_CHECK(sjsu::drive::esp8266_mission_control::create(esp8266, 
                                  uart0, ssid, password, socket_config, 
                                  ip, timeout, buffer, get_request));

  return application_framework{.legs = legs,

                              .mc = &esp_mission_control,
                              
                              .terminal = &uart0,
                              .clock = &counter,
                              .reset = []() { hal::cortex_m::reset(); },
                              };
}

}
