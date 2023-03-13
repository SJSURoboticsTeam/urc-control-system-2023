#pragma once

#include <string>
#include <string_view>

#include <libhal-util/serial.hpp>
#include <libhal/serial.hpp>

inline std::string_view to_string_view(std::span<const hal::byte> p_span)
{
  return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                          p_span.size());
}

inline std::string get_rover_status()
{
  return "?HB=0&IO=1";
}