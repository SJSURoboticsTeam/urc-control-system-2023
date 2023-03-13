#include "hardware_map.hpp"

int main()
{
  auto init_result = initialize_target();

  if (!init_result) {
    hal::halt();
  }

  auto hardware_map = init_result.value();
  auto is_finished = application(hardware_map);

  if (!is_finished) {
    hardware_map.reset();
  } else {
    hal::halt();
  }

  return 0;
}

namespace boost {
void throw_exception([[maybe_unused]] std::exception const& p_error)
{
  std::abort();
}
}  // namespace boost
