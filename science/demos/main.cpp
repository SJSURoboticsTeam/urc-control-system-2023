// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal/error.hpp>

// Application function must be implemented by one of the compilation units
// (.cpp) files.
extern hal::status application();

int main()
{
  auto is_finished = application();

  if (!is_finished) {
    hal::cortex_m::reset();
  } else {
    hal::halt();
  }

  return 0;
}

namespace boost {
void throw_exception(std::exception const&)
{
  hal::halt();
}
}  // namespace boost