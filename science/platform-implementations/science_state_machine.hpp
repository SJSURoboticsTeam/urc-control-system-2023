#pragma once
#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal/units.hpp>

namespace sjsu::science {
class science_state_machine
{
private:
  science_state_machine(sjsu::science::application_framework& p_application);  
  sjsu::science::application_framework& hardware;
  int m_count;

public:
  struct status
  {
    int heartbeat_count = 0;
    int is_operational = 1;
    int sample_recieved = 0;
    int pause_play = 0;
    int contianment_reset = 0;
    int num_vials_used = 0;
    int is_sample_finished = 0;
  };
  status m_status;

  enum class science_states
  {
    GET_SAMPLES,
    MOLISCH_TEST,
    BIURET_TEST,
    RESET
  };
  // auto sm = science_state_machine()
  // science_state_machine::create()

  // vial2_position current_position= SAMPLE;
  // science_states current_state= GET_SAMPLES;

  static hal::result<science_state_machine> create(
    application_framework& p_application);
  hal::status run_state_machine(science_states current_state);

  hal::status mix_solution();
  hal::status turn_on_pump(auto pump, hal::time_duration duration);
  hal::status move_sample(int position);
  hal::status containment_reset();

  int get_num_vials_left();
  status get_status();
  // hal::status turn_off_pumps();
};

}  // namespace sjsu::science