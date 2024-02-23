#pragma once 

#include <cmath>
#include <numbers>


namespace sjsu::drive {

/**
 * @brief A simple Vector 2D library.
 * 
 * @note all angles are in radians, since math is easier in radians.
 * 
 */
class vector2 {
    public:
        float x, y;

        constexpr vector2(float p_x=0, float p_y=0) : x(p_x), y(p_y) {}

        constexpr vector2 operator+(const vector2& b){
            return vector2(x + b.x, y + b.y);
        }
        constexpr vector2 operator-(const vector2& b) {
            return vector2(x - b.x, y - b.y);
        }
        constexpr vector2 operator*(float s) {
            return vector2(x * s, y * s);
        }
        constexpr vector2 operator/(float s) {
            return vector2(x / s, y / s);
        }

        static constexpr float dot(const vector2& a, const vector2& b) {
            return a.x * b.x + a.y * b.y;
        }


        /**
         * @brief Returns a vector rotated 90 degrees counter clockwise when looking from above
         * (x points towards the right, y points upwards)
         * 
         * @return vector2 
         */
        static constexpr vector2 rotate_90_ccw(const vector2& a) {
            return vector2(-a.y, a.x);
        }

        /**
         * @brief Returns a vector rotated 90 degrees clockwise when looking from above
         * (x points towards the right, y points upwards)
         * 
         * @return vector2 
         */
        static constexpr vector2 rotate_90_cw(const vector2& a) {
            return vector2(a.y, -a.x);
        }


        /**
         * @brief Returns the signed angle in radians between a vector and the upwards vector (positive y-axis).
         * Returns a negative angle for vectors to the left of the y-axis.
         * Returns a positive angle for vectors to the right of the y-axis.
         * 
         * @param a 
         * @return float 
         */
        static constexpr float bearing_angle(const vector2& a) {
            return atan2(a.x, a.y);
        }

        /**
         * @brief Returns the angle clockwise in radians between a vector and the upwards vector (positive y-axis).
         * Returns a positive angle between 0 and 2pi.
         * 
         * @param a 
         * @return float 
         */
        static constexpr float bearing_angle_2pi(const vector2& a) {
            float angle = bearing_angle(a);
            if(angle < 0) {
                return angle + std::numbers::pi * 2;
            }
            return angle;
        }

        /**
         * @brief Returns the signed angle in radians between a vector and the right vector (positive x-axis).
         * Returns a negative angle for vectors to the below of the x-axis.
         * Returns a positive angle for vectors to the above of the x-axis.
         * 
         * @param a 
         * @return float 
         */
        static constexpr float polar_angle(const vector2& a){
            return atan2(a.y, a.x);
        }


        /**
         * @brief Returns the counter-clockwise angle in radians between a vector and the right vector (positive x-axis).
         * Returns a positive angle between 0 and 2pi.
         * 
         * @param a 
         * @return float 
         */
        static constexpr float polar_angle_2pi(const vector2& a) {
            float angle = polar_angle(a);
            if(angle < 0) {
                return angle + std::numbers::pi * 2;
            }
            return angle;
        }

        /**
         * @brief Return a vector a distance `r` from the origin and has an clockwise angle `bearing` from the positive y-axis
         * 
         * @param r Distance of vector from origin
         * @param bearing Clockwise angle from positive y-axis
         * @return vector2&& 
         */
        static constexpr vector2 from_bearing(float r, float bearing) {
            return vector2(r * sinf(bearing), r * cosf(bearing));
        }
        /**
         * @brief Return a vector a distance `r` from the origin and has an counter-clockwise angle `theta` from the positive x-axis
         * 
         * @param r Distance of vector from origin
         * @param theta Counter-Clockwise angle from positive x-axis
         * @return vector2&& 
         */
        static constexpr vector2 from_polar(float r, float theta) {
            return vector2(r * cosf(theta), r * sinf(theta));
        }

        /**
         * @brief Euclidean length of a vector
         * 
         * @param a 
         * @return float 
         */
        static constexpr float length(const vector2& a) {
            return sqrt(vector2::length_squared(a));
        }
        /**
         * @brief Euclidean length of a vector squared
         * 
         * @param a 
         * @return float 
         */
        static constexpr float length_squared(const vector2& a) {
            return vector2::dot(a,a);
        }

        /**
         * @brief Angle between two vectors, in radians
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static constexpr float angle_between(const vector2& a, const vector2& b) {
            return acos(vector2::cos_of_angle_between(a, b));
        } 

        /**
         * @brief Cosine of the angle between two vectors.
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static constexpr float cos_of_angle_between(const vector2& a, const vector2& b) {
            return vector2::dot(a, b) / (vector2::length(a) * vector2::length(b));
        }
        /**
         * @brief Non-Standard "Cross Product" of 2 2d vectors. Returns the z component of the resulting 3d cross product with their "z" components set to 0
         * Used to determine whether the angle from `a` to `b` is clockwise or counter-clockwise
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static constexpr float cross_2d(const vector2& a, const vector2& b){
            return (a.x * b.y) - (a.y * b.x);
        }

        /**
         * @brief Returns true if the angle from `a` to `b` is clockwise or counter-clockwise 
         * 
         * @param a 
         * @param b 
         * @return true 
         * @return false 
         */
        static constexpr bool is_clockwise(const vector2& a, const vector2& b) {
            return cross_2d(a, b) < 0;
        }
};
}

// sjsu::drive::vector2&& operator+(const sjsu::drive::vector2& a, const sjsu::drive::vector2& b);
// sjsu::drive::vector2&& operator-(const sjsu::drive::vector2& a, const sjsu::drive::vector2& b);
// sjsu::drive::vector2&& operator*(const sjsu::drive::vector2& a, float s);
constexpr sjsu::drive::vector2 operator*(float s, const sjsu::drive::vector2& a) {
    return sjsu::drive::vector2(a.x * s, a.y * s);
}