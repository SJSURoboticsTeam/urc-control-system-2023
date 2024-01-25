#pragma once 


namespace sjsu::drive {

class vector2 {
    public:
        float x, y;

        vector2(float p_x=0, float p_y=0);

        vector2&& operator+(const vector2& b);
        vector2&& operator-(const vector2& b);
        vector2&& operator*(float s);
        vector2&& operator/(float s);

        static float dot(const vector2& a, const vector2& b);


        /**
         * @brief Returns a vector rotated 90 degrees counter clockwise when looking from above
         * (x points towards the right, y points upwards)
         * 
         * @return vector2 
         */
        static vector2&& rotate_90_ccw(const vector2& a);


        /**
         * @brief Returns the signed angle in radians between a vector and the upwards vector (positive y-axis).
         * Returns a negative angle for vectors to the left of the y-axis.
         * Returns a positive angle for vectors to the right of the y-axis.
         * 
         * @param a 
         * @return float 
         */
        static float bearing_angle(const vector2& a);

        /**
         * @brief Returns the angle clockwise in radians between a vector and the upwards vector (positive y-axis).
         * Returns a positive angle between 0 and 360.
         * 
         * @param a 
         * @return float 
         */
        static float bearing_angle_360(const vector2& a);

        /**
         * @brief Returns the signed angle in radians between a vector and the right vector (positive x-axis).
         * Returns a negative angle for vectors to the below of the x-axis.
         * Returns a positive angle for vectors to the above of the x-axis.
         * 
         * @param a 
         * @return float 
         */
        static float polar_angle(const vector2& a);

        /**
         * @brief Returns the counter-clockwise angle in radians between a vector and the right vector (positive x-axis).
         * Returns a positive angle between 0 and 360.
         * 
         * @param a 
         * @return float 
         */
        static float polar_angle_360(const vector2& a);

        /**
         * @brief Return a vector a distance `r` from the origin and has an clockwise angle `bearing` from the positive y-axis
         * 
         * @param r Distance of vector from origin
         * @param bearing Clockwise angle from positive y-axis
         * @return vector2&& 
         */
        static vector2&& from_bearing(float r, float bearing);
        /**
         * @brief Return a vector a distance `r` from the origin and has an counter-clockwise angle `theta` from the positive x-axis
         * 
         * @param r Distance of vector from origin
         * @param theta Counter-Clockwise angle from positive x-axis
         * @return vector2&& 
         */
        static vector2&& from_polar(float r, float theta);

        /**
         * @brief Euclidean length of a vector
         * 
         * @param a 
         * @return float 
         */
        static float length(const vector2& a);
        /**
         * @brief Euclidean length of a vector squared
         * 
         * @param a 
         * @return float 
         */
        static float length_squared(const vector2& a);

        /**
         * @brief Angle between two vectors, in radians
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float angle_between(const vector2& a, const vector2& b);

        /**
         * @brief Cosine of the angle between two vectors.
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float cos_of_angle_between(const vector2& a, const vector2& b);
};
}
sjsu::drive::vector2&& operator+(const sjsu::drive::vector2& a, const sjsu::drive::vector2& b);
sjsu::drive::vector2&& operator-(const sjsu::drive::vector2& a, const sjsu::drive::vector2& b);
sjsu::drive::vector2&& operator*(const sjsu::drive::vector2& a, float s);
sjsu::drive::vector2&& operator*(float s, const sjsu::drive::vector2& a);