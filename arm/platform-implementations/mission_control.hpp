#pragma once

#include <libhal-util/serial.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::arm {

class mission_control
{
public:
  struct mc_commands
  {
    int heartbeat_count = 0;
    int is_operational = 0;
    int speed = 1;
    int rotunda_angle = 0;
    int shoulder_angle = 0;
    int elbow_angle = 0;
    int wrist_pitch_angle = 0;
    int wrist_roll_angle = 0;
    int rr9_angle = 0;

    void print(hal::serial* terminal) const
    {
      hal::print<128>(*terminal,
                      "HB: %d\n"
                      "IS_OP: %d\n"
                      "Speed: %d\n"
                      "Rotunda: %d\n"
                      "Shoulder: %d\n"
                      "Elbow: %d\n"
                      " WRP: %d\n"
                      " WRR: %d\n"
                      " RR9: %d\n",
                      heartbeat_count,
                      is_operational,
                      speed,
                      rotunda_angle,
                      shoulder_angle,
                      elbow_angle,
                      wrist_pitch_angle,
                      wrist_roll_angle,
                      rr9_angle);
    }
  };
  /**
   * @brief Get the command object
   *
   * This command is infallible and will always return a command.
   * If mission control is disconnected from the machine, this command will
   * return the last known value. The heartbeat_count can be used to indicate
   * when the connection has been re-established.
   *
   * It is the responsibility of this driver to re-establishing the connection.
   * This driver is allowed to utilize time during this call in order to
   * reestablish the connection.
   *
   * The timeout is used to break out of the function. This is useful, because
   * attempting to reestablish a connection to mission control could take a
   * considerable amount of time and thus, could stall out the application.
   *
   * @param p_timeout - the amount of time to either get the latest command or
   * attempt to reestablish a connection to mission control.
   * @return drive_commands - the last known driver command received. If no
   * commands have been received, then this should return the default
   * initialized command.
   */
  mc_commands get_command()
  {
    return impl_get_command();
  }

  virtual ~mission_control() = default;

private:
  virtual mc_commands impl_get_command() = 0;
};
}  // namespace sjsu::arm