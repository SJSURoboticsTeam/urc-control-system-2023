#pragma once 

#include "./vector_2.hpp"
#include <cmath>
using namespace sjsu::drive;

vector2::vector2(float p_x=0, float p_y=0) : x(p_x), y(p_y) {

};

vector2&& vector2::operator+(const vector2& b) {
    return vector2(x + b.x, y + b.y);
}
vector2&& vector2::operator-(const vector2& b) {
    return vector2(x - b.x, y - b.y);
}
vector2&& vector2::operator*(float s) {
    return vector2(x * s, y * s);
}
vector2&& vector2::operator/(float s) {
    return vector2(x / s, y / s);
}

float vector2::dot(const vector2& a, const vector2& b) {
    return a.x * b.x + a.y * b.y;
}

vector2&& vector2::rotate_90_ccw(const vector2& a) {
    return vector2(-a.y, a.x);
}


float vector2::bearing_angle(const vector2& a) {
    return atan2(a.x, a.y);
}

float vector2::bearing_angle_360(const vector2& a) {
    float angle = bearing_angle(a);
    if(angle < 0) {
        return angle + M_2_PI;
    }
    return angle;
}

float vector2::polar_angle(const vector2& a) {
    return atan2(a.y, a.x);
}

float vector2::polar_angle_360(const vector2& a) {
    float angle = polar_angle(a);
    if(angle < 0) {
        return angle + M_2_PI;
    }
    return angle;
}

vector2&& vector2::from_bearing(float r, float bearing) {
    return vector2(r * sinf(bearing), r * cosf(bearing));
}

vector2&& vector2::from_polar(float r, float theta) {
    return vector2(r * cosf(theta), r * sinf(theta));
}

vector2&& operator+(const vector2& a, const vector2& b) {
    return vector2(a.x + b.x, a.y + b.y);
}
vector2&& operator-(const vector2& a, const vector2& b) {
    return vector2(a.x - b.x, a.y - b.y);
}
vector2&& operator*(const vector2& a, float s) {
    return vector2(a.x * s, a.y * s);
}
vector2&& operator*(float s, const vector2& a) {
    return vector2(a.x * s, a.y  * s);
}