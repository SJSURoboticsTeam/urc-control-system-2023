#pragma once

#include <libhal/input_pin.hpp>

#include "../dto/science_dto.hpp"
#include "../implementations/pressure_sensor_driver.hpp"

namespace science {

class state_machine
{
public:
  static constexpr float pressure_requirement = 90.0;

  enum states
  {
    start = 0,
    move_revolver = 1,
    stop_revolver = 2,
    seal = 3,
    depressurizing = 4,
    stopdepressurizing = 5,
    inject = 6,
    stop_injection = 7,
    clearing_chamber = 8,
    stop_clearing_chamber = 9,
    unseal = 10,
    finish = 11,
    wait = 12
  };

  void run_machine(science_status& status,
                   science_commands commands,
                   float pressure,
                   int revolver_hall,
                   hal::serial& terminal)
  {
    if (commands.is_operational == 0) {
      previous_state = states::start;
      current_state = states::start;
      // reset the status durring e_stop
      status = science_status{};
      return;
    }
    switch (current_state) {
      case states::start:
        // reset the status if it goes back to the
        status = science_status{};
        if (commands.state_step == desired_button_value - 1) {
          current_state = states::start;
        } else if (commands.state_step == desired_button_value) {
          current_state = states::move_revolver;
        }
        break;
        // revolver_hall will come back as low when we start, this will need to
        // be fixed with some weird logic

      case states::move_revolver:
        if (revolver_hall == 1) {
          current_state = current_state;
          status.move_revolver_status = status::in_progress;
        } else if (revolver_hall == 0) {
          current_state = states::stop_revolver;
        }
        break;

      case states::stop_revolver:
        if (revolver_hall == 1) {
          current_state = current_state;
        } else if (revolver_hall == 0 && commands.mode == 'M') {
          previous_state = current_state;
          current_state = states::wait;
          desired_button_value++;
        } else if (revolver_hall == 0 && commands.mode == 'A') {
          current_state = states::seal;
        }
        if (revolver_hall == 0)
          status.move_revolver_status = status::complete;
        break;

      case states::seal:
        status.seal_status = status::in_progress;
        if (commands.mode == 'A') {
          current_state = states::depressurizing;
        } else if (commands.mode == 'M') {
          previous_state = current_state;
          current_state = states::wait;
          desired_button_value++;
        }
        break;

      case states::depressurizing:
        status.seal_status = status::complete;
        // 90.0 is a place holder as of rn
        if (pressure > pressure_requirement) {
          current_state = current_state;
          status.depressurize_status = status::in_progress;
        } else if (pressure < pressure_requirement) {
          current_state = states::stopdepressurizing;
          status.depressurize_status = status::complete;
        }
        break;

      case states::stopdepressurizing:
        if (commands.mode == 'M') {
          previous_state = current_state;
          current_state = states::wait;
          desired_button_value++;
        } else {
          current_state = states::inject;
        }
        break;

      case states::inject:
        // status will have to be updated outside of
        status.inject_status = status::in_progress;
        current_state = states::stop_injection;
        // goto the wait state to choose when to stop the experiment
        break;

      case states::stop_injection:
        status.inject_status = status::complete;
        previous_state = current_state;
        current_state = states::wait;
        desired_button_value++;
        break;

      case states::clearing_chamber:
        // 90.0 is a place holder as of rn
        if (pressure > pressure_requirement) {
          status.clear_status = status::in_progress;
          current_state = current_state;
        } else if (pressure < pressure_requirement) {
          current_state = states::stop_clearing_chamber;
        }
        break;

      case states::stop_clearing_chamber:
        status.clear_status = status::complete;
        current_state = states::unseal;
        break;

      case states::unseal:
        status.unseal_status = status::in_progress;
        break;

      case states::finish:
        current_state = states::start;
        status.unseal_status = status::complete;
        desired_button_value = 1;
        break;

      case states::wait:
        if (commands.state_step == desired_button_value - 1) {
          current_state = current_state;
        } else if (commands.state_step == desired_button_value) {
          current_state = previous_state + 1;
        }
        break;
    }
  }

private:
  int previous_state = 0;
  int current_state = 0;
  // any time the button is clicked, increment this
  int desired_button_value = 1;
};
}  // namespace science
