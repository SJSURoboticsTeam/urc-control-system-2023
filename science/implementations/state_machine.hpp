#pragma once

#include <libhal/input_pin.hpp>

#include "../implementations/pressure_sensor_driver.hpp"
#include "../dto/science_dto.hpp"

namespace science {

    class StateMachine {
    public:
        static constexpr kpressure_requirement = 90.0;

        enum States {
            Start = 0,
            MoveRevolver = 1,
            StopRevolver = 2,
            Seal = 3,
            Depressurizing = 4,
            StopDepressurizing = 5,
            Inject = 6,
            StopInjection = 7,
            ClearingChamber = 8,
            StopClearingChamber = 9,
            Unseal = 10,
            Wait = 11
            };

        void RunMachine(science_status& status, science_commands commands, float pressure, int revolver_hall, int seal_hall) {
            if(commands.is_operational == 0) {
                previous_state_ = States::Start;
                current_state_ = States::Start;
                status = science_status{}; // reset the status durring e_stop
                return;
            }
            switch(current_state_) {
                case States::Start:
                    status = science_status{}; // reset the status if it goes back to the first state
                    if(commands.state_step == desired_button_value-1) {
                        current_state_ = States::Start;
                    }
                    else if(commands.state_step == desired_button_value) {
                        current_state_ = States::MoveRevolver;
                    }
                break;
                // revolver_hall will come back as low when we start, this will need to be fixed with some weird logic
                
                case States::MoveRevolver:
                    if(revolver_hall == 0) {
                        current_state_ = current_state_;
                        status.move_revolver_status = Status::InProgress;
                    }
                    else if(revolver_hall == 1) {
                        current_state_ = States::StopRevolver;
                    }
                break;

                case States::StopRevolver:
                    if(revolver_hall == 1) {
                        current_state_ = current_state_;
                    }
                    else if(revolver_hall == 0 && commands.mode == 'M') {
                        previous_state_ = current_state_;
                        current_state_ = States::Wait;
                        desired_button_value++;
                    }
                    else if(revolver_hall == 0 && commands.mode == 'A') {
                        current_state_ = States::Seal;
                    }
                    if(revolver_hall == 0) status.move_revolver_status = Status::Complete;
                break;

                case States::Seal:
                    if(seal_hall == 1) {
                        current_state_ = current_state_;
                        status.seal_status = Status::InProgress;
                    }
                    else if(seal_hall == 0 && commands.mode == 'A') {
                        current_state_ = States::Depressurizing;
                    }
                    else if(seal_hall == 0 && commands.mode == 'M') {
                        previous_state_ = current_state_;
                        current_state_ = States::Wait;
                        desired_button_value++;
                    }
                    if(seal_hall == 0) status.seal_status = Status::Complete;
                break;

                case States::Depressurizing: 
                    if(pressure > kpressure_requirement) { // 90.0 is a place holder as of rn
                        current_state_ = current_state_;
                        status.depressurize_status = Status::InProgress;
                    }
                    else if(pressure < kpressure_requirement) {
                        current_state_ = States::StopDepressurizing;
                    }
                break;

                case States::StopDepressurizing:
                    status.depressurize_status = Status::Complete;
                    if(commands.mode == 'M') {
                        previous_state_ = current_state_;
                        current_state_ = States::Wait;
                        desired_button_value++;
                    }
                    else{
                        current_state_ = States::Inject;
                    }
                break;

                case States::Inject:
                    status.inject_status = Status::InProgress; // status will have to be updated outside of state machine for injection
                    current_state_ = States::StopInjection;
                    // goto the wait state to choose when to stop the experiment
                break;

                case States::StopInjection:
                    status.inject_status = Status::Complete;
                    previous_state_ = current_state_;
                    current_state_ = States::Wait;
                    desired_button_value++;
                break;
                
                case States::ClearingChamber:
                    if(pressure > kpressure_requirement) { // 90.0 is a place holder as of rn
                        status.clear_status = Status::InProgress;
                        current_state_ = current_state_;
                    }
                    else if(pressure < kpressure_requirement) {
                        current_state_ = States::StopClearingChamber;
                    }
                break;

                case States::StopClearingChamber:
                    status.clear_status = Status::Complete;
                    current_state_ = States::Unseal;
                break;

                case States::Unseal:
                    if(seal_hall == 0) {
                        current_state_ = current_state_;
                        status.unseal_status = Status::InProgress;
                    }
                    else if(seal_hall == 1) {
                        current_state_ = States::Start;
                        status.unseal_status = Status::Complete;
                        desired_button_value = 1;
                    }
                break;

                case States::Wait: 
                    if(commands.state_step == desired_button_value-1) {
                        current_state_ = current_state_;
                    }
                    else if(commands.state_step == desired_button_value) {
                        current_state_ = previous_state_ + 1;
                    }
                break;
            }
        }

    private:
        int previous_state_ = 0;
        int current_state_ = 0;
        int desired_button_value = 1; // any time the button is clicked, increment this
    };
}
