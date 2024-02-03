#pragma once

#include <libhal/can.hpp>
#include <libhal-canrouter/can_router.hpp>
#include <libhal/timeout.hpp>
#include <libhal/steady_clock.hpp>

#include <span>

namespace sjsu::arm {
/**
 * @brief This is an extended driver for the mcx motor. It will probably not be turned its soft drivers for anything at the moment.
 */
class mcx_configurer {
public:

    using encoder_position_t = std::int32_t; // Encoder Shaft Position
    using angle_t = float; // Angle in degrees
    using speed_t = float; // Rotational Speed in degrees per second
    using power_t = float; // Power in watts
    using error_status_t = std::uint16_t; // Error Status

    /**
     * @brief Create a MCX Configurer.
     * 
     * @param p_router CAN Router
     * @param p_mcx_address Address of the motor
     * @param p_clock Used for the timeout
     * @param p_timeout_duration Duration to wait for a reply from the motor before timing out.
     * @return hal::result<mcx_configurer> 
     */
    static hal::result<mcx_configurer> create(
        hal::can_router& p_router, 
        hal::can::id_t p_mcx_address, 
        hal::steady_clock& p_clock, 
        hal::time_duration p_timeout_duration);
    
    /**
     * @brief Get the multiturn encoder position of the motor
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<encoder_position_t> Position of the multiturn encoder
     */
    hal::result<encoder_position_t> get_multiturn_encoder_position();
    // hal::result<encoder_position_t> get_multiturn_encoder_position_without_offset();
    // hal::result<encoder_position_t> get_multiturn_encoder_zero_offset();
    /**
     * @brief Set the multiturn encoder zero position in ROM.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * @note This will write the encoder position to ROM. To use the newly set encoder position, you need to
     *      power cycle the motor.
     * @param encoder_position Encoder position to set as zero position
     * @return hal::status 
     */
    hal::status set_multiturn_encoder_zero_position(std::int32_t encoder_position);
    /**
     * @brief Set the current encoder position as zero in ROM.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * @note This will write the encoder position to ROM. To use the newly set encoder position, you need to
     *      power cycle the motor.
     * 
     * @return hal::result<encoder_position_t> 
     */
    hal::result<encoder_position_t> set_current_encoder_position_as_zero();

    /**
     * @brief Get the motor angle.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<angle_t> 
     */
    hal::result<angle_t> get_motor_angle();

    /**
     * @brief Shut down the motor. 
     * @warning No clue if the powercycles or not. RMD documentation is unclear.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::status 
     */
    hal::status shutdown();
    /**
     * @brief Stop the motor.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::status 
     */
    hal::status stop();

    /**
     * @brief Status information returned from calling `get_motor_get_status_1()`
     */
    struct motor_status_1_t {
        float temperature; // Temperature in °C
        bool is_braking; // `true` if braking. `false` otherwise.
        float voltage; // Motor Voltage in volts
        error_status_t error_status; // Check RMD Documentation. Not yet implemented
    };
    
    /**
     * @brief Status information returned from calling `get_motor_get_status_2()`
     */
    struct motor_status_2_t {
        float temperature; // Temperature in °C
        float torque_current; // Torque Current in amps
        float motor_speed; // Motor Speed in degrees per second
        float motor_shaft_angle; // Motor Shaft Angle in degrees
    };
    
    /**
     * @brief Status information returned from calling `get_motor_get_status_2()`
     */
    struct motor_status_3_t {
        float temperature; // Temperature in °C
        float phase_A_current; // Phase A Current in amps
        float phase_B_current; // Phase B Current in amps
        float phase_C_current; // Phase C Current in amps
    };
    /**
     * @brief Get motor status 1
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<motor_status_1_t> 
     */
    hal::result<motor_status_1_t> get_motor_status_1();
    /**
     * @brief Get motor status 2
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<motor_status_2_t> 
     */
    hal::result<motor_status_2_t> get_motor_status_2();
    /**
     * @brief Get motor status 3
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<motor_status_3_t> 
     */
    hal::result<motor_status_3_t> get_motor_status_3();
    /**
     * @brief Move the motor forward or backwards a set amount of degrees with a maximum speed.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @param incremental_angle in degrees
     * @param max_speed in degrees per second
     * @return hal::result<motor_status_2_t> 
     */
    hal::result<motor_status_2_t> increment_position(angle_t incremental_angle, speed_t max_speed);
    /**
     * @brief Move the motor to an angle with a maximum speed.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @param incremental_angle in degrees
     * @param max_speed in degrees per second
     * @return hal::result<motor_status_2_t> 
     */
    hal::result<motor_status_2_t> set_position(angle_t position_angle, speed_t max_speed);

    enum loop_mode_t {
        current_loop=0x01,
        speed_loop=0x02,
        position_loop=0x03
    };
    /**
     * @brief Get the current control loop mode of the motor.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<loop_mode_t> 
     */
    hal::result<loop_mode_t> get_loop_mode();
    /**
     * @brief Get the running power of the motor.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::result<power_t> 
     */
    hal::result<power_t> get_running_power();

    /**
     * @brief Resets the motor system software.
     * @warning No clue if this power cycles the motor. RMD Documentation is unclear.
     * @note This method DOES NOT BLOCK. There is no reply to wait for when running this command.
     * 
     * @return hal::status 
     */
    hal::status reset_system();
    /**
     * @brief Release the brakes on the motor.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::status 
     */
    hal::status release_brake();
    /**
     * @brief Enable the brakes on the motor.
     * @note This method blocks until the timeout expires or a reply from the motor is recieved.
     * 
     * @return hal::status 
     */
    hal::status enable_brake();

    // void on_message(const hal::can::message_t& p_message);
    /**
     * @brief This object as a "functor" callback.
     * This will get called everytime we have a can message thats addressed to this motor.
     * 
     * @param p_message 
     */
    void operator()(const hal::can::message_t& p_message);

    mcx_configurer(mcx_configurer&& p_other);
    mcx_configurer& operator=(mcx_configurer&& p_other);
    
    std::uint32_t m_message_number = 0;
    hal::can::message_t last_message;
private:
    /**
     * @brief Construct a new mcx configurer object
     * 
     * @param p_router 
     * @param p_mcx_address 
     * @param p_clock 
     * @param p_timeout_duration 
     */
    mcx_configurer(hal::can_router& p_router, hal::can::id_t p_mcx_address, hal::steady_clock& p_clock, hal::time_duration p_timeout_duration);


    hal::can_router* m_router;
    hal::can_router::route_item m_route_item;
    hal::can::id_t m_mcx_address;

    hal::steady_clock* m_clock;
    hal::time_duration m_timeout_duration;

    /**
     * @brief Send a payload and wait for a response.
     * 
     * @param p_data 
     * @return hal::result<hal::can::message_t> 
     */
    hal::result<hal::can::message_t> send_command_and_wait_for_reply(std::span<hal::byte> p_data);

    /**
     * @brief Helper struct that waits for a reply from the motor.
     * Essentially spinlocks until the message count increases or the timeout has expired.
     */
    struct response_waiter {
        mcx_configurer* m_this;
        std::uint32_t m_original_message_number = 0;
        response_waiter(mcx_configurer* p_this);

        /**
         * @brief Spinlock until the message_number has changed or the timeout has expired.
         * 
         * @return hal::status 
         */
        hal::status wait();
    };
};

}
