#include "rmd_mcx_configurer.hpp"

#include <libhal-util/steady_clock.hpp>

using namespace sjsu::arm;

mcx_configurer::response_waiter::response_waiter(mcx_configurer* p_this) : m_this(p_this) {
    m_original_message_number = m_this->m_message_number;
}

hal::status mcx_configurer::response_waiter::wait() {
    auto timeout = hal::create_timeout(*m_this->m_clock, m_this->m_timeout_duration);
    while (true) {
        if (m_original_message_number != m_this->m_message_number) {
            return hal::success();
        }
        HAL_CHECK(timeout());
    }
    return hal::success();
}

// void mcx_configurer::on_message(const hal::can::message_t& p_message) {
//     m_message_number++;
//     last_message = p_message;
// }

void mcx_configurer::operator()(const hal::can::message_t& p_message){
    m_message_number++;
    last_message = p_message;
}

hal::result<mcx_configurer> mcx_configurer::create(
        hal::can_router& p_router, 
        hal::can::id_t p_mcx_address, 
        hal::steady_clock& p_clock, 
        hal::time_duration p_timeout_duration) {
    return mcx_configurer(p_router, p_mcx_address, p_clock, p_timeout_duration);
}

mcx_configurer::mcx_configurer(hal::can_router& p_router, hal::can::id_t p_mcx_address, hal::steady_clock& p_clock, hal::time_duration p_timeout_duration) :
    m_router(&p_router), 
    m_clock(&p_clock), 
    m_mcx_address(p_mcx_address), 
    m_timeout_duration(p_timeout_duration),
    m_route_item(p_router.add_message_callback(p_mcx_address - 0x140 + 0x240)) {
    m_route_item.get().handler = std::ref(*this);
}

hal::result<hal::can::message_t> mcx_configurer::send_command_and_wait_for_reply(std::span<hal::byte> p_data) {
    hal::can::message_t message;
    message.id = m_mcx_address;
    for(int i = 0; i < p_data.size(); i++) {
        message.payload[i] = p_data[i];
    }
    message.length = 8;

    response_waiter waiter(this);
    HAL_CHECK(m_router->bus().send(message));
    HAL_CHECK(waiter.wait());

    return last_message;
}

static std::int32_t inline combine_bytes(hal::byte msb, hal::byte second, hal::byte third, hal::byte lsb) {
    return (static_cast<std::int32_t>(msb) << 24) |
        (static_cast<std::int32_t>(second) << 16) |
        (static_cast<std::int32_t>(third) << 8) |
        (static_cast<std::int32_t>(lsb) << 0);
}

static std::int16_t inline combine_bytes(hal::byte msb, hal::byte lsb) {
    return (static_cast<std::int32_t>(msb) << 8) |
        (static_cast<std::int32_t>(lsb) << 0);
}

