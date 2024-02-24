#pragma once

#include "drive_configuration.hpp"

namespace sjsu::drive {
    class drive_configuration_updater {
        public: 
            void set_target(drive_configuration p_target);
            void update(float dt);
            drive_configuration get_current();
            drive_configuration get_target();
            drive_configuration get_delta();
            void set_kP(drive_configuration p_kP);
            void set_max_delta(drive_configuration p_max_delta);
        private:
            drive_configuration m_current;
            drive_configuration m_target;
            drive_configuration m_kP = drive_configuration(1, 10, 20);
            drive_configuration m_delta;
            drive_configuration m_max_delta = drive_configuration(40, 360, 50);
    };
}