#pragma once

#include <libhal/input_pin.hpp>

#include "../implementations/pressure_sensor_driver.hpp"
#include "../dto/science_dto.hpp"

namespace science {

    class StateMachine {
    public:
        enum states {
            start = 0, 
            MoveRevolver = 1,
            
            }
        static constexpr int kStart = 0;
        static constexpr int kMoveRevolver = 1;
        static constexpr int kSeal = 2;
        static constexpr int kDepressurize = 3;
        static constexpr int kInject = 4;
        static constexpr int kClearChamber = 5;
        static constexpr int kWait = 6;

        void RunMachine(science_status status, science_commands commands, float pressure, int revolver_hall, int seal_hall) {
            if(commands.e_stop == 1) {
                previous_state_ = kStart;
                current_state_ = kStart;
                return;
            }
            switch(current_state_) {
                case kStart:
                    if(commands.mc_button == 0) {
                        current_state_ = kStart;
                    }
                    else if(commands.mc_button == 1) {
                        current_state_ = kMoveRevolver;
                    }
                break;

                case kMoveRevolver:
                    if(revolver_hall == 1) {
                        current_state_ = current_state_;
                    }
                    else if(revolver_hall == 0 && commands.automatic == 1) {
                        current_state_ = kSeal;
                    }
                    else if(revolver_hall == 0 && commands.automatic == 0) {
                        previous_state_ = current_state_;
                        current_state_ = kWait;
                    }
                break;

                case kSeal: 
                    if(seal_hall == 1) {
                        current_state_ = current_state_;
                    }
                    else if(seal_hall == 0 && commands.automatic == 1) {
                        current_state_ = kDepressurize;
                    }
                    else if(seal_hall == 0 && commands.automatic == 0) {
                        previous_state_ = current_state_;
                        current_state_ = kWait;
                    }
                break;
                
                case kDepressurize: 
                    if(pressure > 90.0) { // 90.0 is a place holder as of rn
                        current_state_ = current_state_;
                    }
                    else if(pressure < 90.0 && commands.automatic == 1) {
                        current_state_ = kInject;
                    }
                    else if(seal_hall == 0 && commands.automatic == 0) {
                        previous_state_ = current_state_;
                        current_state_ = kWait;
                    }
                break;
                
                case kInject: 
                    // goto state 6 unconditionally
                    current_state_ = kClearChamber;
                break;
                
                case kClearChamber:
                    if(pressure > 90.0) { // 90.0 is a place holder as of rn
                        current_state_ = current_state_;
                    }
                    else if(pressure < 90.0) {
                        current_state_ = kStart;
                    }
                break;
                
                case kWait: 
                    if(commands.mc_button == 0) {
                        current_state_ = current_state_;
                    }
                    else if(commands.mc_button == 1) {
                        current_state_ = previous_state_ + 1;
                    }
                break;
            }
        }

        int GetCurrentState() {
            return current_state_;
        }

    private:
        int current_state_ = 0;
        int previous_state_ = 0;
    };
}
