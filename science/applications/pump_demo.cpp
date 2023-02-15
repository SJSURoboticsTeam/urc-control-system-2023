#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "../hardware_map.hpp"
#include "../implementations/pump_controller.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;


const hal::hertz DOSING_PUMP_PWM_FREQUENCY = 32000;
const hal::hertz AIR_PUMP_PWM_FREQUENCY = 32000;

const bool pwm_impl_exists = false;

hal::status application(science::hardware_map& p_map) {
    if constexpr (!pwm_impl_exists) {
        HAL_CHECK(hal::write(*p_map.science_serial, "Demo cannot be ran as no pwm API exists for lpc40xx"));
        return hal::new_error("Demo cannot be ran as no pwm API exists for lpc40xx");
    }

    while (true)
    {
        science::PressureSensor pressure_sensor(p_map.pressure_sensor_pin);

        science::PumpPwmController dosing_pump(p_map.dosing_pump, DOSING_PUMP_PWM_FREQUENCY);
        science::PumpPwmController air_pump(p_map.air_pump, AIR_PUMP_PWM_FREQUENCY);

        hal::print<128>(*p_map.science_serial, "Dosing pump starting at: %f\n", dosing_pump.frequency());
        hal::print<128>(*p_map.science_serial, "Air pump starting at: %f\n", air_pump.frequency());
        HAL_CHECK(hal::delay(*p_map.clock, 250ms));

        dosing_pump.start_flow(500);
        air_pump.start_flow(750);

        hal::print<128>(*p_map.science_serial, "Dosing pump ending at: %f\n", dosing_pump.frequency());
        hal::print<128>(*p_map.science_serial, "Air pump ending at: %f\n", air_pump.frequency());
        
        HAL_CHECK(hal::delay(*p_map.clock, 500ms));
        dosing_pump.stop_flow();
        air_pump.run_vacuum(pressure_sensor);
        HAL_CHECK(hal::delay(*p_map.clock, 500ms));
    }

    return hal::success();    
}
