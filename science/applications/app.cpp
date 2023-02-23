#include "../implementations/mq4_methane_sensor.hpp"
#include "../implementations/pressure_sensor_driver.hpp"
#include "../implementations/pump_controller.hpp"
#include "../implementations/seal.hpp"
// #include "../implementations/suck.hpp"
// #include "../implementations/inject.hpp"
#include "../implementations/mission_control_handler.hpp"
#include "../implementations/state_machine.hpp"
#include "../implementations/co2_sensor.hpp"

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../hardware_map.hpp"
#include "../dto/science_dto.hpp"

#include <string_view>

hal::status application(science::hardware_map &p_map) {
    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& pressure_adc = *p_map.pressure_sensor_pin;
    auto& methane_adc = *p_map.methane_level;
    auto& revolver_hall_effect = *p_map.revolver_hall_effect;
    auto& revolver_spinner = *p_map.revolver_spinner;
    auto& seal_hall_effect = *p_map.seal_hall_effect;
    auto& can = *p_map.can;
    auto& terminal = *p_map.terminal;
    auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(*p_map.i2c, 0b100'0000));
    auto pca_pwm_0 = pca9685.get_pwm_channel<0>();
    auto pca_pwm_1 = pca9685.get_pwm_channel<1>();
    auto pca_pwm_2 = pca9685.get_pwm_channel<2>();
    auto& clock = *p_map.clock;

    std::string response;
    int revolver_hall_value = 1;
    int seal_hall_value = 1;

    // Constants
    static constexpr float MIN_SEAL_DUTY_CYCLE = 0.065f;
    static constexpr float MAX_SEAL_DUTY_CYCLE = 0.085f;

    auto can_router = HAL_CHECK(hal::can_router::create(can));

    // science::PumpPwmController air_pump(pca_pwm_0, 1000.0_Hz);                               // unknown freqeuncy atm change when this is figured out
    // science::PumpPwmController dosing_pump(pca_pwm_1, 1000.0_Hz);
    science::PressureSensor pressure(pressure_adc);
    science::Mq4MethaneSensor methane(methane_adc);
    auto carbon_dioxide = HAL_CHECK(science::Co2Sensor::create(*p_map.i2c, clock));
    science::StateMachine state_machine;

    science::MissionControlHandler mc_handler;
    science::science_commands mc_commands;
    science::science_data mc_data;

    HAL_CHECK(revolver_spinner.frequency(50.0_Hz));
    HAL_CHECK(p_map.seal_servo->frequency(50._Hz));

    while(true) {
        mc_commands = HAL_CHECK(mc_handler.ParseMissionControlData(response, terminal));
        mc_data.pressure_level = HAL_CHECK(pressure.get_parsed_data());
        HAL_CHECK(hal::delay(clock, 5ms));
        mc_data.methane_level = HAL_CHECK(methane.get_parsed_data());
        HAL_CHECK(hal::delay(clock, 5ms));
        revolver_hall_value = HAL_CHECK(revolver_hall_effect.level()).state;
        seal_hall_value = HAL_CHECK(seal_hall_effect.level()).state;
        HAL_CHECK(hal::delay(clock, 5ms));
        HAL_CHECK(hal::delay(*p_map.clock, 500ms));
        mc_data.co2_level = HAL_CHECK(carbon_dioxide.read_co2());
        HAL_CHECK(hal::delay(*p_map.clock, 1000ms));


        state_machine.RunMachine(mc_data.status, mc_commands, mc_data.pressure_level, revolver_hall_value, seal_hall_value);
        if(mc_data.status.move_revolver_status == science::Status::InProgress) {
            HAL_CHECK(revolver_spinner.duty_cycle(0.085f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.move_revolver_status == science::Status::Complete && mc_data.status.seal_status == science::Status::NotStarted) {
            HAL_CHECK(revolver_spinner.duty_cycle(0.075f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.seal_status == science::Status::InProgress) {
            HAL_CHECK(p_map.seal_servo->duty_cycle(MAX_SEAL_DUTY_CYCLE));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.depressurize_status == science::Status::InProgress) {
            // Suck(air_pump);
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.depressurize_status == science::Status::Complete && mc_data.status.inject_status == science::Status::NotStarted) {
            // StopSucking(air_pump);
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.inject_status == science::Status::InProgress) {
            // Inject(dosing_pump, 2);
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.clear_status == science::Status::InProgress) {
            // Suck(air_pump);
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.clear_status == science::Status::Complete && mc_data.status.unseal_status == science::Status::NotStarted) {
            // StopSucking(air_pump);
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.unseal_status == science::Status::InProgress) {
            HAL_CHECK(p_map.seal_servo->duty_cycle(MIN_SEAL_DUTY_CYCLE));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        response = mc_handler.CreateGETRequestParameterWithRoverStatus(mc_data);
    }
    return hal::success();
}
