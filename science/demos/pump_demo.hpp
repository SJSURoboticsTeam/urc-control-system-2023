#pragma once

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "../libhal_configuration/hardware_map.hpp"
#include "../implementations/pump_controller.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

namespace demos {

const hal::hertz DOSING_PUMP_PWM_FREQUENCY = 32000;
const hal::hertz AIR_PUMP_PWM_FREQUENCY = 32000;

constexpr bool pwm_impl_exists = false;

hal::status pump_demo(science::hardware_map& p_map) {
    if constexpr (!pwm_impl_exists) {
        hal::write(*p_map.science_serial, "Demo cannot be ran as no pwm API exists for lpc40xx");
        return hal::new_error("Demo cannot be ran as no pwm API exists for lpc40xx");
    }

    while (true)
    {
        PressureSensor pressure_sensor = PressureSensor(p_map.pressure_sensor_pin);

        PumpPwmController dosing_pump = PumpPwmController(p_map.dosing_pump, DOSING_PUMP_PWM_FREQUENCY);
        PumpPwmController air_pump = PumpPwmController(p_map.air_pump, AIR_PUMP_PWM_FREQUENCY);

        hal::print<128>(*p_map.science_serial, "Dosing pump starting at: %f\n", dosing_pump.frequency());
        hal::print<128>(*p_map.science_serial, "Air pump starting at: %f\n", air_pump.frequency());
        hal::delay(*p_map.clock, 250ms);

        dosing_pump.start_flow(500);
        air_pump.start_flow(750);

        hal::print<128>(*p_map.science_serial, "Dosing pump ending at: %f\n", dosing_pump.frequency());
        hal::print<128>(*p_map.science_serial, "Air pump ending at: %f\n", air_pump.frequency());
        
        hal::delay(*p_map.clock, 500ms);
        dosing_pump.stop_flow();
        air_pump.run_vacuum(pressure_sensor);
        hal::delay(*p_map.clock, 500ms);
    }

    return hal::success();    
}

} // namespace demos