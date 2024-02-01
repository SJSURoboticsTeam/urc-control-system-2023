#pragma once

#include <libhal/serial.hpp>

#include <string_view>

namespace Drive {
inline hal::result<std::string_view> get_commands(hal::serial& terminal)
{
  std::array<hal::byte, 8192> buffer{};
  auto received = HAL_CHECK(terminal.read(buffer)).data;
  if (received.size() != 0) {
    auto response = std::string_view(
      reinterpret_cast<const char*>(received.data()), received.size());
    hal::print<200>(
      terminal, "%.*s", static_cast<int>(response.size()), response.data());
    return response;
  } else {
    return std::string_view("");
  }
}
}  // namespace Drive