#pragma once

#include <libhal-util/serial.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::science{

static constexpr char kResponseBodyFormat[] =
  "{\"HB\":%d,\"IO\":%d,\"SR\":%d,\"PP\":\"%d\",\"CR\":\"%d\"}\n";

static constexpr char kGETRequestFormat[] =
  "drive?heartbeat_count=%d&is_operational=%d&sample_recieved=%d&pause_play=%"
  "c&contianment_reset=%d";


class mission_control
{
  public:
  struct mc_commands
  {
    int heartbeat_count = 0;
    int is_operational = 0;
    int sample_recieved = 0;
    int pause_play = 0;
    int contianment_reset =0;

    hal::status print(hal::serial* terminal)
    {
      hal::print<128>(*terminal,
                      "HB: %d\n"
                      "IS_OP: %d\n"
                      "Sample_Recieved: %d\n"
                      "Pause_Play: %d\n"
                      "Contianment_Reset: %d\n",
                      heartbeat_count,
                      is_operational,
                      sample_recieved,
                      pause_play,
                      contianment_reset);
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