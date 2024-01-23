#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>

#include "../hardware_map.hpp"

#include <string_view>
#include <array>
#include <cinttypes>
using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::arm {
std::string_view to_string_view(std::span<const hal::byte> p_span)
{
return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                        p_span.size());
}
hal::status application(sjsu::arm::application_framework& p_framework) {
    using namespace std::literals;
    auto& terminal = *p_framework.terminal;

    // auto& end_effector_my = *p_framework.end_effector;
    auto pwm0 = HAL_CHECK((hal::lpc40::pwm::get(1, 6)));
    hal::print(terminal, "after pwm create");
    auto servo_settings = hal::soft::rc_servo::settings{
        .min_angle = 0.0_deg, //to be tested with end effector
        .max_angle = 180.0_deg, 
        .min_microseconds = 500,
        .max_microseconds = 2500,
    };

    static auto end_effector_my =
    HAL_CHECK(hal::soft::rc_servo::create(pwm0, servo_settings));
    
    hal::print(terminal, "after rc servo create");

    // auto end_effector_my = HAL_CHECK((hal::lpc40::pwm::get(1, 5)));
    // static auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
    // static auto i2c = HAL_CHECK(hal::lpc40::i2c::get(2)); //need to use pca here

    // static auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c,0b100'0000)); 
    // static auto pwm0 = pca9685.get_pwm_channel<3>(); 
   
    auto& clock = *p_framework.clock;
    std::array<hal::byte, 1024> received_buffer{}; //{100
    while (true) {

        hal::delay(clock, 2000ms);

        hal::print(terminal, "restart\n");
        HAL_CHECK(end_effector_my.position(static_cast<float> (100)));
        // hal::print(terminal, "before delay\n");

        // hal::delay(clock, 3000ms);
        // hal::print(terminal, "after delay\n");
        // HAL_CHECK(end_effector_my.position(static_cast<float> (100)));

        auto readings = HAL_CHECK(terminal.read(received_buffer)); //.data,
        hal::print(terminal, "something happened\n");
        // hal::print<200>(terminal, "result: %s\n", received_buffer);
        // hal::print<200>(terminal, "%.*s",static_cast<int>(readings.data.length()),readings.data());

        // hal::delay(clock, 100ms);
        hal::print<200>(terminal, "readings: %s\n", readings.data);
        auto result = to_string_view(received_buffer);
        // // hal::print<200>(terminal, "result: %s\n", result.data());
        // hal::print<200>(terminal, "%.*s",static_cast<int>(result.length()),result.data());
        if (result.find("{") == std::string_view::npos){
            hal::print(terminal, "could not find\n");
        }
        else{
            hal::print(terminal, "here\n");

            result = result.substr(result.find('{') +1 );
            // char * end; //string view is not null terminated
            float location = (std::clamp(std::stof(result.data()),0.0f,100.0f)/100) * 110; //clamps to value
            hal::print<200>(terminal, "Go to: %f\n", location);
            // hal::delay(clock, 10ms);
            HAL_CHECK(end_effector_my.position(static_cast<float>(location)));
        }
        // hal::delay(clock, 2000ms);
        // HAL_CHECK(end_effector_my.position(static_cast<float>(0)));

        // HAL_CHECK(end_effector_my.position(static_cast<float> (100)));

        // hal::delay(clock, 3000ms);
        // HAL_CHECK(end_effector_my.position(0.0_deg));

    }


    return hal::success();
}


}