#pragma once
#include "../implementations/mq4_methane_sensor.hpp"
#include "../implementations/pressure_sensor_driver.hpp"
#include "../implementations/pump_controller.hpp"
#include "../implementations/move_revolver.hpp"
#include "../implementations/seal.hpp"
#include "../implementations/suck.hpp"
#include "../implementations/mission_control_handler.hpp"

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>

#include "../hardware_map.hpp"
#include "../dto/science_dto.hpp"

#include <string_view>

hal::status application(science::hardware_map &p_map) {
    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& pressure_adc = *p_map.pressure_sensor_pin;
    auto& methane_gpio = *pmap.is_methane;
    auto& methane_adc = *methane_level;
    auto& hall_effect = *p_map.halleffect
    auto& can = *p_map.can;
    auto& terminal = *p_map.terminal;

    int automatic = 1; // data from mission control
    int mcButtonPressed = 1;
    float desiredPressure = 90;
    bool sendToMissionControl;
    std::string_view response;

    auto can_router = HAL_CHECK(hal::can_router::create(can));

    science::PressureSensor pressure(pressure_adc);
    science::Mq4MethaneSensor methane(methane_adc, methane_gpio);
    science::MissionControlHandler mc_handler;

    auto revolver_spinner = HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x141));       // RMD can addres may have to chage, this is a temp

    while(true) {
        mc_data = mc_handler.ParseMissionControlData(response, terminal);
        if(mcButtonPressed == 1){

            if(automatic == 1) {

                //moving revolver to the soil sample
                sendToMissionControl = science::moveRevolver();
                while(!HAL_CHECK(hall_effect.level())){
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopRevolver();
                HAL_CHECK(hal::delay(counter, 10ms));

                // sealing super station
                sendToMissionControl = science::seal();
                while(!HAL_CHECK(hall_effect.level())){
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopSeal();
                HAL_CHECK(hal::delay(counter, 10ms));
                
                // depressurizing super station
                sendToMissionControl = science::suck();
                while(HAL_CHECK(pressure.get_parsed_data()) > desiredPressure) {
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopSucking();
                HAL_CHECK(hal::delay(counter, 10ms));

                // injecting chemicals into super station
                sendToMissionControl = science::inject(3);
                HAL_CHECK(hal::delay(counter, 10ms));

                // Reading data from sensors
                mcButtonPressed == 0;                                 // this will need to be removed when mc is done being implemented
                while(!mcButtonPressed) {
                    sendToMissionControl = methane.get_parsed_data(); // Maybe always have data being read from sensors?
                    HAL_CHECK(hal::delay(counter, 10ms));             // or have it read for a set amount of time
                    mcButtonPressed == 1;                             // this will need to be removed when mc is done being implemented
                }
                // Clearing chamber
                sendToMissionControl = science::suck();
                while(HAL_CHECK(pressure.get_parsed_data()) > desiredPressure) {
                    HAL_CHECK(hal::delay(counter, 10ms));
                }
                sendToMissionControl = science::stopSucking();
                HAL_CHECK(hal::delay(counter, 10ms));
            }
            else if(automatic == 0) {
                // do alternative loop
            }
        }
    }
    return hal::success();
}
