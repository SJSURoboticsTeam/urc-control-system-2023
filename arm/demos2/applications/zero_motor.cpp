#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "../../platform-implementations/rmd_mcx_configurer.hpp"
#include "application.hpp"

namespace sjsu::arm {


bool starts_with(std::span<hal::byte> string, const char * buffer) {
    for(int i = 0; buffer[i] != '\0'; i ++) {
        if(string.size() <= i) {
            return false;
        }
        if(string[i] != buffer[i]) {
            return false;
        }
    }
    return true;
}

hal::status application(sjsu::arm::application_framework& p_framework)
{

    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& can_router = *p_framework.router;
    auto& terminal = *p_framework.terminal;
    auto& clock = *p_framework.clock;

    auto motor = HAL_CHECK(mcx_configurer::create(can_router, 0x141, clock, 1ms));

    hal::print(terminal, "intialization finished\n");
    // hal::can::message_t msg;

    while(true) {
        std::array<hal::byte, 512> buffer;
        hal::serial::read_t reading = HAL_CHECK(terminal.read(buffer));
        if(reading.data.size() > 0) {
            if(starts_with(reading.data, "zero")) {
                hal::print(terminal, "sending zero command\n");
                (motor.set_current_encoder_position_as_zero());
                hal::print(terminal, "zero command sent\n");
            }else if(starts_with(reading.data, "goto")) {
                float f = std::atof((char *) &reading.data[5]);
                hal::print<1024>(terminal, "Moing to %f°\n", f);
                (motor.set_position(f, 60 * 36 * 5));
            }else if(starts_with(reading.data, "shutdown")) {
                hal::print(terminal, "sending shutdown command\n");
                (motor.shutdown());
                hal::print(terminal, "shutdown command sent\n");
            }else if(starts_with(reading.data, "reset")) {
                hal::print(terminal, "sending reset command\n");
                (motor.reset_system());
                hal::print(terminal, "reset command sent\n");
            }else if(starts_with(reading.data, "where")) {
                hal::print(terminal, "Getting angle");
                auto a = HAL_CHECK(motor.get_motor_angle());
                hal::print<512>(terminal, "Angle: %f°\n", a);
            }
        }
    }

    return hal::success();
}
}  // namespace sjsu::arm