// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <cmath>

namespace Loom
{
    struct Vector2
    {
        float X;
        float Y;

        constexpr Vector2() = default;

        constexpr Vector2(float x, float y)
            : X(x), Y(y) {}

        constexpr Vector2 operator+(const Vector2& Other) const
        {
            return { X + Other.X, Y + Other.Y };
        }

        constexpr Vector2 operator-(const Vector2& Other) const
        {
            return { X - Other.X, Y - Other.Y };
        }

        constexpr Vector2 operator*(const float Scalar) const
        {
            return { X * Scalar, Y * Scalar};
        }
    };
}

