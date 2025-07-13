#pragma once
#include <random>
#include <Vector3.h>

namespace Utilities {
    constexpr float clamp(float value, float min, float max) {
        return (value < min) ? min : (value > max) ? max : value;
    }
    inline float randomFloat() {
        static std::mt19937 rng(std::random_device{}()); // random engine
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f); // range [0,1)
        return dist(rng);
    }
    inline Vector3 randomInUnitSphere() {
        while (true) {
            Vector3 p = Vector3(randomFloat() * 2 - 1, randomFloat() * 2 - 1, randomFloat() * 2 - 1);
            if (p.lengthSquared() < 1) return p;
        }
    }
}