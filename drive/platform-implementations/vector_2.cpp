#pragma once 

#include "./vector_2.hpp"
#include <cmath>
#include <numbers>

using namespace sjsu::drive;

constexpr vector2::vector2(float p_x, float p_y) : x(p_x), y(p_y) {

};

constexpr vector2 vector2::operator+(const vector2& b) {
    return vector2(x + b.x, y + b.y);
}
constexpr vector2 vector2::operator-(const vector2& b) {
    return vector2(x - b.x, y - b.y);
}
constexpr vector2 vector2::operator*(float s) {
    return vector2(x * s, y * s);
}
constexpr vector2 vector2::operator/(float s) {
    return vector2(x / s, y / s);
}

constexpr float vector2::dot(const vector2& a, const vector2& b) {
    return a.x * b.x + a.y * b.y;
}

constexpr vector2 vector2::rotate_90_ccw(const vector2& a) {
    return vector2(-a.y, a.x);
}


constexpr float vector2::bearing_angle(const vector2& a) {
    return atan2(a.x, a.y);
}

constexpr float vector2::bearing_angle_2pi(const vector2& a) {
    float angle = bearing_angle(a);
    if(angle < 0) {
        return angle + std::numbers::pi * 2;
    }
    return angle;
}

constexpr float vector2::polar_angle(const vector2& a) {
    return atan2(a.y, a.x);
}

constexpr float vector2::polar_angle_2pi(const vector2& a) {
    float angle = polar_angle(a);
    if(angle < 0) {
        return angle + std::numbers::pi * 2;
    }
    return angle;
}

constexpr float vector2::length(const vector2& a) {
    return sqrt(vector2::length_squared(a));
}

constexpr float vector2::length_squared(const vector2& a) {
    return vector2::dot(a,a);
}

constexpr float vector2::angle_between(const vector2& a, const vector2& b) {
    return acos(vector2::cos_of_angle_between(a, b));
} 

constexpr float vector2::cos_of_angle_between(const vector2& a, const vector2& b) {
    return vector2::dot(a, b) / (vector2::length(a) * vector2::length(b));
}

constexpr float vector2::cross_2d(const vector2& a, const vector2& b) {
    return (a.x * b.y) - (a.y * b.x);
}

constexpr bool vector2::is_clockwise(const vector2& a, const vector2& b) {
    return cross_2d(a, b) < 0;
}

constexpr vector2 vector2::from_bearing(float r, float bearing) {
    return vector2(r * sinf(bearing), r * cosf(bearing));
}

constexpr vector2 vector2::from_polar(float r, float theta) {
    return vector2(r * cosf(theta), r * sinf(theta));
}
constexpr vector2 operator*(const vector2& a, float s) {
    return vector2(a.x * s, a.y * s);
}