hal::result<mcx_configurer::encoder_position_t> mcx_configurer::get_multiturn_encoder_position() {
    std::array<hal::byte, 8> command = {
        0x60,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    encoder_position_t encoder_position = combine_bytes(response.payload[7],response.payload[6],response.payload[5],response.payload[4]);
    return encoder_position;
}

hal::status mcx_configurer::set_multiturn_encoder_zero_position(std::int32_t encoder_position) {
    std::array<hal::byte, 8> command = {
        0x63,
        0x00,
        0x00,
        0x00,
        static_cast<std::uint8_t>((encoder_position >> 0) & 0xff),
        static_cast<std::uint8_t>((encoder_position >> 8) & 0xff),
        static_cast<std::uint8_t>((encoder_position >> 16) & 0xff),
        static_cast<std::uint8_t>((encoder_position >> 24) & 0xff),
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    encoder_position_t response_encoder_position = combine_bytes(response.payload[7],response.payload[6],response.payload[5],response.payload[4]);
    // response_encoder_position and encoder_position should match
    return hal::success();
}

hal::result<mcx_configurer::encoder_position_t> mcx_configurer::set_current_encoder_position_as_zero() {
    std::array<hal::byte, 8> command = {
        0x64,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    encoder_position_t encoder_zero_position = combine_bytes(response.payload[7],response.payload[6],response.payload[5],response.payload[4]);
    return encoder_zero_position;

}

hal::result<mcx_configurer::angle_t> mcx_configurer::get_motor_angle() {
    std::array<hal::byte, 8> command = {
        0x92,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    encoder_position_t angle_raw = combine_bytes(response.payload[7],response.payload[6],response.payload[5],response.payload[4]);
    angle_t angle = static_cast<float>(angle_raw) * 0.01;
    return angle;
}

hal::status mcx_configurer::shutdown() {
    std::array<hal::byte, 8> command = {
        0x80,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    return hal::success();
}
hal::status mcx_configurer::stop() {
    std::array<hal::byte, 8> command = {
        0x81,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    return hal::success();
}


hal::result<mcx_configurer::motor_status_1_t> mcx_configurer::get_motor_status_1() {
    std::array<hal::byte, 8> command = {
        0x9A,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    motor_status_1_t status;
    status.temperature = static_cast<float>(response.payload[1]);
    status.is_braking = response.payload[3] == 0x01;
    std::int16_t voltage_raw = combine_bytes(response.payload[5], response.payload[4]);
    status.voltage = static_cast<float>(voltage_raw) * 0.1;
    status.error_status = combine_bytes(response.payload[7], response.payload[6]);
    return status;
}
hal::result<mcx_configurer::motor_status_2_t> mcx_configurer::get_motor_status_2() {
    std::array<hal::byte, 8> command = {
        0x9C,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    motor_status_2_t status;
    status.temperature = static_cast<float>(response.payload[1]);
    std::int16_t torque_current_raw = combine_bytes(response.payload[3], response.payload[2]);
    status.torque_current = static_cast<float>(torque_current_raw) * 0.01;
    std::int16_t speed_raw = combine_bytes(response.payload[5], response.payload[4]);
    status.motor_speed = static_cast<float>(speed_raw) * 1;
    std::int16_t position_raw = combine_bytes(response.payload[7], response.payload[6]);
    status.motor_shaft_angle = static_cast<float>(position_raw) * 1;
    return status;

}
hal::result<mcx_configurer::motor_status_3_t> mcx_configurer::get_motor_status_3() {
    std::array<hal::byte, 8> command = {
        0x9D,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    motor_status_3_t status;
    status.temperature = static_cast<float>(response.payload[1]);
    std::int16_t phase_A_current_raw = combine_bytes(response.payload[3], response.payload[2]);
    status.phase_A_current = static_cast<float>(phase_A_current_raw) * 0.01;
    std::int16_t phase_B_current_raw = combine_bytes(response.payload[5], response.payload[4]);
    status.phase_B_current = static_cast<float>(phase_B_current_raw) * 0.01;
    std::int16_t phase_C_current_raw = combine_bytes(response.payload[7], response.payload[6]);
    status.phase_C_current = static_cast<float>(phase_C_current_raw) * 0.01;
    return status;
}

hal::result<mcx_configurer::motor_status_2_t> mcx_configurer::increment_position(mcx_configurer::angle_t incremental_angle, mcx_configurer::speed_t max_speed) {
    std::int16_t max_speed_int = static_cast<std::int16_t>(max_speed * 1);
    std::int32_t incremental_angle_int = static_cast<std::int32_t>(incremental_angle * 100);
    std::array<hal::byte, 8> command = {
        0xA8,
        0x00,
        static_cast<std::uint8_t>((max_speed_int >> 0) & 0xff),
        static_cast<std::uint8_t>((max_speed_int >> 8) & 0xff),
        static_cast<std::uint8_t>((incremental_angle_int >> 0) & 0xff),
        static_cast<std::uint8_t>((incremental_angle_int >> 8) & 0xff),
        static_cast<std::uint8_t>((incremental_angle_int >> 16) & 0xff),
        static_cast<std::uint8_t>((incremental_angle_int >> 24) & 0xff),
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    motor_status_2_t status;
    status.temperature = static_cast<float>(response.payload[1]);
    std::int16_t torque_current_raw = combine_bytes(response.payload[3], response.payload[2]);
    status.torque_current = static_cast<float>(torque_current_raw) * 0.01;
    std::int16_t speed_raw = combine_bytes(response.payload[5], response.payload[4]);
    status.motor_speed = static_cast<float>(speed_raw) * 1;
    std::int16_t position_raw = combine_bytes(response.payload[7], response.payload[6]);
    status.motor_shaft_angle = static_cast<float>(position_raw) * 1;
    return status;
}

hal::result<mcx_configurer::motor_status_2_t> mcx_configurer::set_position(mcx_configurer::angle_t position_angle, mcx_configurer::speed_t max_speed) {
    std::int16_t max_speed_int = static_cast<std::int16_t>(max_speed * 1);
    std::int32_t position_angle_int = static_cast<std::int32_t>(position_angle * 100);
    std::array<hal::byte, 8> command = {
        0xA5,
        0x00,
        static_cast<std::uint8_t>((max_speed_int >> 0) & 0xff),
        static_cast<std::uint8_t>((max_speed_int >> 8) & 0xff),
        static_cast<std::uint8_t>((position_angle_int >> 0) & 0xff),
        static_cast<std::uint8_t>((position_angle_int >> 8) & 0xff),
        static_cast<std::uint8_t>((position_angle_int >> 16) & 0xff),
        static_cast<std::uint8_t>((position_angle_int >> 24) & 0xff),
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    motor_status_2_t status;
    status.temperature = static_cast<float>(response.payload[1]);
    std::int16_t torque_current_raw = combine_bytes(response.payload[3], response.payload[2]);
    status.torque_current = static_cast<float>(torque_current_raw) * 0.01;
    std::int16_t speed_raw = combine_bytes(response.payload[5], response.payload[4]);
    status.motor_speed = static_cast<float>(speed_raw) * 1;
    std::int16_t position_raw = combine_bytes(response.payload[7], response.payload[6]);
    status.motor_shaft_angle = static_cast<float>(position_raw) * 1;
    return status;
}


hal::result<mcx_configurer::loop_mode_t> mcx_configurer::get_loop_mode() {
    std::array<hal::byte, 8> command = {
        0x70,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    loop_mode_t mode = static_cast<loop_mode_t>(response.payload[7]);
    return mode;
}
hal::result<mcx_configurer::power_t> mcx_configurer::get_running_power() {
    std::array<hal::byte, 8> command = {
        0x71,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    std::int16_t power_raw = combine_bytes(response.payload[7], response.payload[6]);
    power_t mode = static_cast<power_t>(power_raw) * 0.1;
    return mode;

}

hal::status mcx_configurer::reset_system() {
    // NOTE: This command does not have a reply. Therefore we cannot wait for a reply. 
    // So we can just send the can message manually.
    hal::can::message_t message;
    message.id = m_mcx_address;
    message.length = 8;
    message.payload = {
        0x76,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    HAL_CHECK(m_router->bus().send(message));

    return hal::success();
}
hal::status mcx_configurer::release_brake() {
    std::array<hal::byte, 8> command = {
        0x77,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    return hal::success();
}
hal::status mcx_configurer::enable_brake() {
    std::array<hal::byte, 8> command = {
        0x78,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    hal::can::message_t response = HAL_CHECK(send_command_and_wait_for_reply(command));
    return hal::success();
}