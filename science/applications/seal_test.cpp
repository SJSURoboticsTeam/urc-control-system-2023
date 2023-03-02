#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-pca/pca9685.hpp>
#include <string_view>
#include <libhal-util/serial.hpp>

#include "../hardware_map.hpp"


hal::status application(science::hardware_map &p_map) {
    using namespace std::chrono_literals;
    using namespace hal::literals;

    static constexpr float MIN_SEAL_DUTY_CYCLE = 0.075f;
    static constexpr float MAX_SEAL_DUTY_CYCLE = 0.125f;
    auto& clock = *p_map.clock;
    auto& terminal = *p_map.terminal;
    HAL_CHECK(p_map.seal->frequency(50.0_Hz));
    
    while(true) {
        HAL_CHECK(hal::write(terminal, "Max\n"));
        (void)hal::delay(clock, 5ms);
        HAL_CHECK(p_map.seal->duty_cycle(MAX_SEAL_DUTY_CYCLE));
        (void)hal::delay(clock, 5s);
        (void)hal::write(terminal, "Min\n");
        (void)hal::delay(clock, 5ms);
        HAL_CHECK(p_map.seal->duty_cycle(MIN_SEAL_DUTY_CYCLE));
        HAL_CHECK(hal::delay(clock, 5s));
    }

    return hal::success();
}