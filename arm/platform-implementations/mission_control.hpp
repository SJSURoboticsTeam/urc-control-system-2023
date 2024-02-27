#pragma once

#include <libhal-util/serial.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::arm {

constexpr char kResponseBodyFormat[] =
  "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%"
  "d,%d,%d,%d,%d]}\n";
constexpr char kGETRequestFormat[] =
  "arm?heartbeat_count=%d&is_operational=%d&speed=%d&rotunda_angle=%d&shoulder_"
  "angle=%d&elbow_angle=%d&wrist_pitch_angle=%d&wrist_roll_angle=%d&rr9_angle=%"
  "d";

constexpr const char * get_request_format = 
  "GET /arm?"
  "dt=%.4f&"
  "rotunda_motor_angle=%.2f&"
  "rotunda_motor_speed=%.2f&"
  "rotunda_motor_current=%.4f&"
  "rotunda_motor_temperature=%.4f&"
  "shoulder_motor_angle=%.2f&"
  "shoulder_motor_speed=%.2f&"
  "shoulder_motor_current=%.4f&"
  "shoulder_motor_temperature=%.4f&"
  "elbow_motor_angle=%.2f&"
  "elbow_motor_speed=%.2f&"
  "elbow_motor_current=%.4f&"
  "elbow_motor_temperature=%.4f&"
  "wrist_left_motor_angle=%.2f&"
  "wrist_left_motor_speed=%.2f&"
  "wrist_left_motor_current=%.4f&"
  "wrist_left_motor_temperature=%.4f&"
  "wrist_right_motor_angle=%.2f&"
  "wrist_right_motor_speed=%.2f&"
  "wrist_right_motor_current=%.4f&"
  "wrist_right_motor_temperature=%.4f"
  " HTTP/1.1\r\n"
  "Host: 192.168.0.211:5000\r\n"
  "Connection: keep-alive\r\n"
  "\r\n";

class mission_control
{
public:

  struct mc_feedback {
    float dt = 0.0;
    struct motor_feedback {
      float angle = 0.0;
      float speed = 0.0;
      float current = 0.0;
      float temperature = 0.0;
    };

    motor_feedback rotunda;
    motor_feedback shoulder;
    motor_feedback elbow;
    motor_feedback wristLeft;
    motor_feedback wristRight;
  };
  inline void set_feedback(mc_feedback p_feedback) {
    m_feedback = p_feedback;
  }

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

    hal::status print(hal::serial* terminal)
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
      return hal::success();
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
  hal::result<mc_commands> get_command(
    hal::function_ref<hal::timeout_function> p_timeout)
  {
    return impl_get_command(p_timeout);
  }

  virtual ~mission_control() = default;

  mc_feedback m_feedback;
private:
  virtual hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) = 0;
};

}  // namespace sjsu::arm