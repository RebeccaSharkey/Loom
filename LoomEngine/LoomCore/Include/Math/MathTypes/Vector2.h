// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Types.h"
#include <cmath>

namespace Loom
{
    struct Vector2
    {
        float32 X = 0.0f;
        float32 Y = 0.0f;

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

