#pragma once

#include <cmath>

#define PI 3.14158265


// Returns the minimum of two values--works for any numerical type
template <typename T> T min(T val1, T val2) {
    return val1 > val2 ? val2 : val1;
}
// Returns the maximum of two values--works for any numerical type
template <typename T> T max(T val1, T val2) {
    return val1 > val2 ? val1 : val2;
}

// Returns a linearly interpolated value between values A and B using alpha
template <typename T> T lerp(T a, T b, float alpha) {
    return a * (1.0f - alpha) + b * alpha;
}