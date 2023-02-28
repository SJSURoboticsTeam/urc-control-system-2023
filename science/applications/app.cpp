#include "../implementations/mq4_methane_sensor.hpp"
#include "../implementations/pressure_sensor_driver.hpp"
#include "../implementations/pump_controller.hpp"
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
    auto& seal_spinner = *p_map.seal;
    auto& can = *p_map.can;
    auto& terminal = *p_map.terminal;
    auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(*p_map.i2c, 0b100'0000));
    auto pca_pwm_0 = pca9685.get_pwm_channel<0>();
    auto pca_pwm_1 = pca9685.get_pwm_channel<1>();
    auto pca_pwm_2 = pca9685.get_pwm_channel<2>();
    auto& clock = *p_map.clock;

    std::string response;
    int revolver_hall_value = 1;

    // Constants
    static constexpr float MIN_SEAL_DUTY_CYCLE = 0.065f;
    static constexpr float MAX_SEAL_DUTY_CYCLE = 0.085f;

    auto can_router = HAL_CHECK(hal::can_router::create(can));

    // science::PumpPwmController air_pump(pca_pwm_0, 1000.0_Hz);                               // unknown freqeuncy atm change when this is figured out
    // science::PumpPwmController dosing_pump(pca_pwm_1, 1000.0_Hz);
    science::PressureSensor pressure(pressure_adc);
    science::Mq4MethaneSensor methane(methane_adc);
    // auto carbon_dioxide = HAL_CHECK(science::Co2Sensor::create(*p_map.i2c, clock));
    science::StateMachine state_machine;

    science::MissionControlHandler mc_handler;
    science::science_commands mc_commands;
    science::science_data mc_data;

    HAL_CHECK(revolver_spinner.frequency(50.0_Hz));
    HAL_CHECK(hal::delay(clock, 10ms));
    HAL_CHECK(pca_pwm_0.frequency(1.50_kHz));
    HAL_CHECK(hal::delay(clock, 10ms));
    mc_commands.is_operational = 1;

    while(true) {
        // mc_commands = HAL_CHECK(mc_handler.ParseMissionControlData(response, terminal));
        mc_data.pressure_level = HAL_CHECK(pressure.get_parsed_data());
        HAL_CHECK(hal::delay(clock, 5ms));
        mc_data.methane_level = HAL_CHECK(methane.get_parsed_data());
        HAL_CHECK(hal::delay(clock, 5ms));
        revolver_hall_value = HAL_CHECK(revolver_hall_effect.level()).state;
        HAL_CHECK(hal::delay(clock, 5ms));
        // mc_data.co2_level = HAL_CHECK(carbon_dioxide.read_co2());
        mc_data.pressure_level = 100;
        

        state_machine.RunMachine(mc_data.status, mc_commands, mc_data.pressure_level, revolver_hall_value, terminal);
        mc_commands.state_step = 1;
        if(mc_data.status.move_revolver_status == science::Status::InProgress) {
            HAL_CHECK(revolver_spinner.duty_cycle(0.085f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.move_revolver_status == science::Status::Complete && mc_data.status.seal_status == science::Status::NotStarted) {
            HAL_CHECK(revolver_spinner.duty_cycle(0.075f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.seal_status == science::Status::InProgress) {
            HAL_CHECK(seal_spinner.duty_cycle(MAX_SEAL_DUTY_CYCLE));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.depressurize_status == science::Status::InProgress) {
            //start vacuum pump. Assume 40v input. Step down to 12v.
            //12/40 = 0.30
            HAL_CHECK(pca_pwm_0.duty_cycle(0.40f)); // 12/30
            HAL_CHECK(hal::delay(clock, 5ms));
            HAL_CHECK(hal::delay(clock, 5ms));
            mc_data.pressure_level = 0;
        }
        else if(mc_data.status.depressurize_status == science::Status::Complete && mc_data.status.inject_status == science::Status::NotStarted) {
            //stop vacuum pump
            HAL_CHECK(pca_pwm_0.duty_cycle(0.00f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.inject_status == science::Status::InProgress) {
            //start injecting dosing pumps. Assume 40v input. Step down to 6v.
            //6/40 = 0.15
            HAL_CHECK(pca_pwm_1.duty_cycle(0.15f));
            HAL_CHECK(pca_pwm_2.duty_cycle(0.15f));
            HAL_CHECK(hal::delay(clock, 5ms));
            HAL_CHECK(hal::delay(clock, 5s));
            mc_data.pressure_level = 100;
        }
        else if(mc_data.status.inject_status == science::Status::Complete){
            //stop injecting dosing pumps
            HAL_CHECK(pca_pwm_1.duty_cycle(0.00f));
            HAL_CHECK(pca_pwm_2.duty_cycle(0.00f));
            HAL_CHECK(hal::delay(clock, 5ms));
            mc_commands.state_step = 2;
        }
        else if(mc_data.status.clear_status == science::Status::InProgress) {
            //start vacuum pump. Assume 40v input. Step down to 12v.
            //12/40 = 0.30
            HAL_CHECK(pca_pwm_0.duty_cycle(0.40f)); // 12/30
            HAL_CHECK(hal::delay(clock, 5ms));
            HAL_CHECK(hal::delay(clock, 5s));
            mc_data.pressure_level = 0;
        }
        else if(mc_data.status.clear_status == science::Status::Complete && mc_data.status.unseal_status == science::Status::NotStarted) {
            //stop vacuum pump
            HAL_CHECK(pca_pwm_0.duty_cycle(0.00f));
            HAL_CHECK(hal::delay(clock, 5ms));
        }
        else if(mc_data.status.unseal_status == science::Status::InProgress) {
            HAL_CHECK(seal_spinner.duty_cycle(MIN_SEAL_DUTY_CYCLE));
            HAL_CHECK(hal::delay(clock, 2s));
        }
        response = mc_handler.CreateGETRequestParameterWithRoverStatus(mc_data);
        mc_data.status.Print(terminal);
        HAL_CHECK(hal::delay(clock, 1s));
    }
    return hal::success();
}
