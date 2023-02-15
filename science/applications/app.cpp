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

    int automatic = 1; // data from mission control
    int mcButtonPressed = 1;
    float desiredPressure = 90;
    bool sendToMissionControl = false;
    std::string_view response;
    science::science_commands mc_commands{};
    science::science_data mc_data{};


    auto can_router = HAL_CHECK(hal::can_router::create(can));

    science::PumpPwmController air_pump(air_pump_pwm, 1_kHz);                               //unknown freqeuncy atm change when this is figured out
    science::PumpPwmController dosing_pump(dosing_pump_pwm, 1_kHz);
    science::PressureSensor pressure(pressure_adc);
    science::Mq4MethaneSensor methane(methane_adc, methane_gpio);
    science::StateMachine state_machine();

    science::MissionControlHandler mc_handler;

    auto revolver_spinner = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x141));       // RMD can addres may have to chage, this is a temp

    while(true) {
        mc_commands = HAL_CHECK(mc_handler.ParseMissionControlData(response, terminal));
        if(mcButtonPressed == 1){

            if(automatic == 1) {

                //moving revolver to the soil sample
                sendToMissionControl = science::moveRevolver(revolver_spinner);
                while(!HAL_CHECK(revolver_hall_effect.level())){
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopRevolver(revolver_spinner);
                HAL_CHECK(hal::delay(counter, 10ms));

                // sealing super station
                sendToMissionControl = science::seal();  // needs servo
                while(!HAL_CHECK(seal_hall_effect.level())){
                    HAL_CHECK(hal::delay(counter, 10ms)); //needs servo
                }
                sendToMissionControl = science::stopSeal();
                HAL_CHECK(hal::delay(counter, 10ms));
                
                // depressurizing super station
                sendToMissionControl = science::suck(air_pump);
                while(HAL_CHECK(pressure.get_parsed_data()) > desiredPressure) {
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopSucking(air_pump);
                HAL_CHECK(hal::delay(counter, 10ms));

                // injecting chemicals into super station
                sendToMissionControl = science::inject(dosing_pump, 3);
                HAL_CHECK(hal::delay(counter, 10ms));

                // Reading data from sensors
                mcButtonPressed == 0;                                 // this will need to be removed when mc is done being implemented
                while(!mcButtonPressed) {
                    sendToMissionControl = methane.get_parsed_data(dosing_pump); // Maybe always have data being read from sensors?
                    HAL_CHECK(hal::delay(counter, 10ms));             // or have it read for a set amount of time
                    mcButtonPressed == 1;                             // this will need to be removed when mc is done being implemented
                }
                // Clearing chamber
                sendToMissionControl = science::suck(air_pump);
                while(HAL_CHECK(pressure.get_parsed_data()) > desiredPressure) {
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopSucking(air_pump);
                HAL_CHECK(hal::delay(counter, 1s));

                sendToMissionControl = science::unseal(); // need servo
                HAL_CHECK(hal::delay(counter, 2s)); // wait a variable amound of time
            }
            else if(automatic == 0) {
                // do alternative loop
            }
        }
    }
    return hal::success();
}
