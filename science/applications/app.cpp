#pragma once
#include "../implementations/mq4_methane_sensor.hpp"
#include "../implementations/pressure_sensor_driver.hpp"
#include "../implementations/pump_controller.hpp"
#include "../implementations/move_revolver.hpp"
#include "../implementations/seal.hpp"
#include "../implementations/suck.hpp"
#include "../implementations/inject.hpp"
#include "../implementations/mission_control_handler.hpp"
#include "../implementations/state_machine.hpp"

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal-rmd/drc.hpp>

#include "../hardware_map.hpp"
#include "../dto/science_dto.hpp"

#include <string_view>

hal::status application(science::hardware_map &p_map) {
    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& pressure_adc = *p_map.pressure_sensor_pin;
    auto& methane_gpio = *pmap.is_methane;
    auto& methane_adc = *methane_level;
    auto& revolver_hall_effect = *p_map.revolver_hall_effect;
    auto& seal_hall_effect = *p_map.seal_hall_effect;
    auto& can = *p_map.can;
    auto& terminal = *p_map.terminal;
    auto& air_pump_pwm = *p_map.air_pump;
    auto& dosing_pump_pwm = *p_map.dosing_pump;

    std::string_view response;
    int revolver_hall_value = 1;
    int seal_hall_value = 1;


    auto can_router = HAL_CHECK(hal::can_router::create(can));

    science::PumpPwmController air_pump(air_pump_pwm, 1_kHz);                               // unknown freqeuncy atm change when this is figured out
    science::PumpPwmController dosing_pump(dosing_pump_pwm, 1_kHz);
    science::PressureSensor pressure(pressure_adc);
    science::Mq4MethaneSensor methane(methane_adc, methane_gpio);
    science::StateMachine state_machine();

    science::MissionControlHandler mc_handler;
    science::science_commands mc_commands;
    science::science_data mc_data;

    auto revolver_spinner = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x141));       // RMD can addres may have to chage, this is a temp

    while(true) {
        mc_commands = HAL_CHECK(mc_handler.ParseMissionControlData(response, terminal));
        mc_data.pressure_level = pressure.get_parsed_data();
        HAL_CHECK(hal::delay(counter, 5ms));
        mc_data.methane_level = methane.get_parsed_data();
        HAL_CHECK(hal::delay(counter, 5ms));
        revolver_hall_value = revolver_hall_effect.level();
        seal_hall_value = seal_hall_effect.level();
        HAL_CHECK(hal::delay(counter, 5ms));

        state_machine.RunMachine(mc_data.status, mc_commands, pressure_data, revolver_hall_value, seal_hall_value);
        if(mc_data.status.move_revolver_status == science::Status::InProgress) {
            MoveRevolver(revolver_spinner);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.move_revolver_status == science::Status::Complete && mc_data.status.seal_status == science::Status::NotStarted) {
            StopRevolver(revolver_spinner);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.seal_status == science::Status::InProgress) {
            Seal();
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.depressurize_status == science::Status::InProgress) {
            Suck(air_pump);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.depressurize_status == science::Status::Complete && mc_data.status.inject_status == science::Status::NotStarted) {
            StopSucking(air_pump);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.inject_status == science::Status::InProgress) {
            Inject(dosing_pump, 2);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.clear_status == science::Status::InProgress) {
            Suck(air_pump);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.clear_status == science::Status::Complete && mc_data.status.unseal_status == science::Status::NotStarted) {
            StopSucking(air_pump);
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        else if(mc_data.status.unseal_status == science::Status::InProgress) {
            Unseal();
            HAL_CHECK(hal::delay(counter, 5ms));
        }
        response = CreateGETRequestParameterWithRoverStatus(mc_data);
    }
    return hal::success();
}
