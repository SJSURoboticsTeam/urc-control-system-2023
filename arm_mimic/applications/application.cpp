#include <array>
#include <cstdio>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/i2c.hpp>
#include <libhal-util/i2c.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementations/tla2528.hpp"
#include "../implementations/degree_manipulation.hpp"
#include "../implementations/mission_control_handler.hpp"

#include "application.hpp"
namespace sjsu::arm_mimic {

hal::status application(application_framework& p_framework)
{
    using namespace hal::literals;

    auto& steady_clock = *p_framework.steady_clock;
    auto& uart0 = *p_framework.terminal;
    auto& i2c2 = *p_framework.i2c2;
    auto TLA2528 = tla::tla2528(i2c2, steady_clock);

    const uint8_t N = 6;
    std::array<float, N> scrambled_results = {};
    std::array<float, N> results = {0};
    char* strings[] = {"Rotunda", "Shoulder", "Elbow", "Wrist Pitch", "Wrist Roll", "EndE"};
    float true_degree;
    float max_volt = 4096;
    float max_deg = 360;

    while(true){
        // auto results = HAL_CHECK(TLA2528.read_one(channel));
        // hal::print<64>(uart0, "voltage: %d\n", results);

        auto buffer_of_results = HAL_CHECK(TLA2528.read_all());
        for(int i = 0; i < N; i++) {
            true_degree = tla::voltage_to_degree(buffer_of_results[i], max_volt,max_deg);
            scrambled_results[i] = true_degree;
            // scrambled_results[i] = buffer_of_results[i];
            // hal::print<64>(uart0, "volt %d: %d ", i, (int)buffer_of_results[i]);
        }
        // hal::print<64>(uart0, "\n");

        // for(int i = 0; i < N; i++) {
        //     hal::print<64>(uart0, "volt %d: %d ", i, (int)scrambled_results[i]);
        // }
        // hal::print<64>(uart0, "\n");

        // sr1 is shoulder

        // memset(0, results, sizeof(results));

        results[0] = (8*(scrambled_results[0] - 178) - 50);                     // ROTUNDA
        results[1] = -(scrambled_results[1] - 346);
        results[2] = -(scrambled_results[2] - 155);
        results[3] = (scrambled_results[5] - 180)/2;
        results[4] = scrambled_results[4] - 180;
        results[5] = scrambled_results[3]/3;

        // HARD LOCKS
        if(results[0] > 80) { // ROTUNDA
            results[0] = 80;
        } else if (results[0] < -80) {
            results[0] = -80;
        }

        if(results[2] > 85) {
            results[0] = 85;
        }

        if(results[5] > 100) {
            results[5] = 100;
        }

        // HARD STOP ELBOW 85

        // for(int i = 0; i < N; i++) {
        //     hal::print<64>(uart0, "%s:%d  ", strings[i], (int)results[i]);
        // }
        // hal::print<64>(uart0, "\n");

        HAL_CHECK(tla::send_data_to_mc(*p_framework.terminal, results));
    }
}
}