#pragma once

#define BOOST_LEAF_EMBEDDED
#define BOOST_LEAF_NO_THREADS

#include <string_view>

namespace hal::config {
constexpr std::string_view platform = "stm32f103";
}  // namespace hal::config
