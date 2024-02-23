#pragma once

#include <array>
#include <cstdint>
#include <span>

struct frame_parser {
    enum frame_bytes : std::uint8_t {
        START_OF_FRAME=0x01, // Start of header
        END_OF_FRAME=0x04, // End of transmission
        ESCAPE=0x27,
    };

    enum parser_state {
        WAIT_START_OF_FRAME,
        PARSING_MESSAGE,
        AFTER_ESCAPE,
    };

    enum frame_result {
        FRAME_FINISHED,
        FRAME_UNFINISHED,
        FRAME_ERROR,
    };

    parser_state state = parser_state::WAIT_START_OF_FRAME;

    constexpr static std::size_t max_frame_size = 64;

    std::array<std::uint8_t, max_frame_size> frame;
    std::size_t i = 0;

    frame_result process_byte(std::uint8_t byte) {
        switch(state) {
        case parser_state::WAIT_START_OF_FRAME:
            if(byte == frame_bytes::START_OF_FRAME) {
                state = parser_state::PARSING_MESSAGE;
                i = 0;
            }
            return frame_result::FRAME_UNFINISHED;
        case parser_state::PARSING_MESSAGE:
            if(byte == frame_bytes::ESCAPE) {
                state = parser_state::AFTER_ESCAPE;
                return frame_result::FRAME_UNFINISHED;
            }else if(byte == frame_bytes::END_OF_FRAME) {
                state = parser_state::WAIT_START_OF_FRAME;
                return frame_result::FRAME_FINISHED;
            }
            if(i == max_frame_size) {
                state = parser_state::WAIT_START_OF_FRAME;
                return frame_result::FRAME_ERROR;
            }
            frame[i] = byte;
            i++;
            return frame_result::FRAME_UNFINISHED;
        case parser_state::AFTER_ESCAPE:
            state = parser_state::PARSING_MESSAGE;
            if(i == max_frame_size) {
                state = parser_state::WAIT_START_OF_FRAME;
                return frame_result::FRAME_ERROR;
            }
            frame[i] = byte;
            i ++;
            return frame_result::FRAME_UNFINISHED;
        };
        return frame::FRAME_ERROR;
    }

    std::span<std::uint8_t> get_frame() {
        std::span<std::uint8_t> frame_span = frame;
        frame_span = frame_span.subspan(0, i);
        return frame_span;
    }
};