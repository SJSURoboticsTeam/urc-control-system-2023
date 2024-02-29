#pragma once

#include <libhal-util/serial.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::drive{

static constexpr char kResponseBodyFormat[] =
  "{\"HB\":%d,\"IO\":%d,\"WO\":%d,\"DM\":\"%c\",\"CMD\":[%d,%d]}\n";

static constexpr char kGETRequestFormat[] =
  "drive?heartbeat_count=%d&is_operational=%d&wheel_orientation=%d&drive_mode=%"
  "c&speed=%d&angle=%d";

static constexpr const char* get_request_format = "GET /drive?"
  "dt=%.6f&"
  "current_wheel_speed=%.2f&current_steering_angle=%.2f&current_wheel_heading=%.2f&"
  "delta_wheel_speed=%.2f&delta_steering_angle=%.2f&delta_wheel_heading=%.2f&"
  "fl_steering_angle=%.4f&fl_steering_speed=%.4f&fl_steering_current=%.4f&"
  "fl_propulsion_angle=%.4f&fl_propulsion_speed=%.4f&fl_propulsion_current=%.4f&"
  "fl_requested_steering_angle=%.2f&fl_requested_propulsion_speed=%.2f&"
  "fr_steering_angle=%.4f&fr_steering_speed=%.4f&fr_steering_current=%.4f&"
  "fr_propulsion_angle=%.4f&fr_propulsion_speed=%.4f&fr_propulsion_current=%.4f&"
  "fr_requested_steering_angle=%.2f&fr_requested_propulsion_speed=%.2f&"
  "b_steering_angle=%.4f&b_steering_speed=%.4f&b_steering_current=%.4f&"
  "b_propulsion_angle=%.4f&b_propulsion_speed=%.4f&b_propulsion_current=%.4f&"
  "b_requested_steering_angle=%.2f&b_requested_propulsion_speed=%.2f"
  " HTTP/1.1\r\nHost: 192.168.0.211:5000\r\nConnection:keep-alive\r\n\r\n";

class mission_control
{
  public:
  struct mc_feedback {
    float dt = 0.0f, mc_ping = 0.0;

    float current_wheel_speed = 0.0;
    float current_steering_angle = 0.0;
    float current_wheel_heading = 0.0;

    float delta_wheel_speed = 0.0;
    float delta_steering_angle = 0.0;
    float delta_wheel_heading = 0.0;

    struct motor_feedback {
      float angle = 0.0f;
      float speed = 0.0f;
      float current = 0.0f;
    };
    struct leg_feedback {
      motor_feedback steering;
      motor_feedback propulsion;
      float requested_steering_angle = 0.0;
      float requested_propulsion_speed = 0.0;
    }; 

    leg_feedback fl, fr, b;
  };

  virtual void set_feedback(mc_feedback p_feedback) = 0;
  struct mc_commands
  {
    char mode = 'D';
    int speed = 0;
    int angle = 0;
    int wheel_orientation = 0;

    int message_count = 0;
    float wheel_speed = 0.0;
    float steering_angle = 0.0;
    float wheel_heading = 0.0;


    int is_operational = 0;
    int heartbeat_count = 0;
    hal::status print(hal::serial* terminal)
    {
      hal::print<128>(*terminal,
                      "HB: %d\n"
                      "IS_OP: %d\n"
                      "Speed: %d\n"
                      "Angle: %d\n"
                      "Mode: %c\n"
                      "Wheel Orientation: %d\n",
                      heartbeat_count,
                      is_operational,
                      speed,
                      angle,
                      mode,
                      wheel_orientation);
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
  hal::result<mc_commands> get_command(hal::function_ref<hal::timeout_function> p_timeout)
  {
    return impl_get_command(p_timeout);
  }

  virtual ~mission_control() = default;

private:
  virtual hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) = 0;
};

}