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
#include <libhal-lpc40/input_pin.hpp>

#include "../platform-implementations/drc_speed_sensor.cpp"

#include "../platform-implementations/esp8266_mission_control.cpp"
#include "../platform-implementations/mission_control.hpp"
#include "../applications/application.hpp"
#include "../platform-implementations/home.hpp"
#include "../platform-implementations/offset_servo.hpp"
#include "../platform-implementations/helper.hpp"
#include "../platform-implementations/calibration_settings.hpp"

#include "../platform-implementations/tcp_client.hpp"

#define USE_MOTORS

#include <cmath>

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
    // .baud_rate = 115200,
    .baud_rate = 512000,
  })));
  // servos, we need to init all of the mc_x motors then call make_servo 
  // in order to pass servos into the application
  hal::print(uart0, "Initializing CAN\n");
  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));

  static auto can_router = hal::can_router::create(can).value();
  // left leg

  #ifdef USE_MOTORS

  constexpr hal::rpm max_steering_rpm = 20.0_rpm;
  // constexpr hal::rpm max_speed = 200.0_rpm;
  constexpr hal::rpm max_speed = 100.0_rpm;

  constexpr float angle_correction_factor = 1.3625; // ???? WHY ARE THESE MOTORS SCALED????

  hal::print(uart0, "Initializing Left Leg\n");
  static auto left_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x141));
  static auto left_leg_drc_servo = HAL_CHECK(hal::make_servo(left_leg_steer_drc, max_steering_rpm));
  static auto left_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(left_leg_steer_drc));
  auto left_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 22, hal::input_pin::settings{}));

  static auto left_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x142));
  static auto left_leg_drc_motor = HAL_CHECK(hal::make_motor(left_leg_hub_drc, max_speed));
  static auto left_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(left_leg_hub_drc));
  
  static auto left_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(left_leg_drc_servo, 0.0f));

  static auto left_home = homing{&left_leg_drc_offset_servo, &left_leg_mag, false};
  // static auto left_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto left_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto left_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));

  hal::print(uart0, "Initializing Right Leg\n");
  // right leg
  static auto right_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x143));
  static auto right_leg_drc_servo = HAL_CHECK(hal::make_servo(right_leg_steer_drc, max_steering_rpm));
  static auto right_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(right_leg_steer_drc));
  auto right_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 15, hal::input_pin::settings{}));

  static auto right_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x144));
  static auto right_leg_drc_motor = HAL_CHECK(hal::make_motor(right_leg_hub_drc, max_speed));
  static auto right_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(right_leg_hub_drc));

  static auto right_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(right_leg_drc_servo, 0.0f));
  static auto right_home = homing{&right_leg_drc_offset_servo, &right_leg_mag, false};
  // static auto right_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto right_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto right_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));

  hal::print(uart0, "Initializing Back Leg\n");
  // back leg
  static auto back_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x145));
  static auto back_leg_drc_servo = HAL_CHECK(hal::make_servo(back_leg_steer_drc, max_steering_rpm));
  static auto back_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(back_leg_steer_drc));
  auto back_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  static auto back_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x146));
  static auto back_leg_drc_motor = HAL_CHECK(hal::make_motor(back_leg_hub_drc, max_speed));
  static auto back_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(back_leg_hub_drc));

  static auto back_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(back_leg_drc_servo, 0.0f));
  static auto back_home = homing{&back_leg_drc_offset_servo, &back_leg_mag, false};

  // static auto back_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto back_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto back_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));


  // extra leg
  // static auto extra_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x147));
  // static auto extra_leg_drc_servo = HAL_CHECK(hal::make_servo(extra_leg_steer_drc, 5.0_rpm));
  // auto extra_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  // static auto extra_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x148));
  // static auto extra_leg_drc_motor = HAL_CHECK(hal::make_motor(extra_leg_hub_drc, 100.0_rpm));
  // static auto extra_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(extra_leg_hub_drc));
  
  // static auto extra_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(extra_leg_drc_servo, 0.0f));
  // static auto extra_home = homing{&extra_leg_drc_offset_servo, &extra_leg_mag, false};
  const size_t number_of_legs = 3;

  static std::array<homing*, number_of_legs> homing_structs = {
        &left_home,
        &right_home,
        &back_home,
        // &extra_home,
    };
  hal::print(uart0, "Begin homing\n");
  HAL_CHECK(home(homing_structs, counter, &uart0));
  hal::print(uart0, "Finished homing\n");
  left_leg_drc_offset_servo.set_offset(left_leg_drc_offset_servo.get_offset() - 30 * angle_correction_factor);
  right_leg_drc_offset_servo.set_offset(right_leg_drc_offset_servo.get_offset() - 150 * angle_correction_factor);
  back_leg_drc_offset_servo.set_offset(back_leg_drc_offset_servo.get_offset() - 90 * angle_correction_factor);

  HAL_CHECK(left_leg_drc_offset_servo.position(0));
  HAL_CHECK(right_leg_drc_offset_servo.position(0));
  HAL_CHECK(back_leg_drc_offset_servo.position(0));
  // hal::delay(counter, 5s);
  // hal::print<100>(uart0, "right offset: %f", right_home.servo->get_offset());
  // hal::print<100>(uart0, "left offset: %f", left_home.servo->get_offset());
  // hal::print<100>(uart0, "back offset: %f", back_home.servo->get_offset());
  static leg left_leg{.steer = &left_leg_drc_offset_servo, 
              .propulsion = &left_leg_drc_motor,
              .steer_speed_sensor = &left_leg_drc_steer_speed_sensor,
              .propulsion_speed_sensor = &left_leg_drc_hub_speed_sensor,
              };

  static leg right_leg{.steer = &right_leg_drc_offset_servo, 
              .propulsion = &right_leg_drc_motor,
              .steer_speed_sensor = &right_leg_drc_steer_speed_sensor,
              .propulsion_speed_sensor = &right_leg_drc_hub_speed_sensor,
              };

  static leg back_leg{.steer = &back_leg_drc_offset_servo, 
              .propulsion = &back_leg_drc_motor,
              .steer_speed_sensor = &back_leg_drc_steer_speed_sensor,
              .propulsion_speed_sensor = &back_leg_drc_hub_speed_sensor
              };
  // static leg extra_leg{.steer = &extra_leg_drc_offset_servo, 
  //             .propulsion = &extra_leg_drc_motor,
  //             .spd_sensor = &extra_leg_drc_speed_sensor,
  //             .propulsion_speed_sensor = &
  //             };
