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

#pragma once

#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include <libhal/pwm.hpp>
#include <libhal-pca/pca9685.hpp>

namespace sjsu::science {

struct application_framework
{
  hal::servo* rotunda_science;
  hal::serial* terminal;
  // mission_control* mc;
  hal::steady_clock* clock;
  hal::callback<void()> reset;
};

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

} 
