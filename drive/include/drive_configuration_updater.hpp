#pragma once

#include "drive_configuration.hpp"

namespace sjsu::drive {
    /**
     * @brief This class helps smoothly change driving configurations without moving
     * any of the wheels too fast or accelerating to quickly.
     * 
     * Basically does this formula: `current += clamp((current - target) * kP, -max_change, max_change)`
     * for `steering_angle`, `wheel_heading`, and `wheel_speed`.
     */
    class drive_configuration_updater {
        public: 
            /**
             * @brief Set the target configuration
             * 
             * @param p_target 
             */
            void set_target(drive_configuration p_target);
            /**
             * @brief Update the current configuration
             * 
             * @param dt Change in time since last call
             */
            void update(float dt);
            /**
             * @brief Get the current configuration
             * 
             * @return drive_configuration 
             */
            drive_configuration get_current();
            /**
             * @brief Get the current target
             * 
             * @return drive_configuration 
             */
            drive_configuration get_target();
            /**
             * @brief Get the difference calculated in the last update
             * @note units/sec
             * 
             * @return drive_configuration 
             */
            drive_configuration get_delta();
            /**
             * @brief Set the kP. This affects how sensitive changes to the target is.
             * 
             * @param p_kP 
             */
            void set_kP(drive_configuration p_kP);
            /**
             * @brief Set the max delta. The affects the maximum change to the current configuration.
             * @note units/sec
             * 
             * @param p_max_delta 
             */
            void set_max_delta(drive_configuration p_max_delta);
        private:
            drive_configuration m_current;
            drive_configuration m_target;
            drive_configuration m_kP;
            drive_configuration m_delta;
            drive_configuration m_max_delta;
    };
}