// 

  static std::array<leg*, number_of_legs> legs = {
    &left_leg, 
    &right_leg, 
    &back_leg,
    // extra_leg,
  };
    #endif

    static std::array<vector2, 3> wheel_locations = {
      vector2::from_bearing(1, -60 * std::numbers::pi / 180),
      vector2::from_bearing(1, 60 * std::numbers::pi / 180),
      vector2::from_bearing(1, std::numbers::pi),
    };
    // static std::array<vector2, 3> wheel_locations = {
    //   vector2(-0.5, 0),
    //   vector2(0.5, 0),
    //   vector2(0, -std::sqrt(3/2)),
    // };
    // static std::array<vector2, 3> wheel_locations = {
    //   vector2(-0.5, std::sqrt(3/2)),
    //   vector2(0.5, std::sqrt(3/2)),
    //   vector2(0, 0),
    // };
    static std::array<wheel_setting, 3> _wheel_settings;
    static ackermann_steering steering(wheel_locations, _wheel_settings, 2, 2);// WE ARE UNABLE TO USE MAXIMUM ANGULAR SPEED UNLESS WE HAVE THE CORRECT SCALE FACTORS SET. 


  // mission control object
  static std::array<hal::byte, 8192> recieve_buffer1{};

  static auto uart1 =
    HAL_CHECK((hal::lpc40::uart::get(1, recieve_buffer1, hal::serial::settings{
      .baud_rate = 115200,
    })));

  static constexpr std::string_view ssid = "TP-Link_FC30"; //change to wifi name that you are using
  static constexpr std::string_view password = "R0Bot1cs3250";     // change to wifi password you are using

  hal::print(uart0, "Creating socket\n");
  // still need to decide what we want the static IP to be
  static constexpr std::string_view ip = "192.168.0.224";
  static constexpr auto socket_config = hal::esp8266::at::socket_config{
    .type = hal::esp8266::at::socket_type::tcp,
    .domain = "192.168.0.211",
    .port = 5000,
  };



  hal::print(uart0, "created Socket\n");
  static constexpr std::string_view get_request = "GET /drive HTTP/1.1\r\n"
                                "Host: 192.168.0.211:5000\r\n"
                                "\r\n";

  static std::array<hal::byte, 1024> buffer{};
  // static auto helper = serial_mirror(uart1, uart0);

  auto timeout = hal::create_timeout(counter, 10s);
  static auto esp8266 = HAL_CHECK(hal::esp8266::at::create(uart1, timeout));
  static tcp_client client = HAL_CHECK(tcp_client::create(esp8266, ssid, password, socket_config, ip));
  client.use_debug_serial(uart0);
  auto mc_timeout = hal::create_timeout(counter, 10s);
  hal::print(uart0, "Establishing connection...");
  auto success = client.reestablish_connection(mc_timeout);
  if(success) {
    hal::print(uart0, "Connected to server.\n");
  }else {
    hal::print(uart0, "Failed to connect to server.\n");
  }

  // auto mc_timeout = hal::create_timeout(counter, 10s);
  // static auto esp_mission_control = sjsu::drive::esp8266_mission_control::create(esp8266, 
  //                                 uart0, ssid, password, socket_config, 
  //                                 ip, mc_timeout, buffer, get_request);


  static auto esp_mission_control = HAL_CHECK(sjsu::drive::esp8266_mission_control::create(
                                  // esp8266, 
                                  client,
                                  uart0, 
                                  // ssid, password, socket_config, ip, mc_timeout, 
                                  buffer, 
                                  get_request));
  int i = 0;
  // while(esp_mission_control.has_error()) {
  //   mc_timeout = hal::create_timeout(counter, 30s);
  //   hal::print<128>(uart0, "Pinging the server: %d\n", i);
  //   i++;
  //   esp_mission_control = sjsu::drive::esp8266_mission_control::create(esp8266, 
  //                                   uart0, ssid, password, socket_config, 
  //                                   ip, mc_timeout, buffer, get_request);
  // }
  // hal::print(uart0, "Found Server\n");
  // static auto drive_mission_control = esp_mission_control.value();

  return application_framework{
                              .steering = &steering,
                              #ifdef USE_MOTORS
                              .legs = legs,
                              #endif
                              
                              // .mc = &drive_mission_control,
                              .mc = &esp_mission_control,
                              
                              .terminal = &uart0,
                              .clock = &counter,
                              .reset = []() { hal::cortex_m::reset(); },
                              };
}

}
